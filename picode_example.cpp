/*  
    Example of using the PiCode Library 
    
    https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

*/

#include <cstdio>           /* printf()            */
#include <cstdlib>          /* free()              */
#include <cinttypes>        /* uint8_t             */

#include "src/PiCode.h"     /* PiCode object class */

int main(){

    int result = 0;

    printf("picode_example (%s)\n", STRINGIFY(BUILD_VERSION));
    printf("Compiled at " __DATE__ " " __TIME__ " %s (%s)\n",STRINGIFY(BUILD_COMPILER), BUILD_TYPE );

    /* Get PiCode library version */

    char* library_version = PiCode.getPiCodeVersion();

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

    char* decoded_string = PiCode.decodeString(pilight_string);

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

    char* encoded_json_string = PiCode.encodeJson(json,repeats);

    printf("\nJSON to encode: \"%s\"\n",json);

    if (encoded_json_string){

        printf("Encode successful:\n");
        printf("%s\n",encoded_json_string);

        free(encoded_json_string);
    
    }else{
        printf("ERROR: Unable to encode JSON.\n");
        result--;
    }
    printf("\n");
    return result;
}
