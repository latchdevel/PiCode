# PiCode Library

C/C++ library to manage OOK protocols supported by [**"pilight"**](https://github.com/pilight/pilight) project.

Works on any libc/libc++ compatible system, like macOS, FreeBSD, Linux, even Windows.

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![Build tests](https://github.com/latchdevel/PiCode/actions/workflows/BuildTest.yml/badge.svg)](https://github.com/latchdevel/PiCode/actions/workflows/BuildTest.yml)

A large number of wireless protocols of RC switches and weather stations using the 433/315Mhz radio frequency band via ASK/OOK pulse modulation are implemented by the excellent work of the "pilight" community. A list of supported protocols can be found in the pilight manual: https://manual.pilight.org/protocols/433.92/index.html

Inspired by [**"ESPiLight"**](https://github.com/puuu/ESPiLight) project, provides a standard C++ class to should help to implement IoT bridges between the RF 433/315MHz band and internet protocols.

The "pilight" project defines a special string format to easily transcribe an OOK pulse train, in order to be able to exchange it with other systems, such as a remote RF receiver/transmitter.

pilight string format: `c: <pulses_index> ; p: <pulse_0_lenght>, <pulse_1_lenght>, ... <pulse_n_lenght> [;r:<repeats>] @`

pilight string example: `c:001010101100101010101010101010110010101102;p:700,1400,7650;r:5@`

The main features are to encode and decode the pilight strings.
When decoding a pilight string is successful, a json array is returned containing the protocol matches, as sometimes decoded string matches more than one protocol.

pilight string: `c:010002000200020002000200020002000200020002000200020002000200020002000200020002020000020200020002000002000200020200000200020002000203;p:315,2835,1260,10710@`

decode as:
```json
{
  "protocols": [
    {
      "arctech_contact": {
        "id": 92,
        "unit": 0,
        "state": "opened"
      }
    },
    {
      "arctech_dimmer": {
        "id": 92,
        "unit": 0,
        "state": "on"
      }
    },
    {
      "arctech_screen": {
        "id": 92,
        "unit": 0,
        "state": "up"
      }
    },
    {
      "arctech_switch": {
        "id": 92,
        "unit": 0,
        "state": "on"
      }
    },
    {
      "smartwares_switch": {
        "id": 92,
        "unit": 0,
        "state": "on"
      }
    }
  ]
}
```

To encode to pilight string, you must provide the protocol and custom parameters of that protocol in json format.

Example: `{"arctech_switch":{"id":92,"unit":0,"on":1}}`


## BUILD
No external depends, can run on any libc/libc++ compatible system, like macOS, FreeBSD, Linux, even Windows.
 ```
    $ git clone https://github.com/latchdevel/PiCode (or download .zip)
    $ cd PiCode
    $ mkdir build
    $ cd build
    $ cmake .. (or "cmake -DCMAKE_BUILD_TYPE=debug .." for debug)
    $ make
    $ make install (optional)
    # make picode_example (optional C++ example)
    # make cpicode_example (optional C example)
    $ make uninstall (to uninstall)
```


## C example
```c
/*  
    Example of using the pure C PiCode Library 
    
    https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

*/

#include <stdio.h>          /* printf()              */
#include <stdlib.h>         /* free(), uint8_t       */

#include "src/cPiCode.h"    /* Pure C PiCode library */

int main(){

    int result = 0;

    printf("cpicode_example (%s)\n", STRINGIFY(BUILD_VERSION));
    printf("Compiled at " __DATE__ " " __TIME__ " %s (%s)\n",STRINGIFY(BUILD_COMPILER), BUILD_TYPE );

    /* Get PiCode library version */

    char* library_version = getPiCodeVersion();

    if (library_version){
        printf("PiCode library version: %s\n", library_version);
        free(library_version);
    }else{
        printf("ERROR: Unable to get PiCode library version.\n");
        result--;
    }

    printf("\n");

    /* Decode from pilight string */

    char* pilight_string = (char*) "c:011010100101011010100110101001100110010101100110101010101010101012;p:1400,600,6800@";

    char* decoded_string = decodeString(pilight_string);

    printf("String to decode: \"%s\"\n",pilight_string);

    if (decoded_string){

        printf("Decode string successful:\n");
        printf("%s\n",decoded_string);

        free(decoded_string);
    
    }else{
        printf("ERROR: Unable to decode string.\n");
        result--;
    }

    /* Encode to pilight string from json */

    char*   json    = (char*) "{ 'arctech_switch' : { 'id': 92, 'unit': 0, 'on': 1 }}";
    uint8_t repeats = 5;

    char* encoded_json_string = encodeJson(json,repeats);

    printf("\nJSON to encode: \"%s\"\n",json);

    if (encoded_json_string){

        printf("Encode successful:\n");
        printf("%s\n",encoded_json_string);

        free(encoded_json_string);
    
    }else{
        printf("ERROR: Unable to encode JSON.\n");
        result--;
    }

    /* Encode from protocol name and json data to array of pulses if success */

    char* protocol_name = (char*) "arctech_switch";
    char* json_data     = (char*) "{'id': 92, 'unit': 0, 'on': 1}";

    uint32_t*   pulses     = NULL;
    uint16_t  n_pulses_max = 0;
    int       n_pulses     = 0;

    n_pulses_max = protocol_maxrawlen();

    pulses = (uint32_t*)malloc(sizeof *pulses * (n_pulses_max + 1));

    if (pulses != NULL){

        printf("\nEncode protocol: \"%s\" JSON data: \"%s\"\n",protocol_name,json_data);
        
        n_pulses = encodeToPulseTrainByName(pulses, n_pulses_max, protocol_name, json_data);

        if (n_pulses>0){
            printf("Encode successful:\n");
            printf("pulses[%d]={",n_pulses);
            for (int i = 0; i<n_pulses; i++){
                printf("%d",pulses[i]);
                if (i<n_pulses-1){
                    printf(",");
                }else{
                    printf("};\n");
                }
            }
        }else{
            printf("ERROR: Unable to encode (%i)\n",n_pulses);
        }
        free(pulses);
    }else{
        printf("ERROR: out of memory (%d)) \n",n_pulses_max);
        result--;
    }

    printf("\n");
    return result;
}
```

## Output
```
picode_example (539485e)
Compiled at Oct  6 2022 23:10:26 AppleClang 13.0.0.13000029 (Release)
PiCode library version: v1.4-539485e

String to decode: "c:011010100101011010100110101001100110010101100110101010101010101012;p:1400,600,6800@"
Decode string successful:
{
   "protocols": [{
      "conrad_rsl_switch": {
         "id": 1,
         "unit": 2,
         "state": "on"
      }
   }]
}

JSON to encode: "{ 'arctech_switch' : { 'id': 92, 'unit': 0, 'on': 1 }}"
Encode successful:
c:010002000200020002000200020002000200020002000200020002000200020002000200020002020000020200020002000002000200020200000200020002000203;p:315,2835,1260,10710;r:5@

Encode protocol: "arctech_switch" JSON data: "{'id': 92, 'unit': 0, 'on': 1}"
Encode successful:
pulses[132]={315,2835,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,1260,315,315,315,315,315,1260,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,1260,315,315,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,315,315,1260,315,10710};
```

# License
Copyright (c) 2021-2022 Jorge Rivera. All right reserved.

License GNU Lesser General Public License v3.0.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License 
along with this library; if not, write to the Free Software Foundation, 
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.

See the [LICENSE](LICENSE.md) file for license rights and limitations (lgpl-3.0).
