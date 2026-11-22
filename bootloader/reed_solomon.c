#include "utils/reed_solomon.h"

//Used the following articles to get this implementation
// https://en.wikiversity.org/wiki/Reed%E2%80%93Solomon_codes_for_coders
// https://www.cs.cmu.edu/~guyb/realworld/reedsolomon/reed_solomon_codes.html
// https://www.nayuki.io/page/reed-solomon-error-correcting-code-decoder#peterson-gorenstein-zierler-decoder
// https://berthub.eu/articles/posts/reed-solomon-for-programmers/

int fix_gaps(uint8_t* data, uint8_t* missing, uint8_t size_in, uint8_t size_missing){
    for (int i = 0; i < size_missing; i++) {
        int index = missing[i];
        __builtin_memmove(&data[index + 1], &data[index], size_in + i - index);
        data[index] = 0;
    }

    return 0;
}

const FRAME_SIZE = 255;
const DATA_SIZE = 223;
const PARITY_SIZE = 32;

//Function only valid when size = 255
uint8_t* reed_solomon(uint8_t data, uint32_t size){


    //Syndrome calc

    uint8_t* synd[PARITY_SIZE];
    for(int i = 0; i < PARITY_SIZE; i++){
        //synd[i] = gf_poly_eval(msg, gf_pow(2,i));
        //What does this mean!!
    }

    //When we have a corruption syndromes will be non zero

    //Error polynomial

    //Need to find e_pos or the positions of the errors

    //Error locations

    //Error magnitudes

    //Error corrector
}