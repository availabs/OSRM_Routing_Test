const path = require("path");
module.exports = {
  	entry: path.resolve(__dirname, 'src/main.js'),
  	output: {
    	filename: 'bundle.js',
    	path: path.resolve(__dirname, 'dist')
  	},
  	resolve: {
  		extensions: [".js", ".jsx", ".json"]
  	},
  	module: {
  		rules: [
  			{ test: /\.jsx?$/,
  				include: [path.resolve(__dirname, "src")],
  				loader: 'babel-loader',
  				options: {
  					presets: ['react']
  				}
  			}
  		]
  	}
};