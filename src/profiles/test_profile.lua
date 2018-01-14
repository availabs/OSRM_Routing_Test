
api_version = 2

function setup()
	return {
		properties = {
			weight_name = 'duration'
		},
		default_speed = 5,
		default_mode = mode.walking
	}
end

function process_node(profile, node, result)
-- perhaps check node altitude and make impassable after specified altitude
end

function process_way(profile, way, result)
	result.forward_mode = profile.default_mode
	result.backward_mode = profile.default_mode

-- TODO: calculate speeds based on way slope:
-- a positive slope decreases forward_speed, a negative slope increases forward_speed
-- a positive slope increases backward_speed, a negative slope decreases backward_speed
	result.forward_speed = profile.default_speed
	result.backward_speed = profile.default_speed

	local height_delta = way:get_value_by_key('height_delta')
	local distance = way:get_value_by_key('distance')

	if height_delta and distance then
		height_delta = tonumber(height_delta)
		distance = tonumber(distance)
		result.forward_speed = 6 * math.exp(-3.5 * math.abs(height_delta / distance + 0.05));
		result.backward_speed = 6 * math.exp(-3.5 * math.abs(-height_delta / distance + 0.05));
	end
--[[
	if height_delta then
		height_delta = tonumber(height_delta)
		if height_delta > 0 then
			result.forward_speed = profile.default_speed - 4
			result.backward_speed = profile.default_speed + 4
		elseif height_delta < 0 then
			result.forward_speed = profile.default_speed + 4
			result.backward_speed = profile.default_speed - 4
		end
	end
]]
--[[
W = 6 * Math.exp(-3.5 * Math.abs(delta_h/d + 0.05));
]]
end

function process_turn(profile, turn)
	turn.duration = 0
end

return {
  setup = setup,
  process_way = process_way,
  process_node = process_node,
  process_turn = process_turn
}