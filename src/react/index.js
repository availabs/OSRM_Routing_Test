import React from "react"
import { connect } from 'react-redux'

import Mapboxgl from "./mapboxgl/Mapboxgl.react"
import GeojsonSource from "./mapboxgl/GeojsonSource"
import GeojsonLayer from "./mapboxgl/GeojsonLineLayer"
import geojson from "../geojson.json"
// let geojson = [];

import { mapClick } from "./actions/mapClick"

const mapStateToProps = state => ({
	mapPoints: state.mapPoints,
	routeGeometry: state.routeGeometry
})
const mapDispatchToProps = dispatch => ({
    onMapClick: coords => dispatch(mapClick(coords))
})

const extractLngLat = onMapClick =>
	e => (console.log(e.lngLat),onMapClick([e.lngLat.lng, e.lngLat.lat]))

const divStyles = {
	position: "absolute",
	left: "20px",
	top: "20px"
}
const buttonStyles = {
	borderRadius: "4px",
	padding: "5px 20px"
}
const ClearButton = ({ onMapClick }) =>
	<div style={ divStyles }>
		<button onClick={ () => onMapClick(null) }
			style={ buttonStyles }>
			CLEAR
		</button>
	</div>

//-113.205717,35.237149/-113.065857,35.291823
class App extends React.Component {
	constructor(props) {
		super(props);

		let source = GeojsonSource(),
			layer = GeojsonLayer()
				.source(source),

			geoSource = GeojsonSource(),
			geoLayer = GeojsonLayer()
				.source(geoSource)
				.lineWidth(1)
				.lineOpacity(0.5)
				.lineColor("#000");
		geoSource.features(geojson.features);

		this.state = {
			source,
			layer,
			geoSource,
			geoLayer
		}
	}

	componentWillReceiveProps(newProps) {
		let features = newProps.routeGeometry ?
			[{
				geometry: newProps.routeGeometry,
				type: "Feature",
				properties: {}
			}]
			: [];
		this.state.source.features(features);
	}

	render() {
		const height = `${ window.innerHeight - 16 }px`;
		return (
			<div style={ { height } }>
				<Mapboxgl onClick={ extractLngLat(this.props.onMapClick) }
					sources={ [this.state.source, this.state.geoSource] }
					layers={ [this.state.geoLayer, this.state.layer] }
					zoom={ 10 }
					center={ [-110.942376, 31.334095] }
					style={ 'mapbox://styles/mapbox/satellite-v9' }
					cursor="pointer"/>
				<ClearButton onMapClick={ this.props.onMapClick }/>
			</div>
		)
	}
}

// style={ 'mapbox://styles/mapbox/satellite-v9' }
// style={ 'mapbox://styles/mapbox/streets-v9' }

export default connect(mapStateToProps, mapDispatchToProps)(App)