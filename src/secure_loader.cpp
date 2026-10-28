#include "secure_loader.h"
#include "secure_crypto.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/gd_script.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void SecureLoader::_bind_methods() {
	ClassDB::bind_method(D_METHOD("encrypt_file", "in_path", "out_path", "password"), &SecureLoader::encrypt_file);
	ClassDB::bind_method(D_METHOD("load_script", "path", "password"), &SecureLoader::load_script);
}

void SecureLoader::encrypt_file(const String &in_path, const String &out_path, const String &password) {
	// 1. Leer el texto del script tal cual.
	Ref<FileAccess> in = FileAccess::open(in_path, FileAccess::READ);
	if (in.is_null()) {
		UtilityFunctions::push_error("encrypt_file: no se pudo abrir ", in_path);
		return;
	}
	PackedByteArray plain = in->get_buffer(in->get_length());
	in->close();

	// 2. Cifrar.
	PackedByteArray key = secure_key_from_password(password);
	PackedByteArray enc = secure_xor(plain, key);

	// 3. Escribir el archivo cifrado.
	Ref<FileAccess> out = FileAccess::open(out_path, FileAccess::WRITE);
	if (out.is_null()) {
		UtilityFunctions::push_error("encrypt_file: no se pudo escribir ", out_path);
		return;
	}
	out->store_buffer(enc);
	out->close();
}

Ref<Script> SecureLoader::load_script(const String &path, const String &password) {
	// 1. Leer los bytes cifrados.
	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	if (f.is_null()) {
		UtilityFunctions::push_error("load_script: no se pudo abrir ", path);
		return Ref<Script>();
	}
	PackedByteArray enc = f->get_buffer(f->get_length());
	f->close();

	// 2. Descifrar (mismo XOR) y pasar a texto.
	PackedByteArray key = secure_key_from_password(password);
	PackedByteArray plain = secure_xor(enc, key);
	String source = plain.get_string_from_utf8();

	// 3. Construir un GDScript a partir del texto y compilarlo.
	Ref<GDScript> script;
	script.instantiate();
	script->set_source_code(source);
	Error err = script->reload();
	if (err != OK) {
		UtilityFunctions::push_error("load_script: el script no compilo, error ", err);
		return Ref<Script>();
	}
	return script;
}
