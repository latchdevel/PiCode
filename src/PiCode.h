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

#include "../libs/OutputPrint/src/OutputPrint.h"    /* String class */

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

  /* String class */ /* Convert from array of pulses and lenght to pilight string format */  
  String pulseTrainToStringClass(const uint16_t* pulses, unsigned int length, uint8_t repeats = 0 );

  /* Convert pulses and lenght to pilight string format. Return must be free() */
  char* pulseTrainToString(const uint16_t* pulses, unsigned int length, uint8_t repeats = 0 );

  /* String class */ /* Encode protocol and json data to array of pulses if success */ 
  int encodeToPulseTrain(uint16_t* pulses, protocol_t* protocol, const String& json_data);

  /* Encode protocol and json data to array of pulses if success */
  int encodeToPulseTrain(uint16_t* pulses, protocol_t* protocol, const char* json_data);

  /* String class */ /* Convert from pilight string to array of pulses if success */ 
  int stringToPulseTrain(const String& data, uint16_t* pulses, size_t maxlength);

  /* Convert from pilight string to array of pulses if success */
  int stringToPulseTrain(const char* data, uint16_t* pulses, size_t maxlength);

  /* String class */ /* Decode from array of pulses to String */ 
  String decodePulseTrainToString(uint16_t* pulses, uint8_t length, const char* indent = "   ");

  /* Decode from array of pulses to dynamic char*. Must be free() after use */
  char* decodePulseTrain(uint16_t* pulses, uint8_t length, const char* indent = "   ");

  /* Decode from pilight string. Must be free() after use */
  char* decodeString(const char* pilight_string);

  /* String class */ /* Decode from pilight string */
  String decodeString(const String& pilight_string);

  /* Encode to pilight string. Must be free() after use */
  char* encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats = 0);

  /* Encode to pilight string from json. Must be free() after use */
  char* encodeJson(const char* json, uint8_t repeats = 0);

  /* String class */  /* Encode to pilight string from json */
  String encodeJson(const String& json, uint8_t repeats = 0);

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