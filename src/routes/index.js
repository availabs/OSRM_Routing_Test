module.exports = {
	"/route/:start/:end": "osrm.route",
	"/route/:coords": "osrm.route",

	"/health": "health",

	"/test": {
		controller: "test",
		method: "get"
	}
}