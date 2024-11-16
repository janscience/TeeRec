# https://github.com/pyserial/pyserial
try:
    import serial
    from serial.tools.list_ports import comports
except ImportError:
    print('ERROR: failed to import serial module !')
    print('You need to install the pyserial package using')
    print('> pip install pyserial')
    exit()


def discover_teensy_ports():
    for port in sorted(comports(False)):
        print(port.device, port.name, port.description, port.hwid, port.vid, port.pid, port.serial_number, port.manufacturer, port.product)
        # teensy36 = re.match(".*VID:PID=16C0:0483.*", hwid)


def read_teensy():
    ser = serial.Serial('/dev/ttyACM0', 9600)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    while True:
        ser_bytes = ser.readline()
        print(ser_bytes.decode('latin1').rstrip())
    

if __name__ == '__main__':
    #read_teensy()
    discover_teensy_ports()
    
