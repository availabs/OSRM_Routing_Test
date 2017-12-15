import getUniqueId from './getUniqueId'

export default function () {
  let stopperId = getUniqueId(),
    paint = 'line-color',
    property = 'Tmc',
    type = 'categorical',
    _default = '#000',
    scale = d3.scale.ordinal()

  function stopper (layer) {
    let keys = scale.domain()
    const prop = {
      stops: keys.map(k => [k, scale(k)]),
      default: _default,
      property,
      type
    }
    if (prop.stops.length) {
// console.log("<Stopper> UPDATING STOPS:",prop.stops.length,paint)
      layer[paint](prop)
    }
    return stopper
  }
  stopper.getId = () => stopperId
  stopper.paint = function (d) {
    if (!arguments.length) {
      return paint
    }
    paint = d
    return stopper
  }
  stopper.property = function (d) {
    if (!arguments.length) {
      return property
    }
    property = d
    return stopper
  }
  stopper.type = function (d) {
    if (!arguments.length) {
      return type
    }
    type = d
    return stopper
  }
  stopper.scale = function (d) {
    if (!arguments.length) {
      return scale
    }
    scale = d
    return stopper
  }
  return stopper
}
