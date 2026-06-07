# Debugging Log — BMP280 Sensor Hardware Failure
**Date:** June 7, 2026  
**Author:** Sohail Ali Hassan Abbasi  
**Hardware:** Raspberry Pi 3 Model B+ | BMP280 Sensor Module

---

## Problem Description

BMP280 temperature sensor stopped responding on I2C bus after 
working correctly for several hours. Sensor was reading 36.51°C 
successfully, then after reconnection it disappeared from the 
I2C bus completely.

---

## Timeline

**Morning — Working:**
- Wired BMP280 to Pi (VCC, GND, SDA, SCL, CSB, SDO)
- sudo i2cdetect -y 1 → showed 76 at address 0x76
- i2cget -y 1 0x76 0xD0 → returned 0x58 (correct chip ID)
- Driver loaded, sysfs working
- cat /sys/kernel/bmp280/temperature → 36.51°C

**Evening — Failed:**
- Reconnected sensor after handling
- sudo i2cdetect -y 1 → empty grid, no 76
- i2cget -y 1 0x76 0xD0 → Error: Read failed

---

## Diagnosis Steps

**Step 1 — Check for conflicting drivers:**
```bash
lsmod | grep i2c
sudo rmmod i2c_dummy
sudo i2cdetect -y 1
```
Result: Grid still empty after removing i2c_dummy.
Conclusion: Not a driver conflict issue.

**Step 2 — Check I2C bus health:**
```bash
ls /dev/i2c*
lsmod | grep i2c_bcm2835
```
Result: /dev/i2c-1 exists, i2c_bcm2835 loaded.
Conclusion: I2C bus is working. Problem is the sensor.

**Step 3 — Check software configuration:**
```bash
cat /boot/firmware/config.txt | grep i2c
```
Result: dtparam=i2c_arm=on — correctly enabled.
Conclusion: Software configuration is correct.

**Step 4 — Direct register read:**
```bash
i2cget -y 1 0x76 0xD0
```
Result: Error: Read failed
Conclusion: Sensor not responding at hardware level.

**Step 5 — Physical inspection:**
- Rewired with new jumper wires
- Moved sensor to different breadboard holes
- Verified correct pin connections: SDA→Pin3, SCL→Pin5
Result: Still not detected.
Conclusion: Hardware failure confirmed.

---

## Root Cause

ESD (Electrostatic Discharge) damage to BMP280 chip.
Repeated handling of sensor pins without grounding caused 
static discharge that destroyed the internal chip circuitry.
The chip is physically dead — no software fix possible.

---

## Evidence

Working chip ID read:    0x58  (correct)
Failed chip ID read:     Error: Read failed
Failed chip ID read:     0xffffff87 (garbage — corrupted I2C)

---

## Fix

Replace BMP280 sensor module. Order two units — one active, 
one spare.

---

## Lessons Learned

1. Always ground yourself before handling sensor pins
2. Touch metal before touching any electronic component
3. Power down Pi before rewiring — never hot-swap
4. Buy 2 of every sensor — always keep a spare
5. Document chip ID immediately after first successful connection
6. ESD damage has no visible symptoms — chip looks fine but is dead

---

## Prevention for Future Projects

- Use anti-static wrist strap for sensitive components
- Never touch chip pins directly — hold PCB by edges
- Power cycle sensor gently — no hot plugging
- Verify chip ID register immediately after every reconnection
