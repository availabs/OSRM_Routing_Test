export default () => {
  let mapboxglMap = null,

    sourceId = 'composite',
    sourceLayerId = 'ny_simple';

  function source (map) {
    if (map) {
      mapboxglMap = map
      return;
    }
  }

  source.id = d => {
    if (!arguments.length) {
      return sourceId;
    }
    sourceId = d;
    return source;
  }
  source.getId = () => sourceId;
  source.layerId = d => {
    if (!arguments.length) {
      return sourceLayerId;
    }
    sourceLayerId = d;
    return source;
  }
  source.getLayerId = () => sourceLayerId;

  return source
}
