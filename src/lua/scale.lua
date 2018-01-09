local Scale = function()
	local domain = { 0, 1 }
	local range = { 0, 10 }

	local scale = {}

	scale.value = function(v)
		local ds = 0 - domain[1]
		local dv = domain[2] + ds

		local rs = 0 - range[1]
		local rv = range[2] + rs

		local ratio = rv / dv

		return (v + ds) * ratio - rs
	end

	scale.domain = function(...)
		if select("#", ...) == 0 then
			return domain
		end
		domain = select(1, ...)
		return scale
	end

	scale.range = function(...)
		if select("#", ...) == 0 then
			return range
		end
		range = select(1, ...)
		return scale
	end

	return scale
end

return Scale