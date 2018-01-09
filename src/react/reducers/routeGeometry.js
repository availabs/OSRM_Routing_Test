import { MAP_CLICK, RECEIVE_ROUTE_GEOMETRY } from "../actions/mapClick"

export default (state = null, action) => {
	if (action.type === RECEIVE_ROUTE_GEOMETRY) {
		return action.geom;
	}
	else if (action.type === MAP_CLICK) {
		if (!action.coords) {
			return null;
		}
	}
	return state;
}