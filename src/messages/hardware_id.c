#include "messages.h"
#include "utils/short_hwid.h"
#include <stdlib.h>

static uint8_t g_config = 0;
static uint16_t hw_ids[255] = {};
static uint16_t hw_id;

void Hardware_Id_Init(void) {
    hw_id = hw_ids[0] = ShortHWID();
}

int compare(const void *a, const void *b) {
    return (*(int *)b - *(int *)a);
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
    //Speed doesn't matter here tbh
    qsort(hw_ids, 255, sizeof(uint16_t), compare);
    // Our node ID is simply the ordered index of our SHWID in the array
    node_id = (int)bsearch(hw_id, hw_ids, 255, sizeof(uint16_t), compare) - *hw_ids; 
    //return msg;
}

