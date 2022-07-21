/*  
    PiCode Library 

    Pure C library to manage OOK protocols supported by "pilight" project.

    A large number of wireless protocols of RC switches and weather 
    stations using the 433/315Mhz radio frequency band via ASK/OOK 
    pulse modulation are implemented by the excellent work of the 
    "pilight" community.
    
    https://github.com/latchdevel/PiCode

    Copyright (c) 2021 Jorge Rivera. All right reserved.
    License GNU Lesser General Public License v3.0.

*/

#include <stdio.h>           /* sprintf()                */
#include <string.h>          /* strcmp(), strcpy(), etc. */
#include <stdlib.h>          /* malloc(), free(), etc.   */
#include <inttypes.h>        /* uint8_t, etc.            */

#include "cPiCode.h"         /* Pure C PiCode library .h */

/* Declared in protocol.h from pilight sources included  */
extern protocols_t* pilight_protocols;

/* Aux functions                                                             */
/* ------------------------------------------------------------------------- */

/* Find protocol node */
static protocols_t* find_protocol_node(const char* name) {
  if (pilight_protocols==NULL){protocol_init();}
  protocols_t *pnode = pilight_protocols;
  while (pnode != NULL) {
    if (strcmp(name, pnode->listener->id) == 0) {
      return pnode;
    }
    pnode = pnode->next;
  }
  return NULL;
}

/* Search index of char in char* from String class */
static int indexOf(const char* data, char ch, unsigned int fromIndex) {
	if (fromIndex >= strlen(data)) return -1;
	const char* temp = strchr(data + fromIndex, ch);
	if (temp == NULL) return -1;
	return (int)(temp - data);
}

/* Library functions                                                         */
/* ------------------------------------------------------------------------- */

/* Find protocol by name */
protocol_t* findProtocol(const char* name) {
  protocols_t *pnode = find_protocol_node(name);
  if (pnode != NULL) {
    return pnode->listener;
  }
  return NULL;
}

/* Convert from array of pulses and length to pilight string format. Must be free() after use */
char* pulseTrainToString(const uint32_t* pulses, uint16_t maxlength, uint8_t repeats){
  
  bool match = false;
  int  diff  = 0;

  // Check for maxlength
  if (maxlength < 2) return NULL;

  uint8_t  nrpulses                  =  0 ;  // number of pulse types
  uint32_t plstypes[MAX_PULSE_TYPES] = {0};  // array to store pulse types

  // Aux string to compose data string //
  char pulse_str[11] = {0};      

  // Dynamic string to return. Must be free() after use //
  // Reserve max memory      "c:             ;   p:      4294967295,            ;  r:31;  @\0"
  char* data = (char*)malloc( (size_t)(2 + (maxlength) + 1 + 2 + (MAX_PULSE_TYPES*11) + 1 + (5) +  2) );            

  if (!data){
    return NULL;
  }else{
    strcpy(data,"c:");
  }

  for (uint16_t i = 0; i < maxlength; i++) {
    match = false;
    for (uint8_t j = 0; j < MAX_PULSE_TYPES; j++) {
      // We device these numbers by 10 to normalize them a bit
      diff = (int)((plstypes[j] / 50) - (pulses[i] / 50));
      if ((diff >= -2) && (diff <= 2)) {
        // Write numbers
        sprintf(pulse_str,"%c",(char)('0' + ((char)j))); 
        strcat(data, pulse_str );
        match = true;
        break;
      }
    }

    if (!match) {
      plstypes[nrpulses++] = pulses[i];
        sprintf(pulse_str,"%c",(char)('0' + ((char)(nrpulses - 1)))); 
        strcat(data, pulse_str);
      if (nrpulses > MAX_PULSE_TYPES) {
        return NULL;
      }
    }

  }

  strcat(data, ";p:" );

  for (uint8_t i = 0; i < nrpulses; i++) {

    sprintf(pulse_str, "%d", plstypes[i]); 
    strcat(data, pulse_str);

    if (i + 1 < nrpulses) {
      strcat(data, "," );
    }
  }
  if (repeats > 0 ){
    strcat(data, ";r:" );
    sprintf(pulse_str, "%d", repeats); 
    strcat(data, pulse_str);

  }
  strcat(data, "@" );

  // Reduce dynamic memory to only used //
  data = (char*)realloc(data,strlen(data)+1);
  
  return data;
}

