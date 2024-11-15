# pip install pyserial
# https://github.com/pyserial/pyserial
import serial

ser = serial.Serial('/dev/ttyACM0', 9600)
ser.flushInput()
ser.flushOutput()

while True:
    ser_bytes = ser.readline()
    print(ser_bytes.decode('latin1').rstrip())
