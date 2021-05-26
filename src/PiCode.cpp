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

// ------------------------------------------------------------------------- //

/* Constructor */
PiCode::PiCode(){
  /* Ensure to call init_protocols() only one time by default object instance */
  if (pilight_protocols == nullptr){
    protocol_init();
  }
  used_protocols = pilight_protocols;
}

/* Aux find protocol node */
protocols_t* PiCode::find_protocol_node(const char* name) {
  protocols_t *pnode = used_protocols;
  while (pnode != nullptr) {
    if (strcmp(name, pnode->listener->id) == 0) {
      return pnode;
    }
    pnode = pnode->next;
  }
  return nullptr;
}

/* Find protocol by name */
protocol_t* PiCode::findProtocol(const char* name) {
  protocols_t *pnode = find_protocol_node(name);
  if (pnode != nullptr) {
    return pnode->listener;
  }
  return nullptr;
}

/* Convert from array of pulses and lenght to pilight string format. Must be free() after use */
char* PiCode::pulseTrainToString(const uint32_t* pulses, unsigned int length, uint8_t repeats){
  
  bool match = false;
  int  diff  = 0;

  uint8_t  nrpulses                  =  0 ;  // number of pulse types
  uint32_t plstypes[MAX_PULSE_TYPES] = {0};  // array to store pulse types

  // Aux string to compose data string //
  char pulse_str[6] = {0};      

  // Dynamic string to return. Must be free() after use //
  // Reserve max memory      "c:             ;   p:      65535,            ;  r:31;  @\0"
  char* data = (char*)malloc( 2 + (length) + 1 + 2 + (MAX_PULSE_TYPES*6) + 1 + (5) +  2 );            

  if (!data){
    return nullptr;
  }else{
    strcpy(data,"c:");
  }

  for (unsigned int i = 0; i < length; i++) {
    match = false;
    for (uint8_t j = 0; j < MAX_PULSE_TYPES; j++) {
      // We device these numbers by 10 to normalize them a bit
      diff = (plstypes[j] / 50) - (pulses[i] / 50);
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
        return nullptr;
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

/* Encode protocol and json parameters to array of pulses if success */
int PiCode::encodeToPulseTrain(uint32_t* pulses, protocol_t* protocol, const char* json_data){

  int result = PiCode::ERROR_UNAVAILABLE_PROTOCOL;

  if (json_data == nullptr) return PiCode::ERROR_INVALID_PULSETRAIN_MSG;

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
    result = PiCode::ERROR_INVALID_JSON;
  }else{
    if ( (protocol != nullptr) && (protocol->createCode != nullptr) ) {  // always (protocol->maxrawlen <= std::numeric_limits<typeof(protocol->maxrawlen)>::max()

      protocol->rawlen = 0;
      protocol->raw = pulses;

      JsonNode *message = json_decode(n_json);
      int return_value = protocol->createCode(message);

      json_delete(message);
      // delete message created by createCode()
      json_delete(protocol->message);
      protocol->message = nullptr;

      if (return_value == EXIT_SUCCESS) {
        result = protocol->rawlen;
      } else {
        result = PiCode::ERROR_INVALID_PILIGHT_MSG;
      }
    }
  }
  free(n_json);
  return result;
}

/* Aux indexOf from String class */
static int indexOf(const char* data, char ch, unsigned int fromIndex = 0) {
	if (fromIndex >= strlen(data)) return -1;
	const char* temp = strchr(data + fromIndex, ch);
	if (temp == NULL) return -1;
	return (int)(temp - data);
}

/* Convert from pilight string to array of pulses if success */
int PiCode::stringToPulseTrain(const char* data, uint32_t* pulses, size_t maxlength){

  int          length                    =  0 ;    // length of pulse train
  unsigned int nrpulses                  =  0 ;    // number of pulse types
  uint32_t     plstypes[MAX_PULSE_TYPES] = {0};    // array to store pulse types

  // Aux string to compose data string //
  char pulse_str[6] = {0};   

  // validate data string
  int scode = indexOf(data,'c') + 2;
  if (scode < 2 || (unsigned)scode >  strlen(data)) {
    // 'c' not found in data string, or has no data
    return ERROR_INVALID_PULSETRAIN_MSG_C;
  }
  int spulse = indexOf(data,'p') + 2;
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
char* PiCode::decodePulseTrain(uint32_t* pulses, uint8_t length, const char* indent){
size_t matches = 0;

  char *result = nullptr;

  protocol_t *protocol = nullptr;
  protocols_t *pnode = used_protocols;

  JsonNode  *output_json = json_mkarray();

  JsonNode  *protocol_json = nullptr;
  JsonNode  *message_json = nullptr;

  while (pnode != nullptr) {
    protocol = pnode->listener;

    if (protocol->parseCode != nullptr && protocol->validate != nullptr) {
      protocol->raw = pulses;
      protocol->rawlen = length;

      if (protocol->validate() == 0) {

        protocol->message = nullptr;

        protocol->parseCode();

        if (protocol->message != nullptr) {

          // Protocol Match!
          matches++;

          protocol_json = json_mkobject();
          message_json  = json_mkobject();

          json_clone(protocol->message, &message_json);
          
          json_append_member(protocol_json, protocol->id, message_json );
          json_append_element(output_json , protocol_json);

          json_delete(protocol->message);
          protocol->message = nullptr;

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

  for (size_t i = 0 ; i<matches; i++){
    json_delete(json_first_child(output_json));
  }
  json_delete(output_json);

  return result;
}

/* Decode from pilight string. Must be free() after use */
char* PiCode::decodeString(const char* pilight_string){

  char*    result      = nullptr;
  uint32_t pulses[255] = {0};
  int      n_pulses    =  0;

  if (pilight_string != nullptr){
     n_pulses = stringToPulseTrain(pilight_string, pulses, 255); 
     if (n_pulses > 0){
        result = decodePulseTrain(pulses, (uint8_t)n_pulses);
        if (result!=nullptr){
            if (strlen(result) <  4){ // emply json []
              free(result);
              result=nullptr;
            }
        } 
     }
  }
  return result;
}

/* Encode to pilight string. Must be free() after use */
char* PiCode::encodeToString(const char* protocol_name, const char* json_data, uint8_t repeats){

  char*       result      = nullptr;  
  protocol_t* protocol    = nullptr;
  uint32_t    pulses[255] = {0};
  int         n_pulses    =  0;

  if (protocol_name != nullptr  &&  json_data != nullptr ) {
    protocol  = findProtocol(protocol_name);
    if (protocol != nullptr){
      if (protocol->createCode != nullptr){
        n_pulses = encodeToPulseTrain(pulses, protocol, json_data);
        if (n_pulses > 0){
            result = pulseTrainToString(pulses,(unsigned int)n_pulses, repeats);
        }
      }
    }
  }
  return result;
}

/* Encode to pilight string from json. Must be free() after use */
char* PiCode::encodeJson(const char* json, uint8_t repeats){

  char* result = nullptr;

  if (json != nullptr){ 

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
    if (root_json != nullptr){

      /* check for child */
      JsonNode* child_json = json_first_child(root_json);
      if (child_json != nullptr){            
      
        /* check for child key */
        if (child_json->key != nullptr){

          char* protocol_name = child_json->key;
          char* json_data     = json_encode(child_json);

          if (json_data != nullptr){
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
