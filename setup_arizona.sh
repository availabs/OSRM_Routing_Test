#!/bin/bash

OSM_DATA="data/osm/arizona.osm.pbf"
OSRM_DATA="data/osm/arizona.osrm"

curl --create-dirs --output $OSM_DATA http://download.geofabrik.de/north-america/us/arizona-latest.osm.pbf

node_modules/osrm/lib/binding/osrm-extract $OSM_DATA -p node_modules/osrm/profiles/car.lua > arizona_extract_log.txt
node_modules/osrm/lib/binding/osrm-contract $OSRM_DATA > arizona_contract_log.txt