/* Convert from array of pulses and length to normalized pilight string format. Must be free() after use */
char* normalizedPulseTrainToString(const uint32_t* pulses, uint16_t maxlength){

  char* picode = NULL;    // To return the picode string, must be free after use.

  // Check for maxlength
  if (maxlength < 1 ) return NULL;

  uint16_t max_pulses = maxlength+1; // Prevent overflow if add pulse if odd

  // Local array to store pulse train (nomarlized and typed) 
  struct { 
    uint32_t pulse_raw_length;
    uint8_t  pulse_type_index;
  } normalized_pulses[max_pulses]; 

  // Local index of pulse train array
  uint16_t index;

  // Local index of pulse types array
  uint8_t pulse_type_index = 0;

  // Local array to store average length each pulse type
  float pulse_types[MAX_PULSE_TYPES] = {0.0};

  // Indentify pulse types
  for ( index = 0 ; index < maxlength; index ++){
    if (pulse_type_index == 0){ // First pulse

        // Store raw pulse length
        pulse_types[pulse_type_index] = (float)pulses[index];
        
        // Store pulse type
        normalized_pulses[index].pulse_type_index = pulse_type_index;

        // Ready for next pulse type
        pulse_type_index++;

        // Check for max pulse types overflow
        if (pulse_type_index > MAX_PULSE_TYPES){
          return NULL;
        }
    }else{
        uint8_t new_pulse = 0;

        for (uint8_t i = 0; i < pulse_type_index; i ++){

            // Check if pulse length match with stored pulse types
            if (((float)pulses[index] <= pulse_types[i]*(1+MAX_PULSE_DRIFT)) && ((float)pulses[index] >= pulse_types[i]*(1-MAX_PULSE_DRIFT))) {
                
                // Update pulse type average length
                pulse_types[i] = (pulse_types[i]+(float)pulses[index])/2.0;
  
                // Store pulse type
                normalized_pulses[index].pulse_type_index = i;

                new_pulse = 1;
                break;
            }

        }

        // No match, add new pulse type
        if (new_pulse == 0){

            // Store raw pulse length
            pulse_types[pulse_type_index] = (float)pulses[index];

            // Store pulse type
            normalized_pulses[index].pulse_type_index = pulse_type_index;

            // Ready for next pulse type
            pulse_type_index++;

            // Check for max pulse types overflow
            if (pulse_type_index > MAX_PULSE_TYPES){
              return NULL;
            }
        }
    }
  }

  // Ready to process 
  
  // Type (index) of footer pulse
  uint8_t footer_type   = 0;

  // Normalized length of footer pulse 
  uint32_t footer_length = 0;

  // Count of footer pulses
  uint8_t footer_count  = 0;
  
  // Aux var to store pulse length normalized to tens of usecs
  uint32_t normalized;

  // Local array to store normalized length each pulse type
  uint32_t pulse_types_normalized[MAX_PULSE_TYPES] = {0};

  for (uint8_t t = 0; t < pulse_type_index ; t ++){

      // Pulse lenght in usecs form average
      normalized = (uint32_t)pulse_types[t];

      // Normalized pulse length to tens of usecs
      normalized = (normalized/10 + (normalized%10>=10/2)) * 10;

      // Ensure minimal pulse length to 10
      if (normalized < 10) normalized = 10;

      //printf("[ENCODER] Pulse type:  %d average length: %9.3f (usecs) normalized to: %5d (usecs)\n",t, pulse_types[t], normalized) ;

      // Populate normalized pulse types array
      pulse_types_normalized[t]=normalized;

      // Identify footer pulse length and type index
      if (normalized> footer_length){
          footer_type = t;
          footer_length = normalized;
      }
  }

  for ( index = 0 ; index < maxlength; index ++){

      // Populate normalized pulses array with normalized pulse length
      normalized_pulses[index].pulse_raw_length = pulse_types_normalized[normalized_pulses[index].pulse_type_index];

      // Count of footer pulses
      if (normalized_pulses[index].pulse_type_index == footer_type){
          footer_count++;
      }
  }

  //printf("[ENCODER] Footer type: %d lenght: %d count: %d\n",footer_type, footer_length, footer_count);

  // Check if number of pulses is even
  if (maxlength%2!=0) {
      // Add footer pulse
      normalized_pulses[maxlength].pulse_type_index = footer_type;
      normalized_pulses[maxlength].pulse_raw_length = footer_length;
      maxlength++;
      footer_count++;
      //printf("[ENCODER] Pulse count is odd, adding a footer pulse\n");
  }

  //printf("[ENCODER] Total pulses: %d footer pulses: %d\n",maxlength,footer_count);

  // PiCode encoding process

  // Aux string to compose data string //
  char pulse_str[11] = {0};        

  // String for normalized pulse types, size max_pulses+1
  char normalized_pulses_string[max_pulses+1];

  // Ensure to clear normalized pulse types string
  memset(normalized_pulses_string,0, sizeof normalized_pulses_string );

  // Charge normalized pulse types string
  for (index = 0; index<maxlength; index++){
      normalized_pulses_string[index] = normalized_pulses[index].pulse_type_index+'0';
  }

  // Dynamic string to return. Must be free() after use //
  // Reserve max memory           "c:                ;   p:      4294967295,         @\0"
  picode = (char*)malloc( (size_t)(2 + (maxlength) + 1 + 2 + (pulse_type_index*11) +  2 ));    

  if (picode){

    // Add header and pulses
    sprintf(picode,"c:%s;p:",normalized_pulses_string);
   
    // Add pulse types
    for (uint8_t i = 0; i < pulse_type_index; i++){
        sprintf(pulse_str, "%d", pulse_types_normalized[i]); 
        strcat(picode, pulse_str);
        if (i + 1 < pulse_type_index) {
            strcat(picode, "," );
        }else{
           strcat(picode, "@" );
        }
    }

    // Reduce dynamic memory to only used //
    picode = (char*)realloc(picode,strlen(picode)+1);
  }
  
  return picode;
}

