extends Node


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_pressed(button):
	print("button " + str(button) + " pressed")
	networking.send_packet(button.to_utf8_buffer())
	pass # Replace with function body.


func _on_released(button):
	print("button " + str(button) + " released")
	networking.send_packet(button.to_utf8_buffer())
	pass # Replace with function body.
