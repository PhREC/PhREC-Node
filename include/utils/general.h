#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

// If the packing is not perfect we pad the end with 0
uint64_t* pack_64(uint8_t* data, uint32_t size);
uint32_t* pack_32(uint8_t* data, uint32_t size);
uint16_t* pack_16(uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif