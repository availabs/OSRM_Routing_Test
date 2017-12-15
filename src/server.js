const http = require("http");
const express = require("express");
const bodyParser = require("body-parser");
const helmet = require("helmet");
// const logfmt = require("logfmt");

const Webpack = require("webpack");
const webpackMiddleware = require("webpack-dev-middleware");

const loadRoutes = require("./loadRoutes");

function configureMiddleware(server, options) {
  server.use(helmet());
  server.use(bodyParser.json());
  server.use(bodyParser.urlencoded({ extended: false }));
  // server.use(logfmt.requestLogger());
  const webpack = Webpack(options.webpackConfig);
  server.use(webpackMiddleware(webpack, {
    publicPath: "/",
    index: "index.html",
    lazy: false
  }));
}

function createServer(options) {
  const server = express();
  configureMiddleware(server, options);
  loadRoutes(server);
  server.use("/", express.static("src"));
  return http.createServer(server);
}

module.exports = {
  createServer
};