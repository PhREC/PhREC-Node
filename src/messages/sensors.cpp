#include "messages.h"

Message Request_Sensor_Depth_Handler(Message msg){
    
    return (Message){.raw = 0};
}


Message Response_Sensor_Data_Handler(Message msg){
    // We dont really need to listen to this response on the Node
    // as the node doesnt *really* need to know how many sensors the
    // other nodes have, but it can be usefull if we decide to use it
    // esp on the Request_Sensor_data_handler
    return (Message){.raw = 0};
}

Message Request_Sensor_Data_Handler(Message msg){
    
    return (Message){.raw = 0};
}

Message Request_Sensor_Data_Stream_Header_Handler(Message msg){
    
    return (Message){.raw = 0};
}

Message Response_Sensor_Data_Stream_Header_Handler(Message msg){
    
    // TODO :: IMPLEMENT THE DATASTREAM
    // SPECIAL CASE :: OTHER NODE'S DATASTREAM
    // THE SENSOR DATA STREAM RESPONSE HEADER WILL HAVE A LENGTH
    // WE NEED TO LISTEN TO THE HEADERS AND APPEND THAT TO OUR WAIT TIME

    return (Message){.raw = 0};
}

Message Response_Sensor_Data_Stream_Data_Handler(Message msg){
    return (Message){.raw = 0};
}
