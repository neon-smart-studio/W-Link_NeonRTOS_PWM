
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Converts a hex number to char.
char htoa(uint8_t hex)
{
    if(hex>0xf){
        return '0';
    }
    
    if(hex<10){
        return hex+0x30;
    }
    else{
        return hex-9+0x40;
    }
}

int htoa_str(uint32_t hex, char* hex_str, uint32_t len)
{
    if(hex_str==NULL || len==0){return 0;}
    for(uint32_t i = 0; i<len; i++){
        hex_str[i] = htoa((hex>>(((len-1)-i)*4)) & 0x0F);
    }
    return 0;
}
