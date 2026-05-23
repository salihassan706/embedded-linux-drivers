#!/usr/bin/env python3
import sys

def log_pass(msg):
    print(f"[PASS] {msg}")

def log_fail(msg):
    print(f"[FAIL] {msg}")
    sys.exit(1)

def main():
    if len(sys.argv) < 2:
        log_fail("No log file provided")

    with open(sys.argv[1], 'r', errors='replace') as f:
        output = f.read()

    print("=" * 50)
    print("ARM64 Embedded Linux Driver Test Suite")
    print("Sohail Ali Hassan Abbasi")
    print("=" * 50)
    print("")

    # Test 1
    if "ARM64 Embedded Linux Test Environment" in output:
        log_pass("System booted successfully")
    else:
        log_fail("System did not boot")

    # Test 2
    if "ARM64_TEST: Module loaded in QEMU" in output:
        log_pass("hello_arm64 module loaded successfully")
    else:
        log_fail("Module did not load")

    # Test 3
    if "ARM64_TEST: Sohail's embedded Linux framework running" in output:
        log_pass("Module init message verified")
    else:
        log_fail("Module init message not found")

    # Test 4
    if "Loaded: /modules/hello_arm64.ko" in output:
        log_pass("Module load confirmed by init script")
    else:
        log_fail("Init script did not confirm module load")

    print("")
    print("=" * 50)
    print("All tests passed.")
    print("ARM64 embedded Linux framework verified.")
    print("=" * 50)
    sys.exit(0)

if __name__ == "__main__":
    main()
