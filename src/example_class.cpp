#include "key_locator.h"
#include "example_class.h"

void ExampleClass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &ExampleClass::print_type);
	godot::ClassDB::bind_method(D_METHOD("debug_info"), &ExampleClass::debug_info);
}

void ExampleClass::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}

String ExampleClass::debug_info() const {
	String out = vformat("exe base: 0x%x\n", (int64_t)get_exe_base());
	for (const char *n : { ".text", ".rdata", ".bss" }) {
		Section s;
		if (find_section(n, s)) {
			out += vformat("%s: 0x%x (%d bytes)\n", String(n), (int64_t)s.start, (int64_t)s.size);
		} else {
			out += vformat("%s: no encontrada\n", String(n));
		}
	}

	Section rdata;
	find_section(".rdata", rdata);
	uintptr_t str = find_text(rdata, "Can't open encrypted pack directory.");
	out += vformat("texto: 0x%x\n", (int64_t)str);

	return out;
}
