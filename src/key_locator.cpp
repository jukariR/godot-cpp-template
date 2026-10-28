#define WIN32_LEAN_AND_MEAN // Carga solo lo básico de Windows.
#define NOMINMAX // Evita que Windows choque con nombres de Godot.
#include "key_locator.h"
#include <windows.h>
#include <cstring>
#include <algorithm>

uintptr_t get_exe_base() {
	// nullptr = "el .exe que está corriendo", o sea, el juego.
	return reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
}

bool find_section(const char *name, Section &out) {
	uintptr_t base = get_exe_base();
	// El primer encabezado dice dónde está el segundo (e_lfanew).
	auto *dos = reinterpret_cast<IMAGE_DOS_HEADER *>(base);
	auto *nt = reinterpret_cast<IMAGE_NT_HEADERS64 *>(base + dos->e_lfanew);
	// Justo después viene la lista de secciones.
	IMAGE_SECTION_HEADER *sec = IMAGE_FIRST_SECTION(nt);
	for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++) {
		if (strncmp(reinterpret_cast<char *>(sec->Name), name, 8) == 0) {
			out.start = base + sec->VirtualAddress; // inicio + RVA
			out.size = sec->Misc.VirtualSize;
			return true;
		}
	}
	return false;
}

uint8_t *locate_script_encryption_key() {
	return nullptr;
}

uintptr_t find_text(const Section &s, const char *text) {
	const char *begin = reinterpret_cast<const char *>(s.start);
	const char *end = begin + s.size;
	// +1 para incluir el '\0': así no confunde con un texto más largo que empiece igual.
	const char *hit = std::search(begin, end, text, text + strlen(text) + 1);
	return hit == end ? 0 : reinterpret_cast<uintptr_t>(hit);
}
