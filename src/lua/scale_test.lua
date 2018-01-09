local Scale = require("scale")

local scale = Scale()

print(scale.value(0.5))
print(scale.value(5))
print(scale.value(-0.5))

scale.domain({ 5, 10 })

print(scale.value(5))
print(scale.value(10))
print(scale.value(7.5))

scale.domain({ 10, 0 })

print(scale.value(10))
print(scale.value(5))
print(scale.value(0))

scale.domain({ 0, 10 })
scale.range({ 5, 10 })

print(scale.value(0))
print(scale.value(5))
print(scale.value(10))

scale.domain({ 5, 10 })
scale.range({ 15, 30 })

print(scale.value(5))
print(scale.value(7.5))
print(scale.value(10))