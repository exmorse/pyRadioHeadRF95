#!/usr/bin/python

from cffi import FFI
import os

ffi = FFI()

buf = ffi.new("char*")
l = ffi.new("uint8_t*")
l[0] = 0
src = ffi.new("uint8_t*")
src[0] = 0

class RF95:

	def __init__(self):

		ffi.cdef("int init();\
                  void setTxPower(int8_t power, bool useRFO);\
                  bool setFrequency(float centre);\
		  int send(uint8_t* data, uint8_t len);\
		  int waitPacketSent();\
	  	  int waitAvailableTimeout(int ms);\
	  	  int available();\
	  	  int recv(char* buf, uint8_t* len);\
	  	  int maxMessageLength();\
	 	  int printRegisters();\
	 	  int enterSleepMode();\
	 	  int setModeIdle();\
	 	  int setModeTx();\
	 	  int setModeRx();\
	  	  \
	  	  int managerInit(int address);\
	  	  int sendtoWait(uint8_t* data, uint8_t len, uint8_t dst);\
	  	  int recvfromAck(char* buf, uint8_t* len, uint8_t* form);\
	 	  int recvfromAckTimeout(char* buf, uint8_t* len, uint16_t timeout, uint8_t* form);\
	  	  int setTimeout(uint16_t timeout);\
		  int retries();\
	 	  int setRetries(uint8_t retries);\
	 	  int retransmissions();\
	  	  int resetRetransmissions();")
	
		global radiohead 
		path_string = os.path.dirname(__file__) + "/libradiohead.so"
		radiohead = ffi.dlopen(path_string)

    		
	def init(self):
		r = radiohead.init()
		if r != 0:
                    raise RuntimeError("RF95 init failed - value: " + str(r))

        def setTxPower(self, power, useRFO):
                radiohead.setTxPower(power, useRFO)

        def setFrequency(self, centre):
                r = radiohead.setFrequency(centre)
                return r

	def managerInit(self, address):
		radiohead.managerInit(address)

	def send(self, data, l):
		r = radiohead.send(data, l)
		if r != 0:
			raise RuntimeError("nRF24 send failed")

	def waitPacketSent(self):
		radiohead.waitPacketSent()

	def waitAvailableTimeout(self):
		radiohead.waitAvailableTimeout()

	def available(self):
		b = radiohead.available()
		if (b == 1):
			return True
		else:
			return False

	def recv(self):
		radiohead.recv(buf, l)
		return (ffi.string(buf), l[0])

	def maxMessageLength(self):
		return radiohead.maxMessageLength()

	def printRegisters(self):
		radiohead.printRegisters()

	def sleep(self):
		radiohead.enterSleepMode()

	def recvfromAck(self):
		radiohead.recvfromAck(buf, l, src)
		return (ffi.string(buf), l[0], src[0])

	def recvfromAckTimeout(self, timeout):
		ris = radiohead.recvfromAck(buf, l, timeout, src)
		if ris > 0:
			return (ffi.string(buf), l[0], src[0])
		else:
			return ("", -1, -1)

	def sendtoWait(self, data, l, dst):
		return radiohead.sendtoWait(data, l, dst)

	def retries(self):
		return radiohead.retries()

	def setRetries(self, retries):
		radiohead.setRetries(retries)

	def retransmissions(self):
		return radiohead.retransmissions()

	def resetRetransmissions(self):
		radiohead.resetRetransmissions()

	def setTimeout(self, timeout):
		radiohead.setTimeout(timeout)
	
	def setModeIdle(self):
		radiohead.setModeIdle()
	
	def setModeTx(self):
		radiohead.setModeTx()
	
	def setModeRx(self):
		radiohead.setModeRx()
