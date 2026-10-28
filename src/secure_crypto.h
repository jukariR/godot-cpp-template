#pragma once
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace godot;

// Cifrado simple para el PoC (XOR con keystream de SHA-256). Se endurece despues.
// Cifrar y descifrar son la MISMA operacion: XOR dos veces devuelve el original.
PackedByteArray secure_xor(const PackedByteArray &data, const PackedByteArray &key);

// Deriva una llave de 32 bytes a partir de una contrasena de texto.
PackedByteArray secure_key_from_password(const String &password);
