import { MAP_CLICK } from "../actions/mapClick"

export default (state = [], action) => {
	if (action.type === MAP_CLICK) {
		return state.concat([action.coords]);
	}
	return state;
}