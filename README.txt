This is the README for Alex Muro!
If you are not Alex Muro, this might not make much sense!
If you are Alex Muro, this still might not make much sense!

"npm install" = install node modules
"setup_arizona.sh" = download and process OSM data for all of Arizona into OSRM data
"python tile_download.py --config=config.json" = downloads height maps as .png files
	use -h for help
	I will send a config file (it contains my mapzen key so I don't want it public)
	the bounding box in the config file is actually just a point (it's OK!)
		this is to guarantee a single tile is retrieved
"sudo apt-get install libsdl2-2.0" = instal SDl2
"sudo apt-get install libsdl2-dev" = install SDL2 dev libraries
"make" = builds the c++ stuff into out file "main"
"./main" = reads loaded tiles and generates an OSM XML file output to "test_1.osm"
"test_osm.sh" = process the self-generated OSM file into OSRM data
"npm start" = starts the test map web server
	requires "config/app.env" containing....
		OSRM_DATA=data/osm/arizona.osrm
	or...
		OSRM_DATA=data/test_osm/test_1.osrm