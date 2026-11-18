#include "crc.h"

// Only used once in bootloader and the main application
// So it is fine to just inline this function

inline uint16_t ShortHWID(){
    uint32_t uid1 = HAL_GetUIDw0();
    uint32_t uid2 = HAL_GetUIDw1();
    uint32_t uid3 = HAL_GetUIDw2();

    uint8_t uid_bytes[12];
    //Convert the 3 uint32_t values to bytes
    memcpy(uid_bytes, &uid1, 4);
    memcpy(uid_bytes + 4, &uid2, 4);
    memcpy(uid_bytes + 8, &uid3, 4);

    return crc16(uid_bytes, 12);
}

inline uint8_t ShortHWID(){
    uint32_t uid1 = HAL_GetUIDw0();
    uint32_t uid2 = HAL_GetUIDw1();
    uint32_t uid3 = HAL_GetUIDw2();

    uint8_t uid_bytes[12];
    //Convert the 3 uint32_t values to bytes
    memcpy(uid_bytes, &uid1, 4);
    memcpy(uid_bytes + 4, &uid2, 4);
    memcpy(uid_bytes + 8, &uid3, 4);

    return crc8(uid_bytes, 12);
}