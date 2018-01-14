#!/bin/bash

FOLDER=$1
FULL_FILE=$2

REGEX="^([a-zA-Z0-9_]+).osm$"

if [[ $FULL_FILE =~ $REGEX ]]
then
    FILE_NAME=${BASH_REMATCH[1]}
    echo $FILE_NAME
else
    echo Inappropriate .osm file name.
    echo 'usage: process_osm.sh <FOLDER> <FILE_NAME.osm>'
    exit 1
fi

mkdir -p data/$FOLDER/
rm data/$FOLDER/*
cp $FULL_FILE data/$FOLDER/

OSM_DATA=data/$FOLDER/$FILE_NAME.osm
OSRM_DATA=data/$FOLDER/$FILE_NAME.osrm

echo
echo STARTING EXTRACTION
time node_modules/osrm/lib/binding/osrm-extract $OSM_DATA -p src/profiles/test_profile.lua

echo
echo STARTING PARTITION
time node_modules/osrm/lib/binding/osrm-partition $OSRM_DATA

echo
echo STARTING CUSTOMIZE
time node_modules/osrm/lib/binding/osrm-customize $OSRM_DATA
