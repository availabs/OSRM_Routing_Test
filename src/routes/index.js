module.exports = {
	"/route/:start/:end": "osrm.route",
	"/route/:coords": "osrm.route",

	"/model": "osrm.model",

	"/health": "health",

	"/test": {
		controller: "test",
		method: "get"
	}
}