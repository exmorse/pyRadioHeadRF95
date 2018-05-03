#!/usr/bin/python

import sys, os

# Add path to pyRadioHeadRF95 module
sys.path.append(os.path.dirname(__file__) + "/../")

import pyRadioHeadRF95 as radio
import time

CLIENT_ADDRESS = 1
SERVER_ADDRESS = 2

rf95 = radio.RF95()

rf95.managerInit(SERVER_ADDRESS)

rf95.setFrequency(868)
rf95.setTxPower(14, False)

print "StartUp Done!"
print "Receiving..."

while True:
	if rf95.available():
		print "Available"
		(msg, l, source) = rf95.recvfromAck()
		print "Received: " + msg + " (" + str(l) + ") from: " + str(source) 

                time.sleep(0.1)
    
		msg = "Hello back\0"
		print "Sending..."
		ret = rf95.sendtoWait(msg, len(msg), source)
		print "Sent " + str(ret)

		time.sleep(0.05)

