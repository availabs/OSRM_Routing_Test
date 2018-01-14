#!/bin/bash

OSM_DATA="data/arizona/arizona.osm.pbf"
OSRM_DATA="data/arizona/arizona.osrm"

curl --create-dirs --output $OSM_DATA http://download.geofabrik.de/north-america/us/arizona-latest.osm.pbf

node_modules/osrm/lib/binding/osrm-extract $OSM_DATA -p node_modules/osrm/profiles/car.lua
node_modules/osrm/lib/binding/osrm-contract $OSRM_DATA