# Script to develop/experiment heavenli/arduino communication

import glob
import serial
import sys
#from time import *
import time
from cobs import cobs

def sendColor(n, q, r, g, b):
    global stateMach
    tmn = int(n)
    tmq = int(q)
    tmr = int(r * 127.0)
    tmg = int(g * 127.0)
    tmb = int(b * 127.0)

    tmm = bytearray([
        tmn, 
        tmq, 
        tmr, 
        tmg, 
        tmb])
    print(tmn, tmq, tmr, tmg, tmb)
    print("sending...")
    print(tmm)
    stateMach['device'].write(tmm)
    
def establishConnection():
    global stateMach
    curTime = time.time()
    print("[HOST] Searching for Heavenli devices over serial...")

    ports = getSerialPorts()
    i = 0
    while (len(ports) == 0):
        b = "Waiting for Serial Devices" + "." * i + " "*(3-i)
        print(b, end="\r")
        if (i > 2):
            i = 0
        else:
            i += 1
        time.sleep(0.5)
        ports = getSerialPorts()

    print("Found Serial devices on ports: " + str(ports))

    #print("[HOST] Available ports:" + str(ports))
    stateMach['device'] = serial.Serial(ports[0], 115200)
    stateMach['device'].close()
    stateMach['device'].open()
    time.sleep(2)

    print("[HOST] Sending syn packets")

    connectionEstablished = False
    sendOrReceive = True
    while not connectionEstablished:
        if time.time() - stateMach['t0'] > 0.251:
            if (sendOrReceive):
                #print("[HOST] Hello device, are you there?")
                #enmass = cobs.encode(b'quack')+b'\x00'
                #stateMach['device'].write(enmass)
                sendOrReceive = False

            else:
                bytesToRead = stateMach['device'].inWaiting()
                if (bytesToRead > 0):
                    try:
                        print("[HOST] Incoming Bytes: " + str(int(bytesToRead)))
                        zeroByte = b'\x00'
                        mess = stateMach['device'].read_until( zeroByte )[0:-1]
                        mess = str(cobs.decode( mess ))[2:-1]
                        print(mess)
                        #stateMach['device'].flushInput()
                    except:
                        print("Error Decoding Packet")

                if (bytesToRead > 128):
                    #print("flushing buffer")
                    stateMach['device'].flushInput()
                sendOrReceive = True

            #sendOrReceive = not sendOrReceive
            stateMach['t0'] = time.time()
        time.sleep(0.25)

    #while time.time() - stateMach['t0'] > 2.0:
        
# Largely based on solution by Thomas on stackoverflow
# https://stackoverflow.com/questions/12090503/listing-available-com-ports-with-python
# Send Thomas a thanks:
# https://stackoverflow.com/users/300783/thomas
def getSerialPorts():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i+1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # excludes current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('dev/tty.*')
    else:
        raise EnvironmetError('Unupported platform, darn shame')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass

    return result

if __name__ == '__main__':
    global stateMach
    stateMach = {}
    stateMach['t0'] = time.time()
    establishConnection()
    while True:
        bytesToRead = device.inWaiting()
        if bytesToRead > 0:
            print("Incoming Bytes: " + str(int(bytesToRead)))
            mess = str(stateMach['device'].read(bytesToRead))[2:-1]
            print(mess)
            if mess == "quack":
                sendColor(3, 1, 0.7, 0.4, 0.1)