/* Encode protocol and json parameters to array of pulses if success */
int encodeToPulseTrain(uint32_t* pulses, uint16_t maxlength, protocol_t* protocol, const char* json_data){

  int result = ERROR_UNAVAILABLE_PROTOCOL;

  if (json_data == NULL) return ERROR_INVALID_PULSETRAIN_MSG;

  /* local copy of json_data */
  char* n_json = (char*)malloc(strlen(json_data)+1);
  strcpy(n_json,json_data);

  /* replace simple ' to double " */
  int i = 0;
  while(n_json[i] != '\0'){
    if(n_json[i] == '\''){
        n_json[i] = '\"';
    }
    i++;
  }

  if (!json_validate(n_json)) {
    result = ERROR_INVALID_JSON;
  }else{
    if (protocol != NULL) {
      if (protocol->createCode != NULL) {      // Check if protocol can encode
        if (protocol->maxrawlen < maxlength){  // Check if array size is enough
          protocol->rawlen = 0;
          protocol->raw = pulses;

          JsonNode *message = json_decode(n_json);
          int return_value = protocol->createCode(message);

          json_delete(message);
          // delete message created by createCode()
          json_delete(protocol->message);
          protocol->message = NULL;

          if (return_value == EXIT_SUCCESS) {
            result = protocol->rawlen;
          } else {
            result = ERROR_INVALID_PILIGHT_MSG;
          }
        }else{
          result = ERROR_NOT_ENOUGH_PULSES_ARRAY_SIZE;
        }
      }else{
        result = ERROR_PROTOCOL_CANNNOT_ENCODE;
      }
    }
  }
  free(n_json);
  return result;
}

