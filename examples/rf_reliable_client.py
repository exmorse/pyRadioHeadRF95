#!/usr/bin/python

import sys, os

# Add path to pyRadioHeadRF95 module
sys.path.append(os.path.dirname(__file__) + "/../")

import pyRadioHeadRF95 as radio
import time

CLIENT_ADDRESS = 1
SERVER_ADDRESS = 2

rf95 = radio.RF95()
rf95.init()
rf95.managerInit(CLIENT_ADDRESS)

rf95.setFrequency(868)
rf95.setTxPower(14, False)

print "StartUp Done!"

while True:
    msg = "Hello\0"
    print "Sending..."
    ret = rf95.sendtoWait(msg, len(msg), SERVER_ADDRESS)
    print "Sent " + str(ret)

    time.sleep(0.4)
    
    if rf95.available():
        (msg, l, source) = rf95.recvfromAck()
        print "Received: " + msg + " (" + str(l) + ") from: " + str(source)

    time.sleep(10)

