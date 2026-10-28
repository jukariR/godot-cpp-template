#include "secure_crypto.h"

#include <godot_cpp/classes/hashing_context.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

// Un bloque de keystream = SHA-256(key + contador). Cada bloque cubre 32 bytes.
static PackedByteArray keystream_block(const PackedByteArray &key, int64_t counter) {
	Ref<HashingContext> ctx;
	ctx.instantiate();
	ctx->start(HashingContext::HASH_SHA256);
	ctx->update(key);

	PackedByteArray cb;
	cb.resize(8);
	for (int b = 0; b < 8; b++) {
		cb.set(b, (uint8_t)((counter >> (b * 8)) & 0xFF));
	}
	ctx->update(cb);
	return ctx->finish(); // 32 bytes
}

PackedByteArray secure_xor(const PackedByteArray &data, const PackedByteArray &key) {
	PackedByteArray out = data; // misma longitud
	int64_t n = data.size();
	int64_t counter = 0;
	for (int64_t off = 0; off < n; off += 32, counter++) {
		PackedByteArray ks = keystream_block(key, counter);
		for (int64_t i = 0; i < 32 && off + i < n; i++) {
			out.set(off + i, data[off + i] ^ ks[i]);
		}
	}
	return out;
}

PackedByteArray secure_key_from_password(const String &password) {
	Ref<HashingContext> ctx;
	ctx.instantiate();
	ctx->start(HashingContext::HASH_SHA256);
	ctx->update(password.to_utf8_buffer());
	return ctx->finish(); // 32 bytes
}
