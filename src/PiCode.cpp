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

#include "PiCode.h"

/* Set a default object instance */
class PiCode PiCode;

/* Class PiCode                                                              */
/* ------------------------------------------------------------------------- */

/* Constructor */
PiCode::PiCode(){
  /* Call protocol_init() only one time by default object instance */
  if (cPiCode::pilight_protocols == nullptr){
      cPiCode::protocol_init();
  }
}

/* Public class methods call pure C functions library                        */
/* ------------------------------------------------------------------------- */

/* Find protocol by name */
protocol_t* PiCode::findProtocol(const char* name) {
  return cPiCode::findProtocol(name);
}

/* Convert from array of pulses and length to pilight string format. Must be free() after use */
char* PiCode::pulseTrainToString(const uint32_t* pulses, uint16_t length, uint8_t repeats){
  return cPiCode::pulseTrainToString(pulses, length, repeats);
}

/* Encode protocol and json parameters to array of pulses if success */
int PiCode::encodeToPulseTrain(uint32_t* pulses, protocol_t* protocol, const char* json_data){
  return cPiCode::encodeToPulseTrain(pulses, protocol, json_data);
}

/* Convert from pilight string to array of pulses if success */
int PiCode::stringToPulseTrain(const char* data, uint32_t* pulses, size_t maxlength){
  return cPiCode::stringToPulseTrain(data, pulses, maxlength);
}

/* Decode from array of pulses to json as dynamic char*. Must be free() after use */
char* PiCode::decodePulseTrain(uint32_t* pulses, uint16_t length, const char* indent){
  return cPiCode::decodePulseTrain(pulses, length, indent);
}

/* Decode from pilight string. Must be free() after use */
char* PiCode::decodeString(const char* pilight_string){
  return cPiCode::decodeString(pilight_string);
}

/* Encode to pilight string. Must be free() after use */
char* PiCode::encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats){
  return cPiCode::encodeToString(protocol_name, json_data, repeats);
}

/* Encode to pilight string from json. Must be free() after use */
char* PiCode::encodeJson(const char* json, uint8_t repeats){
  return cPiCode::encodeJson(json, repeats);
}

/* Get PiCode libray version. Must be free() after use */
char* PiCode::getPiCodeVersion(){
  return cPiCode::getPiCodeVersion();
}
