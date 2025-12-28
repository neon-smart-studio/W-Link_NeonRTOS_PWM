
#ifndef HTTPD_UTILS_H
#define HTTPD_UTILS_H

#include <stdint.h>

#include "HTTPd_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

uint8_t* HTTPd_Get_Host(uint8_t* buf);
uint8_t* HTTPd_Parse_Status_Code(uint8_t* buf, int status_code);
uint8_t* HTTPd_Parse_Reguest_Method(uint8_t* buf, HTTPd_Method req_method, uint8_t* req_path);
uint8_t* HTTPd_Parse_Headers(uint8_t* buf, const char* header_str, const char* header_val);
uint8_t* HTTPd_Parse_End_Of_Headers(uint8_t* buf);
uint8_t* HTTPd_Get_Header_Value(uint8_t* buf, uint8_t* header_str);
HTTPd_Method HTTPd_Get_Header_Method(uint8_t* buf);
uint8_t* HTTPd_Get_Header_Path(uint8_t* buf);

#ifdef __cplusplus
}
#endif

#endif //HTTPD_UTILS_H