#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

// Camino 1: cifrar scripts y descifrarlos con nuestro propio codigo.
class SecureLoader : public RefCounted {
	GDCLASS(SecureLoader, RefCounted)

protected:
	static void _bind_methods();

public:
	// Lee un script en texto (in_path), lo cifra y lo guarda en out_path (.gde).
	void encrypt_file(const String &in_path, const String &out_path, const String &password);

	// Lee un .gde cifrado, lo descifra y devuelve un Script listo para usar con .new().
	Ref<Script> load_script(const String &path, const String &password);
};
