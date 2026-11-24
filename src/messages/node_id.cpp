#include "messages.h"
#include "utils/crc.h"
#include <stdlib.h>
#include <vector>
#include <numeric>

//This message only works in RS485 mode
Message Assign_Node_Id_Handler(Message msg){
    //Assign Node ID is a fairly special message
    //Its the only message that ignores the CRC check (This happens in usart)
    Message nmsg = {.id=MSG_RESP_ASSIGN_NODE_ID};
    
    uint8_t checksum = crc2(&msg.bytes[3], sizeof(msg.bytes[3]));
    if(checksum != msg.assignNodeId.checksum){
        //We failed our checksum, we can't verify that this is valid
        nmsg.assignedNodeIdResponse.assigned_node_id = node_id;
        nmsg.assignedNodeIdResponse.success = 0;
        return nmsg;
    }


    uint8_t crc = crc8(msg.body, sizeof(msg.body));
    node_id = crc ^ msg.bytes[3];
    nmsg.assignedNodeIdResponse.assigned_node_id = node_id;
    nmsg.assignedNodeIdResponse.success = ~0;
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

Message Randomize_Node_Ids_Handler(Message msg){ 
    
//Nothing in the message is usefull to us
    Message nmsg = {.id=MSG_RESP_RANDOMIZE_NODE_IDS};
    //TODO :: DEFINE AVAILABLE NODE IDS
    std::vector<uint8_t> available_ids(255);

    if(USE_RS485){ //msg.bytes[3] is only defined on RS485
        //We need to check if the message was a broadcast message
        //Becuase if it was it invalidates ALL node ids
        //This is almost guarnteed to have a collision due to the birthday paradox
        //So please PLEASE DO NOT USE THIS IN BROADCAST 
        if(crc8(msg.body, sizeof(msg.body)) == msg.bytes[3]){
            available_ids.resize(255);
            std::iota(available_ids.begin(), available_ids.end(), 1);
            //Sets the available IDs to all values
        }
    }

    //C rand should be fine for our purposes
    available_ids[rand() % available_ids.size()];
    //TODO :: WE STILL NEED TO SEED THIS FIRST
    // Make sure our seed is UNIQUE
    // We can't use our 16bit HWID
    nmsg.validateNodeIdResponse.random_val = rand();
    nmsg.validateNodeIdResponse.node_id = node_id;

    return nmsg;
    
}

//Responses are not implemented on PHRec Node atm so return invalid msg
Message Randomize_Node_Ids_Response_Handler(Message msg){ 
    return (Message){.raw=0}; 
}