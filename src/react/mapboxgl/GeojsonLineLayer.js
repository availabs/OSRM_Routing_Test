import mapboxgl from 'mapbox-gl/dist/mapbox-gl'

import { map as d3_map, set as d3_set } from 'd3'

import getUniqueId from './getUniqueId'

const PAINT_PROPERTIES = [
	['line-color', 'lineColor', 'stroke'],
	['line-opacity', 'lineOpacity', 'stroke-opacity', 'strokeOpacity'],
	['line-width', 'lineWidth', 'stroke-width', 'strokeWidth']
]
const HIGHLIGHT_PAINT_PROPERTIES = [
	['highlight-line-color', 'highlightLineColor', 'highlight-stroke', 'highlightStroke'],
	['highlight-line-opacity', 'highlightLineOpacity', 'highlight-stroke-opacity', 'highlightStrokeOpacity'],
	['highlight-line-width', 'highlightLineWidth', 'highlight-stroke-width', 'highlightStrokeWidth']
]

export default function () {
  let mapboxglMap = null,

    stoppers = d3_map(),

    source = null,

    layerId = getUniqueId(),
    paint = {
      'line-color': '#990000',
      'line-opacity': 1,
      'line-width': {
        default: 6,
        type: 'categorical',
        property: 'tmc',
        stops: [['123N45678', 6]]
      }
    },
    layout = {
      'line-join': 'round',
      'line-cap': 'round',
      visibility: 'visible'
    },

    addHighlight = false,

    highlightProperties = d3_set(),

    hlLayerId = getUniqueId(),
		// hlPaint = {
		// 	"line-color": "#dd0000",
		// 	"line-opacity": 1,
		// 	"line-width": 8
		// },
		// hlLayout = {
		// 	"line-join": "round",
		// 	"line-cap": "round",
		// 	visibility: "visible"
		// },

    zoomToBounds = false,

    mouseover = null,
    mousemove = null,
    mouseout = null,
    click = null,

    cursor = 'default'

  function layer (map) {
    if (map) {
      mapboxglMap = map
      addLayerToMap()
      addHighlightToMap()
      if (mouseover) {
        initializeEvent('mouseover', mouseover)
      }
      if (mousemove) {
        initializeEvent('mousemove', mousemove)
      }
      if (mouseout) {
        initializeEvent('mouseout', mouseout)
      }
      if (click) {
        initializeEvent('click', click)
      }
      cursor = mapboxglMap.getCanvas().style.cursor
      if (zoomToBounds) layer.zoomToBounds()
      return
    }
    stoppers.each(stopper => {
      stopper(layer)
    })
  }

  layer.stopper = function (s) {
    const id = s.getId()
    if (stoppers.has(id)) {
      stoppers.remove(id)
    } else {
      stoppers.set(id, s)
    }
    return layer
  }

  layer.mouseover = function (m) {
    if (!arguments.length) {
      return mouseover
    }
    mouseover = m
    if (mouseover && mapboxglMap) {
      initializeEvent('mouseover', m)
    }
    return layer
  }
  layer.mousemove = function (m) {
    if (!arguments.length) {
      return mousemove
    }
    mousemove = m
    if (mousemove && mapboxglMap) {
      initializeEvent('mousemove', m)
    }
    return layer
  }
  layer.mouseout = function (m) {
    if (!arguments.length) {
      return mouseout
    }
    mouseout = m
    if (mouseout && mapboxglMap) {
      initializeEvent('mouseout', m)
    }
    return layer
  }
  layer.click = function (m) {
    if (!arguments.length) {
      return click
    }
    click = m
    if (click && mapboxglMap) {
      initializeEvent('click', m)
    }
    return layer
  }

  layer.zoomToBounds = () => {
    let features = source && source.features()
    zoomToBounds = true
    if (mapboxglMap && features && features.length) {
      let bounds = getBounds(features)
		    mapboxglMap.fitBounds(bounds,
		    	{ padding: 20 })
		    zoomToBounds = false
    }
    return layer
  }

  layer.getId = () => {
    return layerId
  }

  layer.source = function (d) {
    if (!arguments.length) {
      return source
    }
    source = d
    addLayerToMap()
    addHighlightToMap()
    return layer
  }
  layer.features = function (d) {
    if (arguments.length) {
      return source.features
    }
    if (source) {
      source.features(d)
    }
    return layer
  }
  layer.collection = function (d) {
    if (!arguments.length) {
      return source.collection()
    }
    if (source) {
      source.collection(d)
    }
    return layer
  }

  layer.paint = function (d) {
    if (!arguments.length) {
      return paint
    }
    paint = d
    if (mapboxglMap) {
      for (const k in d) {
        layer.paintProperty(k, d[k])
      }
    }
    return layer
  }
  layer.paintProperty = function (n, d) {
    if (arguments.length === 1) {
      return paint[n]
    }
		// if (typeof d === "function") {
		// 	d(layer);
		// }
		// else {
    paint[n] = d
    if (mapboxglMap) {
      mapboxglMap.setPaintProperty(layerId, n, d)
    }
		// }
    return layer
  }
  PAINT_PROPERTIES.forEach(array => {
    array.forEach((p, i, a) => {
      if (i === 0) {
        layer[p] = layer.paintProperty.bind(null, p)
      } else {
        layer[p] = layer[a[0]]
      }
    })
  })

  layer.addHighlight = function (b) {
		// if (!arguments.length) {
		// 	return addHighlight;
		// }
		// addHighlight = b;
		// addHighlightToMap();
    return layer
  }
  layer.hlPaint = function (d) {
		// if (!arguments.length) {
		// 	return hlPaint;
		// }
		// hlPaint = d;
		// if (mapboxglMap && addHighlight) {
		// 	for (const k in d) {
		// 		layer.hlPaintProperty(k, d[k]);
		// 	}
		// }
    return layer
  }
  layer.hlPaintProperty = function (n, d) {
		// if (arguments.length === 1) {
		// 	return hlPaint[n];
		// }
		// hlPaint[n] = d;
		// if (mapboxglMap && addHighlight) {
		// 	mapboxglMap.setPaintProperty(hlLayerId, n, d);
		// }
    return layer
  }
  HIGHLIGHT_PAINT_PROPERTIES.forEach(array => {
    array.forEach((p, i, a) => {
      if (i === 0) {
        layer[p] = layer.hlPaintProperty.bind(null, p.slice(10))
      } else {
        layer[p] = layer[a[0]]
      }
    })
  })

  layer.highlight = function (property, properties) {
		// mapboxglMap.setFilter(hlLayerId, [arg2, arg1, ...args]);
    layer.pointer(Boolean(properties.length))

    let mustHighlight = !properties.length && highlightProperties.size()

    properties.forEach(d => {
      if (!highlightProperties.has(d)) {
        mustHighlight = true
      }
      highlightProperties.add(d)
    })

    if (!mustHighlight) return layer

    let stops = [['none', 4]]
    if (properties.length) {
      stops = properties.map(d => [d, 10])
    }
    mapboxglMap.setPaintProperty(layerId, 'line-width', {
      type: 'categorical',
      default: 4,
      property,
      stops
    })

    stops = [['none', 0.75]]
    if (properties.length) {
      stops = properties.map(d => [d, 1.0])
    }
    mapboxglMap.setPaintProperty(layerId, 'line-opacity', {
      type: 'categorical',
      default: 0.75,
      property,
      stops
    })

    if (!properties.length) {
      highlightProperties = d3_set()
    }
  }

  layer.pointer = function (b) {
    if (b) {
      cursor = mapboxglMap.getCanvas().style.cursor
      mapboxglMap.getCanvas().style.cursor = 'pointer'
    } else {
      mapboxglMap.getCanvas().style.cursor = cursor
    }
    return layer
  }

  return layer

  function addLayerToMap () {
    if (source && mapboxglMap && !mapboxglMap.getLayer(layerId)) {
      mapboxglMap.addLayer({
        id: layerId,
        type: 'line',
        source: source.getId(),
        paint,
        layout
      })
    }
  }
  function addHighlightToMap () {
    if (addHighlight && source && mapboxglMap && !mapboxglMap.getLayer(hlLayerId)) {
      mapboxglMap.addLayer({
        id: hlLayerId,
        type: 'line',
        source: source.getId(),
        filter: ['in', '', ''],
        paint: hlPaint,
        layout: hlLayout
      })
    } else if (!addHighlight && source && mapboxglMap && mapboxglMap.getLayer(hlLayerId)) {
      mapboxglMap.removeLayer(hlLayerId)
    }
  }

  function wrapEventFunction (func) {
    return e => {
      let features = mapboxglMap.queryRenderedFeatures(e.point, { layers: [layerId] })
			// if (features.length) {
			// 	mapboxglMap.getCanvas().style.cursor = 'pointer';
			// }
			// else {
			// 	mapboxglMap.getCanvas().style.cursor = 'default';
			// }
      func.call(layer, e, features, map)
    }
  }
  function initializeEvent (event, func) {
    mapboxglMap.on(event, layerId, wrapEventFunction(func))
  }
}

const getBounds = features => {
  let coordinates = d3.merge(features.map(d => getGeometryCoordinates(d.geometry)))
  return coordinates.reduce((a, c) => {
    return a.extend(c)
  }, new mapboxgl.LngLatBounds(coordinates[0], coordinates[0]))
}

const getGeometryCoordinates = geometry => {
  switch (geometry.type.toLowerCase()) {
    case 'point':
      return [geometry.coordinates]
    case 'linestring':
    case 'polygon':
      return geometry.coordinates
    case 'multilinestring':
    case 'multipolygon':
      return d3.merge(geometry.coordinates)
  }
}
