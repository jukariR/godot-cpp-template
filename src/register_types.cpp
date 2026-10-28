#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "example_class.h"
#include "secure_loader.h"
#include "secure_format_loader.h"

#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

// Guardamos el cargador para quitarlo al cerrar.
static Ref<SecureFormatLoader> secure_format_loader;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(ExampleClass);
	GDREGISTER_CLASS(SecureLoader);
	GDREGISTER_CLASS(SecureFormatLoader);

	// Darle nuestro cargador a Godot (al frente, para que gane sobre otros).
	secure_format_loader.instantiate();
	ResourceLoader::get_singleton()->add_resource_format_loader(secure_format_loader, true);
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	if (secure_format_loader.is_valid()) {
		ResourceLoader::get_singleton()->remove_resource_format_loader(secure_format_loader);
		secure_format_loader.unref();
	}
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}