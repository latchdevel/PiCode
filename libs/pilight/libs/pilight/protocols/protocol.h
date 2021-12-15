/*
  ESPiLight - pilight 433.92 MHz protocols library for Arduino
  Copyright (c) 2016 Puuu.  All right reserved.

  Project home: https://github.com/puuu/espilight/
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with library. If not, see <http://www.gnu.org/licenses/>

  Copyright (c) 2021 Jorge Rivera. All right reserved. LGPL-3.0.
  Feb 2021:
  - Add options.h and struct options_t *options;
  - Add typedef struct protocol_devices_t and struct protocol_devices_t *devices;
  - Delete char *old_content; ESPiLight special, used to compare repeated messages
*/

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>
#include "../core/json.h"
#include "../core/options.h"

// from ../config/hardware.h
typedef enum {
  HWINTERNAL = -1,
  NONE = 0,
  RF433,
  RF868,
  RFIR,
  ZWAVE,
  SENSOR,
  HWRELAY,
  API
} hwtype_t;

typedef enum {
  FIRMWARE = -2,
  PROCESS = -1,
  RAW = 0,
  SWITCH,
  DIMMER,
  WEATHER,
  RELAY,
  SCREEN,
  CONTACT,
  PENDINGSW,
  DATETIME,
  XBMC,
  LIRC,
  WEBCAM,
  MOTION,
  DUSK,
  PING,
  LABEL,
  ALARM
} devtype_t;

typedef struct protocol_devices_t {
	char *id;
	char *desc;
	struct protocol_devices_t *next;
} protocol_devices_t;

typedef struct protocol_t {
  char *id;
  uint16_t rawlen;
  uint16_t minrawlen;
  uint16_t maxrawlen;
  uint32_t mingaplen;
  uint32_t maxgaplen;
  uint8_t txrpt;
  uint8_t rxrpt;
  //short multipleId;
  //short config;
  //short masterOnly;
  struct options_t *options; 
  struct JsonNode *message;

  uint8_t repeats;
  unsigned long first;
  unsigned long second;

  uint32_t *raw;

  hwtype_t hwtype;
  devtype_t devtype;
  struct protocol_devices_t *devices;
  //struct protocol_threads_t *threads;

  union {
    void (*parseCode)(void);
    void (*parseCommand)(struct JsonNode *code);
  };
  
  int (*validate)(void);
  int (*createCode)(JsonNode *code);
  int (*checkValues)(JsonNode *code);
  //struct threadqueue_t *(*initDev)(JsonNode *device);
  void (*printHelp)(void);
  void (*gc)(void);
  //void (*threadGC)(void);
} protocol_t;

typedef struct protocols_t {
  struct protocol_t *listener;
  char *name;
  struct protocols_t *next;
} protocols_t;

extern struct protocols_t *pilight_protocols;

void protocol_init(void);
void protocol_set_id(protocol_t *proto, char *id);
void protocol_register(protocol_t **proto);

void protocol_device_add(protocol_t *proto, const char *id, const char *desc);
int protocol_device_exists(protocol_t *proto, const char *id);

#endif
