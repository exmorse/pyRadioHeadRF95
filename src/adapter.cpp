#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>


// Dragino Raspberry PI hat
// see https://github.com/dragino/Lora
#define BOARD_DRAGINO_PIHAT

// Now we include RasPi_Boards.h so this will expose defined 
// constants with CS/IRQ/RESET/on board LED pins definition
#include "RasPiBoards.h"

// RFM95 Default Configuration 
#define RF_FREQUENCY  868.00
#define RF_GATEWAY_ID 1 
#define RF_NODE_ID    10


RH_RF95 radio(RF_CS_PIN, RF_IRQ_PIN);
RHReliableDatagram* manager = NULL;


int _init() {
        if (!bcm2835_init()) {
                printf("Startup Failed\n");
                return -2;
        }

	#ifdef RF_IRQ_PIN
		//printf( ", IRQ=GPIO%d", RF_IRQ_PIN );
		// IRQ Pin input/pull down 
		pinMode(RF_IRQ_PIN, INPUT);
		bcm2835_gpio_set_pud(RF_IRQ_PIN, BCM2835_GPIO_PUD_DOWN);
	#endif
		
	#ifdef RF_RST_PIN
		//printf( ", RST=GPIO%d", RF_RST_PIN );
		// Pulse a reset on module
		pinMode(RF_RST_PIN, OUTPUT);
		digitalWrite(RF_RST_PIN, LOW );
		bcm2835_delay(150);
		digitalWrite(RF_RST_PIN, HIGH );
		bcm2835_delay(100);
	 #endif
		
	 #ifdef RF_LED_PIN
		//printf( ", LED=GPIO%d", RF_LED_PIN );
		digitalWrite(RF_LED_PIN, LOW );
	 #endif

        if (!radio.init()) {
		printf("Init Failed\n");
		return -1;
	}

	return 0;
}

void _setTxPower(int8_t power, bool useRFO) {
	radio.setTxPower(power, useRFO);
}

bool _setFrequency(float centre) {
	return radio.setFrequency(centre);
}

int _send(uint8_t* data, uint8_t len) {
	bool b = radio.send(data, len);
	if (b) return 0;
	else return -1;
}

int _waitPacketSent() {
	bool b = radio.waitPacketSent();
	if (b) return 0;
	else return -1;
}

int _waitAvailableTimeout(int ms) {
	return radio.waitAvailableTimeout(ms);
}

int _available() {
	/* If manager has been initialized use manager.available(), else radio.available() */
	/* Manager available */
	if (manager != NULL ) {
		return (int) manager->available();
	}
	
	/* Radio available */
	return (int) radio.available();
}

int _recv(char* buf, uint8_t* len) {
	uint8_t buf2[RH_RF95_MAX_MESSAGE_LEN];
	uint8_t len2 = sizeof(buf2);
	
	bool b = radio.recv(buf2, &len2);
	//printf("Received : %s (%d)\n", (char*)buf2, len2);

	strcpy(buf, (char*)buf2);
	//buf[(int)len2] = 0;
	*len = len2; 	

	if (b) return *len;
	else return -1;
}

int _maxMessageLength() {
	return radio.maxMessageLength();
}

int _printRegisters() {
	bool b = radio.printRegisters();
	if (b) return 0;
	else return -1;
}

int _enterSleepMode() {
	bool b = radio.sleep();
	if (b) return 0;
	else return -1;
}

int _managerInit(int address) {
	manager = new RHReliableDatagram(radio, (uint8_t)address); 
        
	if (!bcm2835_init()) {
                printf("Startup Failed\n");
                return -1;
        }
	
	if (!manager->init())
                printf("Init Failed\n");
}

int _recvfromAck(char* buf, uint8_t* len, uint8_t* from) {
	uint8_t buf2[RH_RF95_MAX_MESSAGE_LEN];
	uint8_t len2 = sizeof(buf2);
	uint8_t from2;
		
	bool b = manager->recvfromAck(buf2, &len2, &from2);
	//printf("Received : %s (%d) (from %d)\n", (char*)buf2, len2, from2);
	strcpy(buf, (char*)buf2);
	*len = len2; 	
	*from = from2;

	if (b) return *len;
	else return -1;
}

int _recvfromAckTimeout(char* buf, uint8_t* len, uint16_t timeout, uint8_t* from) {
	uint8_t buf2[RH_RF95_MAX_MESSAGE_LEN];
	uint8_t len2 = sizeof(buf2);
	uint8_t from2;

	bool b = manager->recvfromAckTimeout(buf2, &len2, timeout, &from2);

	if (b) {
		//printf("Received : %s (%d) (from %d)\n", (char*)buf2, len2, from2);
		strcpy(buf, (char*)buf2);
		*len = len2; 	
		*from = from2;
		return *len;
	}

	else {
		printf("Timeout Expired\n");
		return 0;
	}
}	

int _sendtoWait(uint8_t* data, uint8_t len, uint8_t dst) {
	bool b = manager->sendtoWait(data, len, dst);
	if (b) return 0;
	else return -1;
}

int _setTimeout(uint16_t timeout) {
	manager->setTimeout(timeout);
	return 0;
}

int _setRetries(uint8_t retries) {
	manager->setRetries(retries);
	return 0;
}

int _retries() {
	return (int) manager->retries();
}

int _retransmissions() {
	return (int) manager->retransmissions();
}

int _resetRetransmissions() {
	manager->resetRetransmissions();
	return 0;
}

int _setModeIdle() {
	radio.setModeIdle();
	return 0;
}

int _setModeTx() {
	radio.setModeTx();
	return 0;
}

int _setModeRx() {
	radio.setModeRx();
	return 0;
}


extern "C" {
        extern int init() {
                return _init();
        }

	extern void setTxPower(int8_t power, bool useRFO) {
		_setTxPower(power, useRFO);
	}

	extern bool setFrequency(float centre) {
		return _setFrequency(centre);
	}

	extern int send(uint8_t* data, uint8_t len) {
		return _send(data, len);
	}

	extern int waitPacketSent() {
		return _waitPacketSent();
	}

	extern int waitAvailableTimeout(int ms) {
		return _waitAvailableTimeout(ms);
	}

	extern int available() {
		return _available();
	}

	extern int recv(char* buf, uint8_t* len) {
		return _recv(buf, len);
	}

	extern int maxMessageLength() {
		return _maxMessageLength();
	}

	extern int printRegisters() {
		return _printRegisters();
	}

	extern int enterSleepMode() {
		return _enterSleepMode();
	}

	extern int managerInit(int address) {		
		return _managerInit(address);
	}

	extern int recvfromAck(char* buf, uint8_t* len, uint8_t* from) {
		return _recvfromAck(buf, len, from);
	}

	extern int recvfromAckTimeout(char* buf, uint8_t* len, uint16_t timeout, uint8_t* from) {
		return _recvfromAckTimeout(buf, len, timeout, from);
	}

	extern int sendtoWait(uint8_t* data, uint8_t len, uint8_t dst) {
		return _sendtoWait(data, len, dst);
	}

	extern int retries() {
		return _retries();
	}

	extern int setRetries(uint8_t retries) {
		return _setRetries(retries);
	}

	extern int retransmissions() {
		return _retransmissions();
	}

	extern int resetRetransmissions() {
		return _resetRetransmissions();
	}

	extern int setTimeout(uint16_t timeout) {
		return _setTimeout(timeout);
	}
	
	extern int setModeIdle() {
		return _setModeIdle();
	}
	
	extern int setModeTx() {
		return _setModeTx();
	}
	
	extern int setModeRx() {
		return _setModeRx();
	}
}
