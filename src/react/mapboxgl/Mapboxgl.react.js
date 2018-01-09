import React from 'react'
import mapboxgl from 'mapbox-gl/dist/mapbox-gl'

import { set, map } from 'd3'

class MapboxglMap extends React.Component {
	constructor(props) {
		super(props);

		this.state = {
			mapboxglMap: null,
			sources: map(),
			layers: map()
		}

		this.onMapLoad = this.onMapLoad.bind(this);
		this.updateMap = this.updateMap.bind(this);
		this.updateSources = this.updateSources.bind(this);
		this.updateLayers = this.updateLayers.bind(this);

		this.checkDragPan = this.checkDragPan.bind(this);
		this.checkScrollZoom = this.checkScrollZoom.bind(this);
	}

	componentDidMount() {
    	mapboxgl.accessToken = this.props.accessToken;

	    let mbglMap = new mapboxgl.Map({
	      	container: "map",
	      	style: this.props.style,
	      	center: this.props.center,
	      	zoom: this.props.zoom,
  			preserveDrawingBuffer: true
	    });

	    mbglMap.on("load", this.onMapLoad.bind(null, mbglMap));

		if (this.props.onClick) {
			mbglMap.on("click", this.props.onClick);
		}
	}

	componentWillReceiveProps(newProps) {
		this.updateMap(newProps);
	}

	onMapLoad(mbglMap) {
		let sources = map(),
			layers = map();

		this.props.sources.forEach(source => {
			source(mbglMap);
			sources.set(source.getId(), source);
		});
		this.props.layers.forEach(layer => {
			layer(mbglMap);
			layers.set(layer.getId(), layer);
		});

		this.setState({
			mapboxglMap: mbglMap,
			sources,
			layers
		});

		if (this.props.cursor) {
			mbglMap.getCanvas().style.cursor = this.props.cursor;
		}

		this.checkDragPan(this.props);
		this.checkScrollZoom(this.props);
	}
	updateMap(newProps) {
		if (!this.state.mapboxglMap) return;

		this.setState({
			sources: this.updateSources(newProps),
			layers: this.updateLayers(newProps)
		});

		this.checkDragPan(newProps);
		this.checkScrollZoom(newProps);

		this.state.mapboxglMap.resize();

		if (newProps.onClick && !this.props.onClick) {
			this.state.mapboxglMap.on("rightclick", newProps.onClick);
		}
		else if (!newProps.onClick && this.props.onClick) {
			this.state.mapboxglMap.off("rightclick", this.props.onClick);
		}
	}
	updateSources(props) {
		let sourceSet = set(this.state.sources.keys()),

			sources = map(),

			mbglMap = this.state.mapboxglMap;

		props.sources.forEach(source => {
			if (sourceSet.has(source.getId())) {
				sourceSet.remove(source.getId());
			}
			else {
				source(mbglMap);
			}
			sources.set(source.getId(), source);
		}, this);

		sourceSet.each(k => {
			mbglMap.removeSource(k);
		}, this);

		return sources;
	}
	updateLayers(props) {
		let layerSet = set(this.state.layers.keys()),

			layers = map(),

			mbglMap = this.state.mapboxglMap;

		// this.state.layers.each((k, v) => {
		// 	layers.set(k, v);
		// });

		props.layers.forEach(layer => {
			if (layerSet.has(layer.getId())) {
				layerSet.remove(layer.getId());
			}
			else {
				layer(mbglMap);
			}
			layers.set(layer.getId(), layer);
		}, this);

		layerSet.each(k => {
			mbglMap.removeLayer(k);
		}, this);

		return layers;
	}
	checkDragPan(props) {
		if (!this.state.mapboxglMap) return;
		if (props.dragPan) {
			this.state.mapboxglMap.dragPan.enable();
		}
		else {
			this.state.mapboxglMap.dragPan.disable();
		}
	}
	checkScrollZoom(props) {
		if (!this.state.mapboxglMap) return;
		if (props.scrollZoom) {
			this.state.mapboxglMap.scrollZoom.enable();
		}
		else {
			this.state.mapboxglMap.scrollZoom.disable();
		}
	}

	render() { return <div id="map" style={ { height: "100%" } }/> }
}

MapboxglMap.defaultProps = {
  	style: 'mapbox://styles/mapbox/bright-v9',

  	center: [-111.093735, 34.048927],
  	zoom: 7,

  	dragPan: true,
  	scrollZoom: true,

	accessToken: 'pk.eyJ1IjoiYW0zMDgxIiwiYSI6IkxzS0FpU0UifQ.rYv6mHCcNd7KKMs7yhY3rw',

	layers: [],
	sources: [],

	cursor: null
}

export default MapboxglMap;