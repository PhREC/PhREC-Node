
#include <stdint.h>
#include "utils/crc.h"

#ifdef __cplusplus
#include <set>
#endif

#define MSG_RESERVED_EMPTY 0
#define MSG_REQ_HARDWARE_ID 1
#define MSG_REQ_ASSIGN_NODE_ID 2
#define MSG_REQ_VALIDATE_NODE_IDS 3
#define MSG_REQ_RANDOMIZE_NODE_IDS 4
#define MSG_REQ_SENSOR_DEPTH 5
#define MSG_REQ_IGNORE_N_MESSAGES 6
#define MSG_REQ_SENSOR_DATA 10
#define MSG_REQ_SENSOR_DATA_STREAM 11
#define MSG_REQ_MOTOR_IMPULSE 20
#define MSG_REQ_FIRMWARE_BLIND_UPLOAD 30

#define MSG_RESP_RESERVED_EMPTY 63
#define MSG_RESP_HARDWARE_ID 62
#define MSG_RESP_ASSIGN_NODE_ID 61
#define MSG_RESP_VALIDATE_NODE_IDS 60
#define MSG_RESP_RANDOMIZE_NODE_IDS 59
#define MSG_RESP_SENSOR_DEPTH 54
#define MSG_RESP_SENSOR_DATA 53
#define MSG_RESP_SENSOR_STREAM_HEADER 52


#ifdef __cplusplus
static std::set<uint16_t> hw_ids = {};
static inline bool lock_node_id = true;
#endif

static uint16_t hw_id;
static uint8_t node_id = 0;

#ifndef BAUD_RATE
#define BAUD_RATE 580000
#endif
#define FRAME_SIZE 10

#if defined(USE_CAN) && (USE_CAN + 0 != 0)
    #undef USE_RS485
    #undef USE_CAN
    #define USE_CAN 1
    #define USE_RS485 0
#elif defined(USE_RS485) && (USE_RS485 + 0 != 0)
    #undef USE_RS485
    #undef USE_CAN
    #define USE_RS485 1
    #define USE_CAN 0
#else
    #define USE_RS485 1
    #define USE_CAN 0
#endif


#ifdef __cplusplus
extern "C" {
#endif

//Request Messages
typedef struct {
    uint32_t id : 6;
    uint32_t config : 2;
    uint32_t undefined : 16;
} RequestHardwareId;

typedef struct {
    uint32_t id : 6;
    uint32_t hw_id : 16;
    uint32_t checksum : 2;
} AssignNodeId;

typedef struct {
    uint32_t id : 6;
    uint32_t undefined : 18;
} ValidateNodeIds;

typedef struct {
    uint32_t id : 6;
    uint32_t config : 2;
    uint32_t undefined : 16;
} RandomizeNodeIds;

typedef struct {
    uint32_t id : 6;
    uint32_t undefined : 18;
} RequestSensorDepth;

typedef struct {
    uint32_t id : 6;
    uint32_t count : 18;
} RequestIgnoreNMessages;

typedef struct {
    uint32_t id : 6;
    uint32_t sensor_depth : 8;
    uint32_t undefined : 10;
} RequestSensorData;

typedef struct {
    uint32_t id : 6;
    uint32_t sensor_depth : 8;
    uint32_t undefined : 10;
} RequestSensorDataStream;

typedef struct {
    uint32_t id : 6;
    uint32_t motor_strength : 18;
} RequestMotorImpulse;

typedef struct {
    uint32_t id : 6;
    uint32_t undefined : 18;
} RequestFirmwareBlindUpload;


//Response Messages

typedef struct {
    uint32_t id : 6;
    uint32_t hw_id : 16;
    uint32_t undefined : 2;
} HardwareIdResponse;

typedef struct {
    uint32_t id : 6;
    uint32_t assigned_node_id : 8;
    uint32_t success : 10;
} AssignedNodeIdResponse;


typedef struct {
    uint32_t id : 6;
    uint32_t random_val : 10;
    uint32_t node_id : 8;
} ValidateNodeIdResponse;

typedef struct {
    uint32_t id : 6;
    uint32_t random_val : 10;
    uint32_t node_id : 8;
} RandomizedNodeIdResponse;

typedef struct {
    uint32_t id : 6;
    uint32_t sensor_depth : 8;
    uint32_t undefined : 10;
} SensorDepthResponse;

typedef struct {
    uint32_t id : 6;
    uint32_t sensor_data : 18;
} SensorDataResponse;

typedef struct {
    uint32_t id : 6;
    uint32_t sensor_count : 4;
    uint32_t undefined : 14;
} SensorStreamHeader;

typedef struct {
    uint32_t sensor_id : 4;
    uint32_t sensor_data : 20;
} SensorStreamData;

//Basic Message Structure
//We use this for packing and unpacking messages
typedef union {
    uint32_t raw;

    uint32_t id : 6;

    //First 24 bits of our message, useful for crc-8 calculation
    uint8_t body[3];

    uint8_t bytes[4];
    //crc8 is always bytes[3]

    //Request Messages
    RequestHardwareId requestHardwareId;
    
    AssignNodeId assignNodeId;
    ValidateNodeIds validateNodeIds;
    RandomizeNodeIds randomizeNodeIds;
    RequestSensorDepth requestSensorDepth;
    RequestIgnoreNMessages requestIgnoreNMessages;
    RequestSensorData requestSensorData;
    RequestSensorDataStream requestSensorDataStream;

    RequestMotorImpulse requestMotorImpulse;
    RequestFirmwareBlindUpload requestFirmwareBlindUpload;

    //Response Messages
    HardwareIdResponse hardwareIdResponse;
    AssignedNodeIdResponse assignedNodeIdResponse;
    SensorDepthResponse sensorDepthResponse;
    SensorDataResponse sensorDataResponse;
    ValidateNodeIdResponse validateNodeIdResponse;
    RandomizedNodeIdResponse randomizedNodeIdResponse;
    SensorStreamHeader sensorStreamHeader;
    SensorStreamData sensorStreamData;
} Message;

typedef Message (*MessageHandler)(Message msg);

Message Message_Handler(Message msg);

Message Request_Hardware_Id_Handler(Message msg);
Message Assign_Node_Id_Handler(Message msg);
Message Validate_Node_Ids_Handler(Message msg);
Message Randomize_Node_Ids_Handler(Message msg);
Message Request_Sensor_Depth_Handler(Message msg);
Message Request_Ignore_N_Messages_Hander(Message msg);
Message Request_Sensor_Data_Handler(Message msg);
Message Request_Sensor_Data_Stream_Header_Handler(Message msg);
Message Request_Motor_Impulse_Handler(Message msg);

Message Hardware_Id_Response_Handler(Message msg);
Message Assign_Node_Id_Response_Handler(Message msg);
Message Validate_Node_Ids_Response_Handler(Message msg);
Message Randomize_Node_Ids_Response_Handler(Message msg);
Message Response_Sensor_Depth_Handler(Message msg);
Message Response_Sensor_Data_Handler(Message msg);
Message Response_Sensor_Data_Stream_Header_Handler(Message msg);
Message Response_Sensor_Data_Stream_Data_Handler(Message msg);
Message Response_Motor_Impulse_Handler(Message msg);


#ifdef __cplusplus
}
#endif