/* Encode from protocol name and json data to array of pulses if success */
int encodeToPulseTrainByName(uint32_t* pulses, uint16_t maxlength, const char* protocol_name, const char* json_data){

  protocol_t* protocol = NULL; 

  protocol = findProtocol(protocol_name);

  if (protocol == NULL) return ERROR_UNAVAILABLE_PROTOCOL;

  return encodeToPulseTrain(pulses, maxlength, protocol, json_data);
}

/* Convert from pilight string to array of pulses if success */
int stringToPulseTrain(const char* data, uint32_t* pulses, uint16_t maxlength){

  int          length                    =  0 ;    // length of pulse train
  uint8_t      nrpulses                  =  0 ;    // number of pulse types
  uint32_t     plstypes[MAX_PULSE_TYPES] = {0};    // array to store pulse types

  // Aux string to compose data string //
  char pulse_str[6] = {0};   

  // validate data string
  int scode = indexOf(data,'c',0) + 2;
  if (scode < 2 || (unsigned)scode >  strlen(data)) {
    // 'c' not found in data string, or has no data
    return ERROR_INVALID_PULSETRAIN_MSG_C;
  }
  int spulse = indexOf(data,'p',0) + 2;
  if (spulse < 2 || (unsigned)spulse > strlen(data)) {
    // 'p' not found in data string, or has no data
    return ERROR_INVALID_PULSETRAIN_MSG_P;
  }

  // parsing pulse types
  unsigned int start = (unsigned)spulse;
  int            end = indexOf(data,',', start);

  while (end > 0) {
    memset(pulse_str,0,sizeof(pulse_str));
    if ((unsigned)end-start > sizeof(pulse_str)-1) {
      return -2;
    }else{
      strncpy(pulse_str, data+start, (unsigned)end-start);   
      plstypes[nrpulses++] =  (uint32_t)atol(pulse_str);
    }
    start = (unsigned)end + 1;
    end = indexOf(data,',', start);
  }

  end = indexOf(data,';', start);
  if (end < 0) {
    end = indexOf(data,'@', start);
  }
  if (end < 0) {
    // ';' or '@' not found in data string
    return ERROR_INVALID_PULSETRAIN_MSG_END;
  }

  memset(pulse_str,0,sizeof(pulse_str));
  if ((unsigned)end-start > sizeof(pulse_str)-1) {
    return -3;
  }else{
    strncpy(pulse_str, data+start, (unsigned)end-start);
    plstypes[nrpulses++] =  (uint32_t)atol(pulse_str);
  }

  // parsing pulses
  int pulse_index = 0;
  for (unsigned int i = (unsigned)scode; i < strlen(data); i++) {
    if ((data[i] == ';') || (data[i] == '@')) break;
    if (i >= maxlength) break;
    pulse_index = data[i] - '0';
    if ((pulse_index < 0) || ((unsigned)pulse_index >= nrpulses)) {
      // Pulse type not defined
      return ERROR_INVALID_PULSETRAIN_MSG_TYPE;
    }
    pulses[length++] = plstypes[pulse_index];
  }
  return length;
}

/* Decode from array of pulses to json as dynamic char*. Must be free() after use */
char* decodePulseTrain(const uint32_t* pulses, uint16_t length, const char* indent){
  uint16_t matches = 0;

  char *result = NULL;

  protocol_t *protocol = NULL;
  if (pilight_protocols==NULL){protocol_init();}
  protocols_t *pnode = pilight_protocols;

  JsonNode  *output_json = json_mkarray();

  JsonNode  *protocol_json = NULL;
  JsonNode  *message_json = NULL;

  while (pnode != NULL) {
    protocol = pnode->listener;

    if (protocol->parseCode != NULL && protocol->validate != NULL) {
      protocol->raw = (uint32_t*)pulses;
      protocol->rawlen = length;

      if (protocol->validate() == 0) {

        protocol->message = NULL;

        protocol->parseCode();

        if (protocol->message != NULL) {

          // Protocol Match!
          matches++;

          protocol_json = json_mkobject();
          message_json  = json_mkobject();

          json_clone(protocol->message, &message_json);
          
          json_append_member(protocol_json, protocol->id, message_json );
          json_append_element(output_json , protocol_json);

          json_delete(protocol->message);
          protocol->message = NULL;

        }
      }
    }
    pnode = pnode->next;
  }
  
  if (strlen(indent)>0){
    result = json_stringify(output_json, indent);
  }else{
    result = json_encode(output_json);
  }

  for (uint16_t i = 0 ; i<matches; i++){
    json_delete(json_first_child(output_json));
  }
  json_delete(output_json);

  return result;
}

