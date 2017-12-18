export const MAP_CLICK = "MAP_CLICK"
export const RECEIVE_ROUTE_GEOMETRY = "RECEIVE_ROUTE_GEOMETRY"

import HOST from "./getHost"

export const mapClick = coords =>
	(dispatch, getState) => {
		const state = getState();
		if (state.mapPoints.length) {
			dispatch(getRouteGeometry(state.mapPoints.concat([coords])));
		}
		dispatch(_mapClick(coords));
	}

const _mapClick = coords => ({
	type: MAP_CLICK,
	coords
})

const makeUrl = points => `${ HOST }/route/${ points.join(";") }`

const getRouteGeometry = points =>
	dispatch => fetch(makeUrl(points))
		.then(res => {
			if (res.ok) {
				return res.json();
			}
			throw res;
		})
		.then(json => dispatch(receiveRouteGeometry(json)))
		.catch(err => console.log("<FETCH ERROR>",err))

export const receiveRouteGeometry = json => ({
	type: RECEIVE_ROUTE_GEOMETRY,
	geom: json.routes[0].geometry
})