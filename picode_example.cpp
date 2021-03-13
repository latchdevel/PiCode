/*  
    Example of using the PiCode Library 
    
    https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

*/

#include "src/PiCode.h" // from OutputPrint include String class and Serial.print()
#include <stdio.h> 

int main(){

    /* Decode from pilight string */

    String pilight_string = "c:011010100101011010100110101001100110010101100110101010101010101012;p:1400,600,6800@";

    String decoded_string = PiCode.decodeString(pilight_string);

    if (decoded_string){

        Serial.println("Decode string successful:");
        Serial.println(decoded_string);
    
    }else{
        Serial.println("Unable to decode string");
    }

    /* Encode to pilight string from json */

    String  json    = "{ 'arctech_switch' : { 'id': 92, 'unit': 0, 'on': 1 }}";
    uint8_t repeats = 5;

    String encoded_json_string = PiCode.encodeJson(json,repeats);

    if (encoded_json_string){

        Serial.println("Encode successful:");
        Serial.println(encoded_json_string);

    }else{
        Serial.println("Unable to encode");
    }

    return 0;
}
