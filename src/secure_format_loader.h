#pragma once

#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

// Cargador automatico: Godot lo llama solo cuando alguien hace load("...gde").
class SecureFormatLoader : public ResourceFormatLoader {
	GDCLASS(SecureFormatLoader, ResourceFormatLoader)

protected:
	static void _bind_methods() {}

public:
	// Extensiones que este cargador maneja.
	PackedStringArray _get_recognized_extensions() const override;
	// Que tipo de recurso produce (para que Godot sepa si somos los indicados).
	bool _handles_type(const StringName &type) const override;
	String _get_resource_type(const String &path) const override;
	// El trabajo real: leer, descifrar y devolver el Script.
	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};
