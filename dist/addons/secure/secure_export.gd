@tool
extends EditorExportPlugin

# Cifra cada .gd durante la exportacion, usando el dll (SecureLoader.encrypt_bytes).
var _loader := SecureLoader.new()


func _get_name() -> String:
	return "SecureExport"


func _export_file(path: String, type: String, features: PackedStringArray) -> void:
	# Solo cuando se exporta a Windows.
	if not features.has("windows"):
		return
	# Solo scripts .gd.
	if not path.ends_with(".gd"):
		return
	# No cifrar los addons (incluido este plugin).
	if path.begins_with("res://addons/"):
		return

	# Leer el texto original, cifrarlo (+ sello) y reemplazar el archivo exportado.
	var data := FileAccess.get_file_as_bytes(path)
	if data.is_empty():
		return
	var enc := _loader.encrypt_bytes(data)
	add_file(path, enc, false)
	skip()
