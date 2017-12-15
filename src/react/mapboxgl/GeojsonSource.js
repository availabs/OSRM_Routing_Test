import getUniqueId from "./getUniqueId"

export default function() {
	let mapboxglMap = null,

		sourceId = getUniqueId(),

		features  = [];

	const makeCollection = () => ({ type: "FeatureCollection", features });

	function source(map) {
		if (map) {
			mapboxglMap = map;
			mapboxglMap.addSource(sourceId, {
				type: "geojson",
				data: source.collection()
			})
			return;
		}
	}

	source.getId = () => {
		return sourceId;
	}

	source.features = function(d) {
		if (!arguments.length) {
			return features;
		}
		features = d;
		if (mapboxglMap) {
			mapboxglMap.getSource(sourceId)
				.setData(makeCollection());
		}
		return source;
	}
	source.collection = function(d) {
		if (!arguments.length) {
			return makeCollection();
		}
		return source.features(d.features);
	}

	return source;
}