#include "secure_format_loader.h"
#include "secure_crypto.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/gd_script.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// PoC: la clave esta fija aqui. Luego se arma con las dos mitades del diseno.
static const char *SECURE_PASSWORD = "mi-clave";

PackedStringArray SecureFormatLoader::_get_recognized_extensions() const {
	PackedStringArray exts;
	exts.push_back("gd");
	exts.push_back("gde");
	return exts;
}

bool SecureFormatLoader::_handles_type(const StringName &type) const {
	String t = String(type);
	return t == "Script" || t == "GDScript";
}

String SecureFormatLoader::_get_resource_type(const String &path) const {
	if (path.get_extension().to_lower() == "gde") {
		return "GDScript";
	}
	return "";
}

Variant SecureFormatLoader::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const {
	UtilityFunctions::print("mi _load: ", path);

	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	if (f.is_null()) {
		return Variant();
	}
	PackedByteArray raw = f->get_buffer(f->get_length());
	f->close();

	// sin sello SEC1 no es nuestro: que lo cargue Godot
	if (raw.size() < 4 || raw[0] != 'S' || raw[1] != 'E' || raw[2] != 'C' || raw[3] != '1') {
		return Variant();
	}
	PackedByteArray enc = raw.slice(4);

	PackedByteArray key = secure_key_from_password(String(SECURE_PASSWORD));
	PackedByteArray plain = secure_xor(enc, key);
	String source = plain.get_string_from_utf8();

	Ref<GDScript> script;
	script.instantiate();
	script->set_source_code(source);
	if (script->reload() != OK) {
		UtilityFunctions::push_error("SecureFormatLoader: no compilo: ", path);
		return Variant();
	}
	return script;
}
