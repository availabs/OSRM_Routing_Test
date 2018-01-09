import { MAP_CLICK } from "../actions/mapClick"

export default (state = [], action) => {
	if (action.type === MAP_CLICK) {
		if (action.coords) {
			return state.concat([action.coords]);
		} else {
			return [];
		}
	}
	return state;
}