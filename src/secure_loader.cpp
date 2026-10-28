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
	ClassDB::bind_method(D_METHOD("encrypt_bytes", "data"), &SecureLoader::encrypt_bytes);
}

PackedByteArray SecureLoader::encrypt_bytes(const PackedByteArray &data) {
	// Misma clave fija que usa el loader automatico (se endurece despues).
	PackedByteArray key = secure_key_from_password("mi-clave");
	PackedByteArray enc = secure_xor(data, key);

	// Anteponer el sello SEC1 para que el loader reconozca el archivo como nuestro.
	PackedByteArray out;
	out.push_back('S');
	out.push_back('E');
	out.push_back('C');
	out.push_back('1');
	out.append_array(enc);
	return out;
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
	out->store_string("SEC1"); // sello: 4 bytes al inicio
	out->store_buffer(enc); // luego lo cifrado
	out->close();
}

Ref<Script> SecureLoader::load_script(const String &path, const String &password) {
	// 1. Leer los bytes cifrados.
	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	if (f.is_null()) {
		UtilityFunctions::push_error("load_script: no se pudo abrir ", path);
		return Ref<Script>();
	}
	PackedByteArray raw = f->get_buffer(f->get_length());
	f->close();

	// revisar el sello SEC1 y quitarlo
	if (raw.size() < 4 || raw[0] != 'S' || raw[1] != 'E' || raw[2] != 'C' || raw[3] != '1') {
		UtilityFunctions::push_error("load_script: sin sello SEC1: ", path);
		return Ref<Script>();
	}
	PackedByteArray enc = raw.slice(4);

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
