import re, sys, serial

PORT = "/dev/ttyACM0"  # adjust as needed
BAUD = 115200
pat = re.compile(r"\[RX\]\s+From:\s+([0-9a-f:]+)\s+RSSI:\s+(-?\d+)\s+dBm\s+Counter:\s+(\d+)")

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"[i] Listening on {PORT} … Ctrl+C to quit\n")
except serial.SerialException as e:
    print(f"[!] {e}")
    sys.exit(1)

try:
    while True:
        line = ser.readline().decode("utf-8", "ignore").strip()
        if not line:
            continue
        m = pat.search(line)
        if m:
            addr, rssi, count = m.groups()
            print(f"{addr:>17} | RSSI {int(rssi):>4} dBm | Counter {int(count)}")
        else:
            print(line)
except KeyboardInterrupt:
    print("\n[i] Bye.")
finally:
    ser.close()
