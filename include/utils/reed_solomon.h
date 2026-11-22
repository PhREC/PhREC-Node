#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

int fix_gaps(uint8_t* data, uint8_t* missing, uint8_t size_in, uint8_t size_missing);
int reed_solomon_decode(uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif