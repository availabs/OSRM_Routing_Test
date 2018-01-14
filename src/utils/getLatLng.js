const TO_RAD = Math.PI / 180.0,
	TO_DEG = 180.0 / Math.PI;

const getLng = (zoom, x3) => {
	let tiles = Math.pow(2.0, zoom),
		diameter = Math.PI * 2.0,

		x2 = x3 * diameter / tiles - Math.PI;
	return x2 * TO_DEG;
}
const getLat = (zoom, y3) => {
	let tiles = Math.pow(2.0, zoom),
		diameter = Math.PI * 2.0,

		y2 = Math.PI - y3 * diameter / tiles,
		y1 = 2.0 * (Math.atan(Math.exp(y2)) - 0.25 * Math.PI);
	return y1 * TO_DEG;
}

const LatLng = (lat, lng) => {
	let latLng = { lat, lng };
	latLng.toRadians = () => ({
		lat: lat * TO_RAD,
		lng: lng * TO_RAD
	})
	return latLng;
}

const getLatLng = (z, x, y) => {
	return LatLng(getLat(z, y), getLng(z, x));
}

module.exports = getLatLng;