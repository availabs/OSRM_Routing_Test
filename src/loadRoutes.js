const express = require("express");

const routes = require("./routes");

const DEFAULT_METHOD = "get";

const getControllerFunc = controllerString => {
	let split = controllerString.split(".");
	if (split.length == 2) {
		const controller = require(`./controllers/${ split[0] }`);
		return controller[split[1]];
	}
	else {
		return require(`./controllers/${ split[0] }`);
	}
}

const loadRoutes = app => {
	const router = express.Router();

	for (const path in routes) {
		let pathData = routes[path],

			method = DEFAULT_METHOD,

			func = undefined;

		if (typeof pathData == "string") {
			func = getControllerFunc(pathData);
		}
		else {
			method = pathData.method || method;
			func = getControllerFunc(pathData.controller);
		}


		if (path && func) {
			router[method](path, func);
		}
	};

	app.use(router);
}

module.exports = loadRoutes;