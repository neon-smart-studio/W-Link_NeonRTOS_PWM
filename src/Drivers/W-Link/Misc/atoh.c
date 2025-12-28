
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Converts a hex number to char.
uint8_t atoh(char ascii_char)
{
    if(ascii_char>=0x30 && ascii_char<=0x39){
        return ascii_char-0x30;
    }
    else if(ascii_char>=0x41 && ascii_char<=0x46){
        return (ascii_char-0x40)+9;
    }
    else if(ascii_char>=0x61 && ascii_char<=0x66){
        return (ascii_char-0x60)+9;
    }
    else{
        return 0;
    }
}

uint32_t atoh_str(const char* ascii_str, uint32_t ascii_str_len)
{
    if(ascii_str==NULL || ascii_str_len==0){return 0;}
    uint32_t returnVal = 0;
    for(uint32_t i = 0; i<ascii_str_len; i++){
        returnVal |= (atoh(ascii_str[i])<<(((ascii_str_len-1)-i)*8));
    }
    return returnVal;
}