/* Decode from pilight string. Must be free() after use */
char* decodeString(const char* pilight_string){

  char*    result             = NULL;
  uint32_t pulses[MAX_PULSES] = {0};
  int      n_pulses           =  0;

  if (pilight_string != NULL){
     n_pulses = stringToPulseTrain(pilight_string, pulses, MAX_PULSES); 
     if (n_pulses > 0){
        result = decodePulseTrain(pulses, (uint16_t)n_pulses, "   ");
        if (result!=NULL){
            if (strlen(result) <  4){ // emply json []
              free(result);
              result=NULL;
            }
        } 
     }
  }
  return result;
}

/* Encode to pilight string. Must be free() after use */
char* encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats){

  char*       result             = NULL;  
  protocol_t* protocol           = NULL;
  int         n_pulses           =  0;

  // Max possible number of pulses from protocol.h
  uint16_t    maxlength = protocol_maxrawlen();

  // Dynamic array of pulses
  uint32_t    pulses[maxlength]; 
  
  // Clear array of pulses
  memset(pulses,0,maxlength);

  if (protocol_name != NULL  &&  json_data != NULL ) {
    protocol  = findProtocol(protocol_name);
    if (protocol != NULL){
      if (protocol->createCode != NULL){
        n_pulses = encodeToPulseTrain(pulses, maxlength, protocol, json_data);
        if (n_pulses > 0){
            result = pulseTrainToString(pulses,(uint16_t)n_pulses, repeats);
        }
      }
    }
  }
  return result;
}

/* Encode to pilight string from json. Must be free() after use */
char* encodeJson(const char* json, uint8_t repeats){

  char* result = NULL;

  if (json != NULL){ 

    /* local copy of json */
    char* n_json = (char*)malloc(strlen(json)+1);
    strcpy(n_json,json);

    /* replace simple ' to double " */
    int i = 0;
    while(n_json[i] != '\0'){
        if(n_json[i] == '\''){
            n_json[i] = '\"';
        }
        i++;
    }
    
    /* decode as root json */
    JsonNode* root_json = json_decode(n_json);
    if (root_json != NULL){

      /* check for child */
      JsonNode* child_json = json_first_child(root_json);
      if (child_json != NULL){            
      
        /* check for child key */
        if (child_json->key != NULL){

          char* protocol_name = child_json->key;
          char* json_data     = json_encode(child_json);

          if (json_data != NULL){
            result = encodeToString(protocol_name,json_data,repeats);
            free(json_data);
          }
        }
        json_delete(child_json);
      }
      json_delete(root_json); 
    }
    free(n_json);
  }
  return result;
}

/* Get PiCode libray version. Must be free() after use */
char* getPiCodeVersion(void){

  // Reserve dynamic string to return. Must be free() after use //
  char* version = (char*)malloc( 255 );

  if (version!=NULL){
    snprintf(version, 255, "v%s-%s", STRINGIFY(CU_VERSION), STRINGIFY(BUILD_VERSION));
 
    // Reduce dynamic memory to only used //
    version = (char*)realloc(version,strlen(version)+1);
  }

  return version;
}

/* Getter for protocols_t* pilight_protocols */
protocols_t* usedProtocols(void){
  if (pilight_protocols==NULL){protocol_init();}
  return pilight_protocols;
}
