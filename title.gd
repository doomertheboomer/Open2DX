extends Node2D

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_touch_screen_button_pressed():
	networking.create_enet(get_node("TextEdit").text, 42069)
	get_tree().change_scene_to_file("res://main.tscn")

