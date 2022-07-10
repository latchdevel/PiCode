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

#include <cstdlib>           /* size_t                   */
#include <cinttypes>         /* uint8_t, etc.            */

namespace cPiCode {
    extern "C" {
        #include "cPiCode.h" /* Pure C PiCode library header */
    }
}

typedef cPiCode::protocol_t         protocol_t;
typedef cPiCode::protocols_t        protocols_t;
typedef cPiCode::protocol_devices_t protocol_devices_t;

/* Class PiCode                                                              */
/* ------------------------------------------------------------------------- */
class PiCode {

 public:
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
  char* decodePulseTrain(const uint32_t* pulses, uint16_t length, const char* indent = "   ");

  /* Decode from pilight string. Must be free() after use */
  char* decodeString(const char* pilight_string);

  /* Encode to pilight string. Must be free() after use */
  char* encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats = 0);

  /* Encode to pilight string from json. Must be free() after use */
  char* encodeJson(const char* json, uint8_t repeats = 0);

  /* Get PiCode libray version. Must be free() after use */
  char* getPiCodeVersion();

  /* Getter for protocols_t* used_protocols */
  protocols_t* usedProtocols(){return cPiCode::pilight_protocols;}

};

/* Expose a default object instance */
extern class PiCode PiCode;

#endif
