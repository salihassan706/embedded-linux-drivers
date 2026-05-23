#!/usr/bin/env python3

import subprocess
import socket
import time
import sys
import os

SOCKET_PATH = "/tmp/qemu-serial.sock"
TIMEOUT = 60

def log_pass(msg):
    print(f"[PASS] {msg}")

def log_fail(msg):
    print(f"[FAIL] {msg}")
    sys.exit(1)

def wait_for_socket(path, timeout):
    start = time.time()
    while time.time() - start < timeout:
        if os.path.exists(path):
            return True
        time.sleep(0.5)
    return False

def read_until(sock, expected, timeout=30):
    data = b""
    start = time.time()
    while time.time() - start < timeout:
        try:
            chunk = sock.recv(1024)
            if chunk:
                data += chunk
                if expected.encode() in data:
                    return data.decode(errors='replace')
        except:
            pass
        time.sleep(0.1)
    return data.decode(errors='replace')

def send_command(sock, cmd):
    sock.sendall((cmd + "\n").encode())
    time.sleep(0.5)

def main():
    print("=" * 50)
    print("ARM64 Embedded Linux Driver Test Suite")
    print("Sohail Ali Hassan Abbasi")
    print("=" * 50)
    print("")

    # Start QEMU in background
    print("Starting QEMU ARM64 system...")
    qemu_proc = subprocess.Popen(
        ["./boot_test.sh"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )

    # Wait for socket
    if not wait_for_socket(SOCKET_PATH, 30):
        log_fail("QEMU socket did not appear")

    time.sleep(2)

    # Connect to serial
    try:
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(SOCKET_PATH)
        sock.setblocking(False)
    except Exception as e:
        log_fail(f"Cannot connect to QEMU serial: {e}")

    # Wait for boot
    print("Waiting for system to boot...")
    output = read_until(sock, "~ #", timeout=60)

    if "~ #" not in output:
        log_fail("System did not boot to shell")
    log_pass("System booted successfully")

    # Test 1 — kernel version
    send_command(sock, "uname -r")
    output = read_until(sock, "~ #")
    if "6.1.0" in output:
        log_pass("Kernel version verified: 6.1.0")
    else:
        log_fail("Unexpected kernel version")

    # Test 2 — module loaded
    send_command(sock, "cat /proc/modules")
    output = read_until(sock, "~ #")
    if "hello_arm64" in output:
        log_pass("hello_arm64 module is loaded")
    else:
        log_fail("hello_arm64 module not found in /proc/modules")

    # Test 3 — dmesg output
    send_command(sock, "dmesg | grep ARM64_TEST")
    output = read_until(sock, "~ #")
    if "ARM64_TEST: Module loaded in QEMU" in output:
        log_pass("Module init message verified in dmesg")
    else:
        log_fail("Expected dmesg message not found")

    # Test 4 — proc filesystem
    send_command(sock, "ls /proc | grep modules")
    output = read_until(sock, "~ #")
    if "modules" in output:
        log_pass("/proc/modules exists")
    else:
        log_fail("/proc/modules not found")

    # All tests passed
    print("")
    print("=" * 50)
    print("All tests passed.")
    print("ARM64 embedded Linux framework verified.")
    print("=" * 50)

    # Cleanup
    send_command(sock, "poweroff")
    time.sleep(2)
    qemu_proc.terminate()
    sock.close()

    sys.exit(0)

if __name__ == "__main__":
    main()
