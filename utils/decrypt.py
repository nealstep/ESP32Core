#!/usr/bin/env python3

from Crypto.Cipher import AES  # type: ignore

from secret import hex_key

from binascii import unhexlify

msg = "f5286141a6c1d3ce851a1dc6cc11cef3f36cd942c5ffd23b4f59c190b4ef227" \
        + "d56aacd8067bb04f7bd1a394baf8af2378ab9eada996313aec586df1a75016" \
        + "9612060f851275676ecd6dbe7d72acb72e866c5c28f96a70207facc31ac33018bf8"

cleaned = hex_key.removeprefix('"').removesuffix('"')
aes_key = unhexlify(cleaned)
data = unhexlify(msg)

try:
    iv = data[:12]
    tag = data[12:28]
    ciphertext = data[28:]
    cipher = AES.new(aes_key, AES.MODE_GCM, nonce=iv)
    decrypted_bytes = cipher.decrypt_and_verify(ciphertext, tag)
    plaintext_str = decrypted_bytes.decode("utf-8")
    print(plaintext_str)
except ValueError as e:
    print(f"Decryption failed or corrupt data: {e}")
except Exception as e:
    print(f"Unexpected processing error: {e}")
