#include "messages.h"
#include "utils/crc.h"
#include <stdlib.h>

Message Assign_Node_Id_Handler(Message msg){
    //Assign Node ID is a fairly special message
    //Its the only message that ignores the CRC check (This happens in usart)
    
    uint8_t crc = crc8(msg.body, sizeof(msg.body));
    node_id = crc ^ msg.bytes[3];
    Message nmsg = {.id=MSG_RESP_ASSIGN_NODE_ID};
    nmsg.assignedNodeIdResponse.assigned_node_id = node_id;
    lock_node_id = 1;
    return nmsg;
}

//Responses are not implemented on PHRec Node atm so return invalid msg
Message Assign_Node_Id_Response_Handler(Message msg){ return (Message){.raw=0}; }


Message Validate_Node_Ids_Handler(Message msg){
    return msg;
}

//Responses are not implemented on PHRec Node atm so echo msg
Message Validate_Node_Ids_Response_Handler(Message msg){
    //Nothing in the message is usefull to us
    Message nmsg = {.id=MSG_RESP_VALIDATE_NODE_IDS};
    
    //TODO :: WE STILL NEED TO SEED THIS FIRST
    // Make sure our seed is UNIQUE
    // We can't use our 16bit HWID
    nmsg.validateNodeIdResponse.random_val = rand();
    nmsg.validateNodeIdResponse.node_id = node_id;

    return nmsg;
}

Message Randomize_Node_Ids_Handler(Message msg){ return (Message){.raw=0}; }

//Responses are not implemented on PHRec Node atm so return invalid msg
Message Randomize_Node_Ids_Response_Handler(Message msg){ return (Message){.raw=0}; }