import { RECEIVE_ROUTE_GEOMETRY } from "../actions/mapClick"

export default (state = null, action) => {
	if (action.type === RECEIVE_ROUTE_GEOMETRY) {
		return action.geom;
	}
	return state;
}