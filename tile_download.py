#!/usr/bin/env python

from __future__ import print_function
from math import log, tan, pi, exp, atan
from itertools import product
from os.path import join, isdir, exists, isfile
from os import listdir, mkdir, remove

from argparse import ArgumentParser

from time import sleep

import json, io, tempfile, shutil, urllib, re, threading, sys

def parseBoundingBoxes(bounding_boxes_string):
	# split string into separate bounding boxes
	step1 = bounding_boxes_string.split(";")

	# split each bounding box string into coords
	step2 = [bbox.split(",") for bbox in step1]

	# coerce coords of each bounding box to floats
	# each bounding box must contain 4 coords, 2 pairs of lat & lng
	step3 = [[float(c) for c in coords] for coords in step2]

	return step3

def mercator(zoom, lat, lng):
	# convert to radians
	x1 = lng * pi / 180
	y1 = lat * pi / 180

	# project to mercator
	x2 = x1
	y2 = log(tan(0.25 * pi + 0.5 * y1))

	tiles = 2 ** zoom
	diameter = 2 * pi

	# tranform to tile space
	x3 = int(tiles * (x2 + pi) / diameter)
	y3 = int(tiles * (pi - y2) / diameter)

	return zoom, x3, y3

def latLng(zoom, x3, y3):
	tiles = 2 ** zoom
	diameter = 2 * pi

	x2 = x3 * diameter / tiles - pi
	y2 = -(y3 * diameter / tiles - pi)

	x1 = x2
	y1 = 2 * (atan(exp(y2)) - 0.25 * pi)

	lng = x1 * 180 / pi
	lat = y1 * 180 / pi

	return zoom, lat, lng

def getTiles(zoom, lat1, lng1, lat2=None, lng2=None):
	tiles = []
	if lat2 and lng2:
		minLat = min(lat1, lat2)
		minLng = min(lng1, lng2)

		maxLat = max(lat1, lat2)
		maxLng = max(lng1, lng2)

		z, xMin, yMin = mercator(zoom, maxLat, minLng)
		x, xMax, yMax = mercator(zoom, minLat, maxLng)

		xRange = range(xMin, xMax + 1)
		yRange = range(yMin, yMax + 1)

		tiles = [(zoom, x, y) for (x, y) in product(xRange, yRange)]
	else:
		z, x, y = mercator(zoom, lat1, lng1)

		tiles = [mercator(zoom, lat1, lng1)]

	return tiles
	
class ProgressBar(threading.Thread):
	def __init__(self, tiles):
		threading.Thread.__init__(self)
		self.tiles = tiles
		self.total = len(tiles)

	def run(self):
		numBars = 50
		while True:

			ratio = 1 - float(len(self.tiles)) / float(self.total)
			num = int(ratio * numBars)

			filled = "#" * num
			unfilled = "-" * (numBars - num)
			print("\rPROGRESS: |{}{}| {}%".format(filled, unfilled, round(ratio * 1000.0) * 0.1), end="\r")
			
			sys.stdout.flush()
			
			if num == numBars: break

			sleep(0.05)

		print("\rFINISHED\r")

class DownloadThread(threading.Thread):
	TILE_URL = 'https://s3.amazonaws.com/elevation-tiles-prod/terrarium/{z}/{x}/{y}.png'
	
	def __init__(self, out_dir, tiles, api_key, lock):
		threading.Thread.__init__(self)
		self.tiles = tiles
		self.totalTiles = len(tiles)
		self.lock = lock
		self.tile = None
		self.out_dir = out_dir
		self.api_key = api_key

	def run(self):
		while True:
			with self.lock:
				if len(self.tiles):
					self.tile = self.tiles.pop()

			if self.tile:
				self.download()
			else:
				break

	def download(self):
		z, x, y = self.tile
		k = self.api_key
		url = DownloadThread.TILE_URL.format(z=z, x=x, y=y)

		response = urllib.urlopen(url)

		if response.getcode() == 200:
			ratio = 1 - float(len(self.tiles)) / float(self.totalTiles)
			num = int(ratio * 10.0)
			bars = ["#"] * num
			print("{}".format("".join(bars)), end="\r")

			with io.open(join(self.out_dir, "tile-{}-{}-{}.png".format(z, x, y)), "wb") as fp:
				fp.write(response.read())
		else:
			print("<{}> Failed to download tile: {}-{}-{}".format(self.id, z, x, y))

		self.tile = None

class DataDownloader:
	def __init__(self, numThreads=4):
		self.numThreads = numThreads

	def download(self, out_dir, tiles, api_key=None):
		if not len(tiles):
			return print("<DataDownloader> No tiles required to be downloaded.")

		print("<DataDownloader> {} tile{} required to be downloaded.".format(len(tiles), "s" * bool(len(tiles) != 1)))
		lock = threading.Lock()

		threads = [ProgressBar(tiles)] + [DownloadThread(out_dir, tiles, api_key, lock) for i in range(self.numThreads)]

		if not exists(out_dir):
			mkdir(out_dir)

		for thread in threads:
			thread.start()
		for thread in threads:
			thread.join()

		return

REQUIRED_ARGS = set(["zoom", "bounding_boxes", "out_dir", "num_threads"])

parser = ArgumentParser(description="For downloading elevation data.")

parser.add_argument('--zoom', '-z', type=int, default=12, help="Tile zoom level. Defaults to 12.")
parser.add_argument('--bounding_boxes', '-b', default='31.45447192473823,-110.96770162008457',
				help='''List of bounding boxes. Defaults to a point in southern Arizona.''')
parser.add_argument('--out_dir', '-o', default="downloaded_tiles", help="output directory.")
parser.add_argument('--num_threads', '-t', type=int, default=4, help="Number of threads created. Defaults to 8.")

parser.add_argument('--config', '-c', default=None,
				help='''Name of config file. Any supplied command 
					line arguments will override values in config file.''')

def checkArgs(args):
	required = REQUIRED_ARGS.copy()

	for (k, v) in args.items():
		if k in required: required.remove(k)

	if len(required):
		raise RuntimeError("Missing arguments: {}".format(",".join(required)))

def checkOutDir(out_dir, tileSet):
	if exists(out_dir) and isdir(out_dir):
		files = [f for f in listdir(out_dir) if isfile(join(out_dir, f))]

		oldTiles = []

		regex = re.compile('^tile-(\d+-\d+-\d+).png$')

		for f in files:
			m = regex.match(f)
			if m:
				k = m.group(1)
				if k in tileSet:
					del tileSet[k]
				else:
					oldTiles.append(f)

		for tile in oldTiles:
			path = join(out_dir, tile)
			print("Removing extraneous tile: {}.".format(path))
			remove(path)

def main():
	args = vars(parser.parse_args())

	if args["config"]:
		print("Loading config file:", args["config"])
		with io.open(args["config"]) as fp:
			config = json.load(fp)
			args.update(config)

	checkArgs(args)

	zoom = args["zoom"]
	bboxes = parseBoundingBoxes(args["bounding_boxes"])

	tileSet = {}

	print("Generating tile list.")
	for bbox in bboxes:
		tiles = getTiles(zoom, *bbox)
		for tile in tiles:
			key = "{}-{}-{}".format(*tile)
			tileSet[key] = tile

	print("Checking output directory.")

	checkOutDir(args["out_dir"], tileSet)
	tileList = tileSet.values()

	DataDownloader(args["num_threads"]).download(args["out_dir"], tileList)


if __name__ == "__main__":
	main()