#!/bin/bash

mkdir -p data/test_osm/
rm data/test_osm/test_1*
cp test_1.osm data/test_osm/

OSM_DATA=data/test_osm/test_1.osm
OSRM_DATA=data/test_osm/test_1.osrm

#node_modules/osrm/lib/binding/osrm-extract $OSM_DATA -p node_modules/osrm/profiles/foot.lua
node_modules/osrm/lib/binding/osrm-extract $OSM_DATA -p src/profiles/test_profile.lua
#node_modules/osrm/lib/binding/osrm-contract $OSRM_DATA
node_modules/osrm/lib/binding/osrm-partition $OSRM_DATA
node_modules/osrm/lib/binding/osrm-customize $OSRM_DATA
