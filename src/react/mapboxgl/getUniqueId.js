let UNIQUE_ID = 0;
export default () => {
	return `mapboxgl-object-${ ++UNIQUE_ID }`;
}