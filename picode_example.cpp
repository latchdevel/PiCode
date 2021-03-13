/*  
    Example of using the PiCode Library 
    
    https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

*/

#include "src/PiCode.h"
#include <stdio.h> 

int main(){

    /* Decode from pilight string */

    char* pilight_string = (char*) "c:011010100101011010100110101001100110010101100110101010101010101012;p:1400,600,6800@";

    char* decoded_string = PiCode.decodeString(pilight_string);

    if (decoded_string){

        printf("Decode string successful:\n");
        printf("%s\n",decoded_string);

        free(decoded_string);
    
    }else{
        printf("Unable to decode string\n");
    }

    /* Encode to pilight string from json */

    char*   json    = (char*) "{ 'arctech_switch' : { 'id': 92, 'unit': 0, 'on': 1 }}";
    uint8_t repeats = 5;

    char* encoded_json_string = PiCode.encodeJson(json,repeats);

    if (encoded_json_string){

        printf("Encode successful:\n");
        printf("%s\n",encoded_json_string);

        free(encoded_json_string);
    
    }else{
        printf("Unable to encode\n");
    }

    return 0;
}
