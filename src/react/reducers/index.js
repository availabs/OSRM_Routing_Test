import { combineReducers } from 'redux'

import mapPoints from "./mapPoints"
import routeGeometry from "./routeGeometry"

export default combineReducers({
	mapPoints,
	routeGeometry
})