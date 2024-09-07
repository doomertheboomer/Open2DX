extends Sprite2D

var dragging = false
var turntableIndex = 99999 # set to absurdly high value
var angle = 0.0
var previous_angle = 0.0
var trueAngle = 0.0
var start_angle_offset = 0.0

func _input(event):
	if event is InputEventScreenTouch:
		if event.pressed and event.position.x < 310: # click down
			print("Starting turntable!")
			turntableIndex = event.index
			dragging = true
			
			start_angle_offset = _calculate_angle(event.position) - angle
		
		if not event.pressed and event.index == turntableIndex: # click up
			print("Ending turntable!")
			dragging = false
			turntableIndex = 99999 # reset to absurdly high value
	
	if event is InputEventScreenDrag and dragging: # hold
		angle = _calculate_angle(event.position) - start_angle_offset
		
		var delta_angle = angle - previous_angle
		if delta_angle > 180:
			delta_angle -= 360
		elif delta_angle < -180:
			delta_angle += 360
			
		trueAngle += delta_angle
		previous_angle = angle
		
		var wrapped_value = fposmod(trueAngle, 360.0)
		# Normalize the wrapped value to a 0 to 1 range
		var axis_value = wrapped_value / 360.0
		
		var byte_array = str(axis_value).to_utf8_buffer()
		# print("Current angle: %f" % axis_value) # send this over network
		networking.send_packet(byte_array)

func _calculate_angle(position):
	var quartile = 0
	if position.x > 310 and position.y < 527:
		quartile = 1 # 0-90 deg
	elif position.x > 310 and position.y > 527:
		quartile = 2 # 90-180 deg
	elif position.x < 310 and position.y > 527:
		quartile = 3 # 180-270 deg
	else:
		quartile = 4 # 270-360 deg
		
	var gradient = (527 - position.y) / (310 - position.x)
	var calculated_angle = 0.0
	match quartile:
		1:
			calculated_angle = rad_to_deg(atan(gradient)) + 90
		2:
			calculated_angle = rad_to_deg(atan(gradient)) + 90
		3:
			calculated_angle = rad_to_deg(atan(gradient)) + 270
		4:
			calculated_angle = rad_to_deg(atan(gradient)) + 270
	
	return calculated_angle

func _process(delta):
	pass
