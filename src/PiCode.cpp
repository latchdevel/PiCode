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

/* Declared in protocol.h */
extern struct protocols_t* pilight_protocols;

/* Set a default object instance */
class PiCode PiCode;

/* Class PiCode                                                              */
/* ------------------------------------------------------------------------- */

/* Constructor */
PiCode::PiCode(){
  /* Call protocol_init() only one time by default object instance */
  if (pilight_protocols == nullptr){
    protocol_init();
  }
}

/* Aux functions and private methods                                         */
/* ------------------------------------------------------------------------- */

/* Find protocol node (private) */
protocols_t* PiCode::find_protocol_node(const char* name){
  return ::find_protocol_node(name);
}

/* Search index of char in char* from String class (overloaded) */
int indexOf(const char* data, char ch) {
  return ::indexOf(data, ch, 0);
}

/* Public class methods call pure C functions library                        */
/* ------------------------------------------------------------------------- */

/* Find protocol by name */
protocol_t* PiCode::findProtocol(const char* name) {
  return ::findProtocol(name);
}

/* Convert from array of pulses and length to pilight string format. Must be free() after use */
char* PiCode::pulseTrainToString(const uint32_t* pulses, uint16_t length, uint8_t repeats){
  return ::pulseTrainToString(pulses, length, repeats);
}

/* Encode protocol and json parameters to array of pulses if success */
int PiCode::encodeToPulseTrain(uint32_t* pulses, protocol_t* protocol, const char* json_data){
  return ::encodeToPulseTrain(pulses, protocol, json_data);
}

/* Convert from pilight string to array of pulses if success */
int PiCode::stringToPulseTrain(const char* data, uint32_t* pulses, size_t maxlength){
  return ::stringToPulseTrain(data, pulses, maxlength);
}

/* Decode from array of pulses to json as dynamic char*. Must be free() after use */
char* PiCode::decodePulseTrain(uint32_t* pulses, uint16_t length, const char* indent){
  return ::decodePulseTrain(pulses, length, indent);
}

/* Decode from pilight string. Must be free() after use */
char* PiCode::decodeString(const char* pilight_string){
  return ::decodeString(pilight_string);
}

/* Encode to pilight string. Must be free() after use */
char* PiCode::encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats){
  return ::encodeToString(protocol_name, json_data, repeats);
}

/* Encode to pilight string from json. Must be free() after use */
char* PiCode::encodeJson(const char* json, uint8_t repeats){
  return ::encodeJson(json, repeats);
}

/* Get PiCode libray version. Must be free() after use */
char* PiCode::getPiCodeVersion(){
  return ::getPiCodeVersion();
}
