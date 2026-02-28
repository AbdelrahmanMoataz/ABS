import serial
import time

arduino = serial.Serial(port='COM4', baudrate=9600, timeout=.1)
while True:
    arduino.write("S\n".encode())
    print("S\n")
    time.sleep(1)
    arduino.write("s\n".encode())
    print("s\n")
    time.sleep(1)

"""
def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline()
    return data.decode()


while True:
    num = input("Enter a number: ")
    print("hello1")
    value = write_read(num)
    print("hello2")
    print(value)

"""

