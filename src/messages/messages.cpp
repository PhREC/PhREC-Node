#include "messages.h"
#include <unordered_map>

static inline std::unordered_map<int, MessageHandler> message_handlers = {
    {MSG_REQ_HARDWARE_ID,Request_Hardware_Id_Handler},
    {MSG_REQ_ASSIGN_NODE_ID,Assign_Node_Id_Handler},
    // {3,  Validate_Node_Ids_Handler},
    // {4,  Randomize_Node_Ids_Handler},
    // {5,  Request_Sensor_Depth_Handler},
    {MSG_REQ_IGNORE_N_MESSAGES, Hardware_Id_Response_Handler},
    // {10, Request_Sensor_Data_Handler},
    // {11, Request_Sensor_Data_Stream_Handler},
    // {20, Request_Motor_Impulse_Handler},

    {MSG_RESP_HARDWARE_ID, Hardware_Id_Response_Handler},

    // {63 - 2, Assign_Node_Id_Response_Handler},
    // {63 - 3, Validate_Node_Ids_Response_Handler},
    // {63 - 4, Randomize_Node_Ids_Response_Handler},
    // {63 - 5, Request_Sensor_Depth_Response_Handler},
    // {63 - 10, Request_Sensor_Data_Response_Handler},
    // {63 - 11, Request_Sensor_Data_Stream_Response_Handler},

};

int ignore_messages_count = 0;
Message Message_Handler(Message msg) {

    if(USE_RS485){ //In CAN mode we dont need to do the CRC check
        // We failed our first CRC check
        if(crc8(msg.body, sizeof(msg.body)) != msg.bytes[3]){
            // We failed our second CRC check
            if(crc8(msg.body, sizeof(msg.body)) != msg.bytes[3] ^ node_id){
                return (Message){.raw = 0};
            }
        }
    }

    //If we got to this point we know the message is for us
    if(ignore_messages_count > 0){
        ignore_messages_count--;
        return (Message){.raw = 0}; 
    }

    MessageHandler handler = message_handlers[msg.id];
    if(handler == 0){ // We recieved a msg without an associated handler
        return (Message){.raw = 0};
    }
    return handler(msg);
}

Message Request_Ignore_N_Messages_Hander(Message msg){
    ignore_messages_count = msg.requestIgnoreNMessages.count;
    return (Message){.raw=0};
}
