#pragma once
#include <cstdint>
#include <cstddef>

struct Section {
	uintptr_t start; // Dirección real en memoria.
	size_t size; // Tamaño en bytes.
};

// Busca una sección del .exe por nombre (".text", ".rdata", ".bss").
bool find_section(const char *name, Section &out);

// Devuelve la dirección de los 32 bytes de la llave de Godot, o nullptr si no la encuentra.
uint8_t *locate_script_encryption_key();

uintptr_t get_exe_base();

// Busca un texto (con su '\0' final) dentro de una sección. Devuelve su dirección, o 0.
uintptr_t find_text(const Section &s, const char *text);
