import serial
import time

# Configure the serial port.
# On Windows it will be something like 'COM3'.
# On macOS or Linux it will be something like '/dev/tty.usbserial-XXXX' or '/dev/ttyUSB0'.
# Make sure to set the correct port and baud rate.
SERIAL_PORT = '/dev/ttyACM1' # <-- IMPORTANT: Change this to your server's serial port
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud...")
except serial.SerialException as e:
    print(f"Error: Could not open serial port {SERIAL_PORT}. {e}")
    exit()

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
        time.sleep(0.1)
except KeyboardInterrupt:
    print("\nExiting program.")
finally:
    ser.close()
    print("Serial port closed.")
