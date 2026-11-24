#include "messages.h"
#include "utils/short_hwid.h"
#include <stdlib.h>
//While using arduino
#include <Arduino.h>

static uint8_t g_config = 0;
static uint16_t hw_ids[255] = {};
static uint16_t hw_id;

void Hardware_Id_Init(void) {
    hw_id = hw_ids[0] = ShortHWID();
}

int compare(const void *a, const void *b) {
    return (*(const uint16_t *)b - *(const uint16_t *)a);
}

Message Request_Hardware_Id_Handler(Message msg){

    uint16_t shwid = ShortHWID();
    Message response = {0};
    g_config = msg.requestHardwareId.config;
    response.hardwareIdResponse.id = MSG_RESP_HARDWARE_ID;
    response.hardwareIdResponse.hw_id = shwid;
    response.bytes[3] = crc8(response.body, sizeof(response.body));
    return response;
}

// On the node it is usefull to listen to other node's
Message Hardware_Id_Response_Handler(Message msg){ 
    //Keep a sorted list of all hw_ids

    //Alternatively we can use a for loop and a counter to fugure out our ID

    //Speed doesn't matter here tbh
    qsort(hw_ids, 255, sizeof(uint16_t), compare);
    // Our node ID is simply the ordered index of our SHWID in the array
    node_id = (uint16_t*)bsearch(&hw_id, hw_ids, 255, sizeof(uint16_t), compare) - hw_ids;
    
    //We dont wanna respond to this message, just return an invalid message
    return (Message){.raw = 0};
}

