const osrm = require("osrm");

const OSRM_OPTIONS = {
	path: process.env.OSRM_DATA,
	algorithm: "MLD"
}
const OSRM = new osrm(OSRM_OPTIONS);

const splitCoords = coords => coords.split(";").reduce((a, c) => (a.push(c.split(",").map(d => +d)), a), []);

module.exports = {
	route: (req, res) => {
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