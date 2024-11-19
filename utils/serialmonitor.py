# https://github.com/pyserial/pyserial
try:
    import serial
    from serial.tools.list_ports import comports
except ImportError:
    print('ERROR: failed to import serial module !')
    print('You need to install the pyserial package using')
    print('> pip install pyserial')
    exit()
    
# https://github.com/pyusb/pyusb
# pip install pyusb
try:
    import usb.core
    has_usb = True
except ImportError:
    has_usb = False

import sys
import threading
from time import sleep


class KeyboardInput(threading.Thread):

    def __init__(self, ser):
        super().__init__(daemon=True)
        self.ser = ser
        self.start()

    def run(self):
        while True:
            x = input()
            self.ser.write(x.encode('latin1'))
            self.ser.write(b'\n')

    
teensy_model = {   
    0x274: '30',
    0x275: '31',
    0x273: 'LC',
    0x276: '35',
    0x277: '36',
    0x278: '40 beta',
    0x279: '40',
    0x280: '41',
    0x281: 'MM'}
"""Map bcdDevice of USB device to Teensy model version."""


def discover_teensy_ports():
    devices = []
    serial_numbers = []
    models = []
    for port in sorted(comports(False)):
        if port.vid is None and port.pid is None:
            continue
        #if port.vid == 0x16C0 and port.pid in [0x0483, 0x048B, 0x048C, 0x04D5]:
        if port.manufacturer == 'Teensyduino':
            teensy_model = get_teensy_model(port.vid, port.pid,
                                            port.serial_number)
            devices.append(port.device)
            serial_numbers.append(port.serial_number)
            models.append(teensy_model)
    return devices, models, serial_numbers


def get_teensy_model(vid, pid, serial_number):
    if has_usb:
        dev = usb.core.find(idVendor=vid, idProduct=pid,
                            serial_number=serial_number)
        return teensy_model[dev.bcdDevice]
    else:
        return ''

    
def print_teensys(devices, models, serial_numbers):
    for dev, model, num in zip(devices, models, serial_numbers):
        print(f'Teensy{model} with serial number {num} on {dev:<20s}')

        
def read_teensy(device):
    ser = serial.Serial(device)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    KeyboardInput(ser)
    while True:
        try:
            if ser.in_waiting > 0:
                x = ser.read(ser.in_waiting)
                if x == '\n':
                    sys.stdout.write(b'\n')
                else:
                    sys.stdout.write(x.decode('latin1'))
            sleep(0.01)
        except (OSError, serial.serialutil.SerialException):
            print()
            print()
            print('disconnected')
            break
    ser.close()
    

if __name__ == '__main__':
    while True:
        print()
        print('Waiting for Teensy device ...')
        while True:
            devices, models, serial_numbers = discover_teensy_ports()
            if len(devices) > 0:
                break
        print('found ', end='')
        print_teensys(devices, models, serial_numbers)
        read_teensy(devices[0])
        sleep(1)
