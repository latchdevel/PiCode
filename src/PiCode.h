/*  
    PiCode Library 

    C++ class to manage OOK protocols supported by "pilight" project.

    A large number of wireless protocols of RC switches and weather 
    stations using the 433/315Mhz radio frequency band via ASK/OOK 
    pulse modulation are implemented by the excellent work of the 
    "pilight" community.
    
    https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

*/

#ifndef PICODE_H
#define PICODE_H

#include <stdio.h>      /* sprintf              */
#include <string.h>     /* strcmp, strcpy, etc. */
#include <stdlib.h>     /* malloc, free         */

extern "C" {
#include "../libs/pilight/libs/pilight/protocols/protocol.h"
}

/* pilight string pulse index from 0 to 9 like "c:0123456789;" */
#define MAX_PULSE_TYPES 10 

class PiCode {

 public:
  /* Error return codes for encodeToPulseTrain()       */
  static const int ERROR_INVALID_PILIGHT_MSG         = -1;
  static const int ERROR_INVALID_JSON                = -2;
  static const int ERROR_UNAVAILABLE_PROTOCOL        = -3;

  /* Error return codes for stringToPulseTrain()     */
  static const int ERROR_INVALID_PULSETRAIN_MSG_C    = -1;
  static const int ERROR_INVALID_PULSETRAIN_MSG_P    = -2;
  static const int ERROR_INVALID_PULSETRAIN_MSG_END  = -3;
  static const int ERROR_INVALID_PULSETRAIN_MSG_TYPE = -4;
  static const int ERROR_INVALID_PULSETRAIN_MSG_R    = -5;
  static const int ERROR_INVALID_PULSETRAIN_MSG      = -6;

  /* Constructor */
  PiCode();

  /* Find protocol by name */
  protocol_t* findProtocol(const char* name);

  /* Convert pulses and length to pilight string format. Must be free() after use */
  char* pulseTrainToString(const uint32_t* pulses, uint16_t length, uint8_t repeats = 0 );

  /* Encode protocol and json data to array of pulses if success */
  int encodeToPulseTrain(uint32_t* pulses, protocol_t* protocol, const char* json_data);

  /* Convert from pilight string to array of pulses if success */
  int stringToPulseTrain(const char* data, uint32_t* pulses, size_t maxlength);

  /* Decode from array of pulses to json as dynamic char*. Must be free() after use */
  char* decodePulseTrain(uint32_t* pulses, uint16_t length, const char* indent = "   ");

  /* Decode from pilight string. Must be free() after use */
  char* decodeString(const char* pilight_string);

  /* Encode to pilight string. Must be free() after use */
  char* encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats = 0);

  /* Encode to pilight string from json. Must be free() after use */
  char* encodeJson(const char* json, uint8_t repeats = 0);

  /* Getter for protocols_t* used_protocols */
  protocols_t* usedProtocols(){return used_protocols;}

private:
    /* Private property to store used protocol */   
    protocols_t* used_protocols;

    /* Aux find protocol node */
    protocols_t* find_protocol_node(const char* name);
};

/* Expose a default object instance */
extern class PiCode PiCode;

#endif