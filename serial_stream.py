import serial
import time
import image_to_binarray

#--- Miniterm on /dev/ttyACM0  9600,8,N,1 ---

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=1)
# image_to_binarray.WriteOutImage()

x = 'UXXDXUXXXXXDXUXXXY'

def write_to_arduino():
    arduino.write(bytes(x, 'utf-8'))

    while(not arduino.readable()):
        pass
    while (arduino.readable):
        print (f'recvd message {arduino.read_all()}')
'''
Custom GCODE idea:
X16 U X32 D X16 will draw a line

'''