extends Node
#Linea para hacer la build en x64 native tool...
#python -m SCons platform=windows target=template_debug
func _ready() -> void:
	# --- Prueba 1: carga manual (ya funcionaba) ---
	var src := "extends RefCounted\nfunc hola():\n\treturn 42\n"
	var f := FileAccess.open("user://plain.gd", FileAccess.WRITE)
	f.store_string(src)
	f.close()

	var loader := SecureLoader.new()
	loader.encrypt_file("user://plain.gd", "user://secret.gde", "mi-clave")

	var S := loader.load_script("user://secret.gde", "mi-clave")
	if S == null:
		print("FALLO manual")
		return
	var obj = S.new()
	print("manual: ", obj.hola())  # espera 42

	# --- Prueba 2: carga automatica con load() de Godot ---
	# Ciframos a user://, donde el editor NO escanea (evita el ruido de indexado).
	loader.encrypt_file("user://plain.gd", "user://secret.gde", "mi-clave")

	# load() NO llama a nuestra funcion: Godot elige el cargador por la extension .gde.
	var S2 = load("user://secret.gde")
	if S2 == null:
		print("FALLO automatico: load() devolvio null")
		return
	var obj2 = S2.new()
	print("automatico: ", obj2.hola())  # espera 42
