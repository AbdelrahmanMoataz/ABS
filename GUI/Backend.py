from PyQt5.QtWidgets import *
from PyQt5.QtCore import QTimer
import serial
import time
import serial.tools.list_ports as list_ports
# Frontend File
from ABS import Ui_ABS

# ==> YOUR APPLICATION WINDOW
class SplashScreen(QMainWindow):
    def __init__(self):
        super().__init__()
        # Setup
        self.ui = Ui_ABS()
        self.ui.setupUi(self)

        self.arduinoSerial = serial.Serial('COM7', 9600, timeout=.1)

        #try:
            #self.arduinoSerial = serial.Serial('COM5', 9600, timeout=.1)
       # except:
            #print("NO CONNECTION :(")
            #self.buttonControl(False)
        # # Check connection
        # self.myports = [tuple(p) for p in list(list_ports.comports())]
        # self.arduino_port = [port for p
        # ort in self.myports if 'COM3' in port][0]
        # print(self.arduino_port)
        # # Start a daemon thread to check if connection is made in background
        # port_controller = threading.Thread(target=self.bluetoothConnection, args=(0.1,))
        # port_controller.setDaemon(True)
        # port_controller.start()

        self.brake_flag = True

        # Buttons
        self.ui.START_B.pressed.connect(self.start)
        self.ui.STOP_B.pressed.connect(self.stop)
        self.ui.ABS_B.pressed.connect(self.abs)
        self.ui.BRAKES_B.pressed.connect(self.brakes)
        self.ui.MANUAL_B.pressed.connect(self.manual)

        # Speedometers (timer callback reads the data received from arduino)
        self.timer = QTimer()
        self.timer.timeout.connect(self.showTime)  # Note: putting this connection inside the start method was causing
        # it to run more than once in the event loop. Should be called once
        # and controlled by starting and stopping timer

    def buttonControl(self, boolean):
        self.ui.START_B.setEnabled(boolean)
        self.ui.STOP_B.setEnabled(boolean)
        self.ui.ABS_B.setEnabled(boolean)
        self.ui.MANUAL_B.setEnabled(boolean)
        self.ui.BRAKES_B.setEnabled(boolean)

    # Might need to end thread if disconnected
    def bluetoothConnection(self, interval):
        while True:
            myports = [tuple(p) for p in list(serial.tools.list_ports.comports())]
            if self.arduino_port not in myports:
                styleSheet = """background-color: rgb(255,0,0)"""
                self.buttonControl(False)
                self.ui.BLUETOOTH_STATUS.setStyleSheet(styleSheet)
            else:
                styleSheet = """background-color: rgb(0,255,0)"""
                self.buttonControl(True)
                self.ui.BLUETOOTH_STATUS.setStyleSheet(styleSheet)
            time.sleep(interval)

    def start(self):
        self.arduinoSerial.write("S\n".encode())
        data = 5
        print(data)
        data = self.arduinoSerial.readline()
        print(data)

    def stop(self):
        self.arduinoSerial.write("s\n".encode())

    def abs(self):
        self.arduinoSerial.write("A\n".encode())
        # Change ABS icon to green and manual icon to red
        styleSheet = """background-color: rgb(255,0,0)"""
        self.ui.MANUAL_ICON.setStyleSheet(styleSheet)
        styleSheet = """background-color: rgb(0,255,0)"""
        self.ui.ABS_ICON.setStyleSheet(styleSheet)

    def manual(self):
        self.arduinoSerial.write("M\n".encode())
        # Change ABS icon to red and hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
        #
        #
        # manual icon to green
        styleSheet = """background-color: rgb(255,0,0)"""
        self.ui.ABS_ICON.setStyleSheet(styleSheet)
        styleSheet = """background-color: rgb(0,255,0)"""
        self.ui.MANUAL_ICON.setStyleSheet(styleSheet)

    def brakes(self):
        #self.buttonControl(False)
        #self.ui.STOP_B.setEnabled(True)
        #self.ui.BRAKES_B.setEnabled(True)
        if self.brake_flag:
            self.arduinoSerial.write("B\n".encode())
            self.brake_flag = False
        else:
            self.arduinoSerial.write("\0\n".encode())
            self.brake_flag = True

    def showTime(self):
         print(self.arduinoSerial.readline().decode())
         time.sleep(0.05)

if __name__ == '__main__':
    app = QApplication([])
    window = SplashScreen()
    window.show()
    app.exec_()  # Starts event loop


