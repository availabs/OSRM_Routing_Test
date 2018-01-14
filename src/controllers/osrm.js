"use strict"

const esc = require("esc"),

	osrm = require("osrm"),
	
	getLatLng = require("../utils/getLatLng");

let OSRM = null;
try {
	const options = {
		path: process.env.OSRM_DATA,
		algorithm: "MLD"
	};
	OSRM = new osrm(options);
	console.log("<OSRMcontroller> Started OSRM backend.\n");
}
catch (error) {
	console.log("<OSRMcontroller> Could not start OSRM backend.\n");
}

let OSRM_model = null;
try {
	const options = {
		// path: process.env.OSRM_DATA,
		path: process.env.OSRM_MODEL,
		algorithm: "MLD"
	}
	OSRM_model = new osrm(options);
	console.log("<OSRMcontroller> Started OSRM model.\n");
}
catch (error) {
	console.log("<OSRMcontroller> Could not start OSRM model.\n");
}

const splitCoords = coords => coords.split(";").reduce((a, c) => (a.push(c.split(",").map(d => +d)), a), []);

const MODEL_TILES = [
	[7, 23, 51],
	[7, 24, 51],
	[7, 23, 52],
	[7, 24, 52],
	[7, 25, 52]
]

let MODEL_RUNNING = false,
	LOOPS,
	SUCCESSES,
	FAILURES,
	TIME_START,
	TIME_END;

let MODEL_OPTIONS = {
	coordinates: null,
	alternatives: false,
	steps: true,
    annotations: true,
    geometries: "geojson",
    overview: "full",
	continue_straight: false
};
const runModel = (OD_Pairs) => {
	MODEL_OPTIONS.coordinates = OD_Pairs.pop();
    OSRM_model.route(MODEL_OPTIONS, (err, result) => {
      	if (!err) {
      		++SUCCESSES;
      	}
      	else{
      		console.log(err);
      		++FAILURES;
      	}

      	if (OD_Pairs.length) {
      		runModel(OD_Pairs);
      	}
      	else {
      		TIME_END = Date.now();
      	}
    });
}

module.exports = {
	model: (req, res) => {
		if (!OSRM_model) {
			return res.status(500).json({ msg: "OSRM Model was not initialized." });
		}

		if (MODEL_RUNNING) {
			let response = { msg: "OSRM Model is running.",
				SUCCESSES, FAILURES, time: Date.now() - TIME_START
			}
			if ((FAILURES + SUCCESSES) == LOOPS) {
				MODEL_RUNNING = false;
				let TOTAL_TIME = TIME_END - TIME_START;
				response.TOTAL_TIME = TOTAL_TIME;
			}
			res.status(200).json(response);
			return;
		}
		else {
			MODEL_RUNNING = true;
			LOOPS = 10000;
			SUCCESSES = 0;
			FAILURES = 0;
			TIME_START = Date.now();
			TIME_END = 0;
			res.status(200).json({ msg: "OSRM Model has started." });
		}

		const westTile = MODEL_TILES[0],
			eastTile = MODEL_TILES[MODEL_TILES.length - 1];

		let OD_Pairs = []
		for (let i = 1; i <= LOOPS; ++i) {
			let r = esc.randInt(0, 255) / 256,
				c = esc.randInt(0, 32) / 256;
			const origin = getLatLng(westTile[0], westTile[1] + c, westTile[2] + r);
			r = esc.randInt(0, 255) / 256;
			c = esc.randInt(223, 255) / 256;
			const dest = getLatLng(eastTile[0], eastTile[1] + c, eastTile[2] + r);

			OD_Pairs.push([[origin.lng, origin.lat], [dest.lng, dest.lat]]);
		}
		runModel(OD_Pairs);
	},

	route: (req, res) => {
		if (!OSRM) {
			return res.status(500).json({ msg: "OSRM was not initialized." });
		}

		const start = req.params.start,
			end = req.params.end,

			coords = req.params.coords || `${ start };${ end }`,

		  	options = {
			    coordinates: splitCoords(coords),
			    alternatives: false,

			    // Return route steps for each route leg
			    steps: false,

			    // Return annotations for each route leg
			    annotations: false,

			    // Returned route geometry format. Can also be geojson
			    geometries: "geojson",

			    // Add overview geometry either full, simplified according to
			    // highest zoom level it could be display on, or not at all
			    overview: "full",

			    // Forces the route to keep going straight at waypoints and don't do
			    // a uturn even if it would be faster. Default value depends on the profile
			    continue_straight: false
		  	};

	  	try {
		    OSRM.route(options, (err, result) => {
		      	if (err) {
		        	return res.status(422).json({ error: err.message });
		      	}
		      	return res.status(200).json(result);
		    });
	  	}
	  	catch (error) {
	  		res.status(500).json({ error });
	  	}
	}
}