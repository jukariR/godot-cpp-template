@tool
extends EditorPlugin

# Este EditorPlugin solo engancha y desengancha el export plugin.
var _export_plugin: EditorExportPlugin


func _enter_tree() -> void:
	_export_plugin = preload("res://addons/secure/secure_export.gd").new()
	add_export_plugin(_export_plugin)


func _exit_tree() -> void:
	if _export_plugin:
		remove_export_plugin(_export_plugin)
		_export_plugin = null
