import mapboxgl from 'mapbox-gl/dist/mapbox-gl'

import d3 from 'd3'

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

export default () => {
  let mapboxglMap = null,

    stoppers = d3.map(),

    source = null,

    filterKey = 'Tmc',
    filterValues = [],

    zoomBounds = [],
    zoomToBounds = false,

    layerId = 'inrix_npmrds',
    paint = {
      'line-color': '#990000',
      'line-opacity': 0.75,
      'line-width': 4
    },
    layout = {
      'line-join': 'round',
      'line-cap': 'round',
      visibility: 'visible'
    },

    hlPaint = {
      'line-opacity': 1,
      'line-width': 10
    },

    mouseover = null,
    mousemove = null,
    mouseout = null,
    click = null,

    cursor = 'default';

  const makeFilter = () => ['in', filterKey].concat(filterValues);

  function layer (map) {
    if (map) {
      mapboxglMap = map;
      addLayerToMap();
      if (mouseover) {
        initializeEvent('mouseover', mouseover);
      }
      if (mousemove) {
        initializeEvent('mousemove', mousemove);
      }
      if (mouseout) {
        initializeEvent('mouseout', mouseout);
      }
      if (click) {
        initializeEvent('click', click);
      }
      cursor = mapboxglMap.getCanvas().style.cursor;
      if (zoomToBounds) layer.zoomToBounds();
      return
    }
    stoppers.forEach((id, stopper) => stopper(layer));
  }

  layer.filterKey = d => {
    if (!arguments.length) {
      return filterKey;
    }
    filterKey = d;
    return layer;
  }
  layer.filterValues = d => {
    if (!arguments.length) {
      return filterValues;
    }
    filterValues = d;
    if (mapboxglMap) {
      mapboxglMap.setFilter(layerId, makeFilter());
    }
    return layer;
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

  layer.zoomToBounds = function (bounds) {
    if (arguments.length) {
      zoomBounds = bounds;
    }
console.log("<VectorTileLayer.zoomToBounds>",bounds,arguments.length)
    zoomToBounds = true;
    if (mapboxglMap && zoomBounds.length) {
      mapboxglMap.fitBounds(zoomBounds,
		    { padding: 20 });
      zoomToBounds = false;
    }
    return layer;
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
    return layer;
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
    paint[n] = d
    if (mapboxglMap) {
      mapboxglMap.setPaintProperty(layerId, n, d);
    }
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

  layer.hlPaint = function (d) {
		if (!arguments.length) {
			return hlPaint;
		}
		hlPaint = d;
    return layer
  }
  layer.hlPaintProperty = function (n, d) {
		if (arguments.length === 1) {
			return hlPaint[n];
		}
		hlPaint[n] = d;
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

  layer.highlight = function (property, values) {
console.log("<VectorTileLayer.highlight> START",property,values)
    for (const paintProperty in hlPaint) {
      let defaultValue = paint[paintProperty],
        hlValue = hlPaint[paintProperty],
        stops = [['--none--', hlValue]];
      if (values.length) {
        stops = values.map(d => [d, hlValue]);
      }
      if (mapboxglMap && defaultValue) {
console.log("<VectorTileLayer.highlight> PAINT:",paintProperty,stops)
        mapboxglMap.setPaintProperty(layerId, paintProperty, {
          type: 'categorical',
          default: defaultValue,
          property,
          stops
        });
      }
    }
    return layer;
  }

  layer.pointer = function (b) {
    if (mapboxglMap) {
      let canvas = mapboxglMap.getCanvas();
      if (b) {
        cursor = canvas.style.cursor;
        canvas.style.cursor = 'pointer';
      } else {
        canvas.style.cursor = cursor;
      }
    }
    return layer;
  }

  return layer

  function addLayerToMap () {
    if (source && mapboxglMap && !mapboxglMap.getLayer(layerId)) {
      let filter = makeFilter();
      mapboxglMap.addLayer({
        id: layerId,
        type: 'line',
        source: source.getId(),
        'source-layer': source.getLayerId(),
        paint,
        filter,
        layout
      })
    }
  }

  function wrapEventFunction (func) {
    return e => {
      let features = mapboxglMap.queryRenderedFeatures(e.point, { layers: [layerId] })
      func.call(layer, e, features, mapboxglMap)
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
