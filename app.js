const env = require("node-env-file");

const { createServer } = require("./src/server");

env("config/app.env");

const PORT = process.env.PORT || 5000;

const webpackConfig = require("./webpack.config")

createServer({ webpackConfig })
	.listen(PORT, () => {
		console.log(`<SERVER> server running on port ${ PORT }`)
	});