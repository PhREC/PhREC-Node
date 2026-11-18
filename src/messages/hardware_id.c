#include "messages.h"
#include "utils/short_hwid.h"

Message Request_Hardware_Id_Handler(Message msg){

    uint16_t shwid = ShortHWID();
    Message response = {0};
    response.hardwareIdResponse.id = MSG_RESP_HARDWARE_ID;
    response.hardwareIdResponse.hw_id = shwid;
    response.hardwareIdResponse.crc8 = crc8(response.body, sizeof(response.body));
    return response;
}

//Responses are not implemented on PHRec Node atm so echo msg
Message Hardware_Id_Response_Handler(Message msg){ return msg; }
