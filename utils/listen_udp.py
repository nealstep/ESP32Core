#!/usr/bin/env python3

from argparse import ArgumentParser
from datetime import datetime
from socket import socket

from Crypto.Cipher import AES  # type: ignore
from secret import udp_data_port, hex_key

from binascii import unhexlify

from socket import AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_REUSEADDR, SO_BROADCAST

# listener on all interfaces
listen_address = "0.0.0.0"

cleaned = hex_key.removeprefix('"').removesuffix('"')
aes_key = unhexlify(cleaned)

parser = ArgumentParser(description="UDP Log Listener")
parser.add_argument(
    "-p",
    "--port",
    default=udp_data_port,
    type=int,
    help=f"UDP port to listen on ({udp_data_port})",
)
parser.add_argument(
    "-i",
    "--ip",
    default=listen_address,
    help=f"IP address to bind to ({listen_address})",
)
parser.add_argument(
    "-e", "--encrypted", action="store_true", help="Use encryption (false)"
)
args = parser.parse_args()
ip = args.ip
port = args.port
enc = args.encrypted

sock = socket(AF_INET, SOCK_DGRAM)
sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
sock.bind((ip, port))
print(f"UDP listener started on {ip}:{port}")

while True:
    data, addr = sock.recvfrom(1024)
    asof = datetime.now()
    asof_str = asof.strftime("%Y-%m-%d@%H:%M:%S")
    if enc:
        if len(data) < 28:
            print(f"Malformed packet received from {addr} (Too short).")
            continue
        try:
            iv = data[:12]
            tag = data[12:28]
            ciphertext = data[28:]
            cipher = AES.new(aes_key, AES.MODE_GCM, nonce=iv)
            decrypted_bytes = cipher.decrypt_and_verify(ciphertext, tag)
            plaintext_str = decrypted_bytes.decode("utf-8")
            print(f"{asof_str}|{addr[0]}:{plaintext_str}")
        except ValueError as e:
            print(f"Decryption failed or corrupt data from {addr} {e}")
        except Exception as e:
            print(f"Unexpected processing error: {e}")
    else:
        try:
            line = data.decode()
        except UnicodeDecodeError:
            line = data.hex()
        print(f"{asof_str}|{addr[0]}:{line}")
