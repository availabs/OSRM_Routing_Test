import mapboxgl from 'mapbox-gl/dist/mapbox-gl'

import d3 from "d3"

import getUniqueId from "./getUniqueId"

const PAINT_PROPERTIES = [
	["fill-color", "fillColor", "fill"],
	["fill-opacity", "fillOpacity", "fill-opacity", "fillOpacity"]
]
const HIGHLIGHT_PAINT_PROPERTIES = [
	["highlight-fill-color", "highlightLineColor", "highlight-fill", "highlightStroke"],
	["highlight-fill-opacity", "highlightLineOpacity", "highlight-fill-opacity", "highlightStrokeOpacity"]
]

export default function() {
	let mapboxglMap = null,

		source = null,

		layerId = getUniqueId(),
		paint = {
			"fill-color": "#990000",
			"fill-opacity": 1,
			"fill-width": 6
		},
		layout = {
			visibility: "visible"
		},

		addHighlight = false,

		hlLayerId = getUniqueId(),
		hlPaint = {
			"fill-color": "#dd0000",
			"fill-opacity": 1,
			"fill-width": 8
		},
		hlLayout = {
			visibility: "visible"
		},

		mouseover = null,
		mousemove = null,
		mouseout = null,
		click = null;

	function layer(map) {
		if (map) {
			mapboxglMap = map;
			addLayerToMap();
			addHighlightToMap();
			if (mouseover) {
				initializeEvent("mouseover", mouseover);
			}
			if (mousemove) {
				initializeEvent("mousemove", mousemove);
			}
			if (mouseout) {
				initializeEvent("mouseout", mouseout);
			}
			if (click) {
				initializeEvent("click", click);
			}
			return;
		}
	}

	layer.mouseover = function(m) {
		if (!arguments.length) {
			return mouseover;
		}
		mouseover = m;
		if (mouseover && mapboxglMap) {
			initializeEvent("mouseover", m);
		}
		return layer;
	}
	layer.mousemove = function(m) {
		if (!arguments.length) {
			return mousemove;
		}
		mousemove = m;
		if (mousemove && mapboxglMap) {
			initializeEvent("mousemove", m);
		}
		return layer;
	}
	layer.mouseout = function(m) {
		if (!arguments.length) {
			return mouseout;
		}
		mouseout = m;
		if (mouseout && mapboxglMap) {
			initializeEvent("mouseout", m);
		}
		return layer;
	}
	layer.click = function(m) {
		if (!arguments.length) {
			return click;
		}
		click = m;
		if (click && mapboxglMap) {
			initializeEvent("click", m);
		}
		return layer;
	}

	layer.zoomToBounds = () => {
		let features = source && source.features();
		if (mapboxglMap && features && features.length) {
			let bounds = getBounds(features);
		    mapboxglMap.fitBounds(bounds,
		    	{ padding: 20 });
		}
		return layer;
	}

	layer.getId = () => {
		return layerId;
	}

	layer.source = function(d) {
		if (!arguments.length) {
			return source;
		}
		source = d;
		addLayerToMap();
		addHighlightToMap();
		return layer;
	}
	layer.features = function(d) {
		if (!arguments.length) {
			return source.features;
		}
		if (source) {
			source.features(d);
		}
		return layer;
	}
	layer.collection = function(d) {
		if (!arguments.length) {
			return source.collection();
		}
		if (source) {
			source.collection(d);
		}
		return layer;
	}

	layer.paint = function(d) {
		if (!arguments.length) {
			return paint;
		}
		paint = d;
		if (mapboxglMap) {
			for (const k in d) {
				layer.paintProperty(k, d[k]);
			}
		}
		return layer;
	}
	layer.paintProperty = function(n, d) {
		if (arguments.length === 1) {
			return paint[n];
		}
		paint[n] = d;
		if (mapboxglMap) {
			mapboxglMap.setPaintProperty(layerId, n, d);
		}
		return layer;
	}
	PAINT_PROPERTIES.forEach(array => {
		array.forEach((p, i, a) => {
			if (i === 0) {
				layer[p] = layer.paintProperty.bind(null, p);
			}
			else {
				layer[p] = layer[a[0]];
			}
		})
	})

	layer.addHighlight = function(b) {
		if (!arguments.length) {
			return addHighlight;
		}
		addHighlight = b;
		addHighlightToMap();
		return layer;
	}
	layer.hlPaint = function(d) {
		if (!arguments.length) {
			return hlPaint;
		}
		hlPaint = d;
		return layer;
	}
	layer.hlPaintProperty = function(n, d) {
		if (arguments.length === 1) {
			return hlPaint[n];
		}
		hlPaint[n] = d;
		if (mapboxglMap) {
			mapboxglMap.setPaintProperty(hlLayerId, n, d);
		}
		return layer;
	}
	HIGHLIGHT_PAINT_PROPERTIES.forEach(array => {
		array.forEach((p, i, a) => {
			if (i === 0) {
				layer[p] = layer.hlPaintProperty.bind(null, p.slice(10));
			}
			else {
				layer[p] = layer[a[0]];
			}
		})
	})
	// args = (comp, prop, ...values)
	layer.highlight = function(arg1, arg2, ...args) {
		mapboxglMap.setFilter(hlLayerId, [arg2, arg1, ...args]);
	}

	return layer;

	function addLayerToMap() {
		if (source && mapboxglMap && !mapboxglMap.getLayer(layerId)) {
			mapboxglMap.addLayer({
				id: layerId,
				type: "fill",
				source: source.getId(),
				paint,
				layout
			});
		}
	}
	function addHighlightToMap() {
		if (addHighlight && source && mapboxglMap && !mapboxglMap.getLayer(hlLayerId)) {
			mapboxglMap.addLayer({
				id: hlLayerId,
				type: "fill",
				source: source.getId(),
				filter: ["in", "", ""],
				paint: hlPaint,
				layout: hlLayout
			});
		}
		else if (!addHighlight && source && mapboxglMap && mapboxglMap.getLayer(hlLayerId)) {
			mapboxglMap.removeLayer(hlLayerId);
		}
	}

	function wrapEventFunction(func) {
		return e => {
			let features = mapboxglMap.queryRenderedFeatures(e.point, { layers: [layerId] });
			if (features.length) {
				mapboxglMap.getCanvas().style.cursor = 'pointer';
			}
			else {
				mapboxglMap.getCanvas().style.cursor = 'default';
			}
			func.call(layer, e, features, map);
		}
	}
	function initializeEvent(event, func) {
		mapboxglMap.on(event, layerId, wrapEventFunction(func));
	}
}

const getBounds = features => {
	let coordinates = d3.merge(features.map(d => getGeometryCoordinates(d.geometry)));
	return coordinates.reduce((a, c) => {
		return a.extend(c);
	}, new mapboxgl.LngLatBounds(coordinates[0], coordinates[0]));
}

const getGeometryCoordinates = geometry => {
	switch (geometry.type.toLowerCase()) {
		case "point":
			return [geometry.coordinates];
		case "linestring":
		case "polygon":
			return geometry.coordinates;
		case "multilinestring":
		case "multipolygon":
			return d3.merge(geometry.coordinates);
	}
}