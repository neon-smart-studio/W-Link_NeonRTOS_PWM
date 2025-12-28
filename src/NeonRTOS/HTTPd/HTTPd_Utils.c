
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "HTTPd_def.h"
#include "HTTPd_Utils.h"

uint8_t* HTTPd_Get_Host(uint8_t* buf)
{
	uint8_t* host_str_ptr;
	
	host_str_ptr = HTTPd_Get_Header_Value(buf, (uint8_t*)HTTP_Host);
	if (host_str_ptr == NULL)
	{
		return NULL;
	}
	return host_str_ptr;
}

uint8_t* HTTPd_Parse_Status_Code(uint8_t* buf, int status_code)
{
	if (buf == NULL)
	{
		return NULL;
	}
	
	bool error = false;
	uint8_t* buf_ptr = buf;
	const char* status_str_ptr = NULL;
	uint16_t offset;
	
	memcpy(buf_ptr, HTTP_Version_1P1, strlen(HTTP_Version_1P1));
	buf_ptr += strlen(HTTP_Version_1P1);
	offset = sprintf((char*)buf_ptr, " %d ", status_code);
	buf_ptr += offset;
	
	switch (status_code)
	{
	case 100:
		status_str_ptr = HTTP_Status_100;
		break;
	case 101:
		status_str_ptr = HTTP_Status_101;
		break;
	case 102:
		status_str_ptr = HTTP_Status_102;
		break;

	case 200:
		status_str_ptr = HTTP_Status_200;
		break;
	case 201:
		status_str_ptr = HTTP_Status_201;
		break;
	case 202:
		status_str_ptr = HTTP_Status_202;
		break;
	case 203:
		status_str_ptr = HTTP_Status_203;
		break;
	case 204:
		status_str_ptr = HTTP_Status_204;
		break;
	case 205:
		status_str_ptr = HTTP_Status_205;
		break;
	case 206:
		status_str_ptr = HTTP_Status_206;
		break;
	case 207:
		status_str_ptr = HTTP_Status_207;
		break;

	case 300:
		status_str_ptr = HTTP_Status_300;
		break;
	case 301:
		status_str_ptr = HTTP_Status_301;
		break;
	case 302:
		status_str_ptr = HTTP_Status_302;
		break;
	case 303:
		status_str_ptr = HTTP_Status_303;
		break;
	case 304:
		status_str_ptr = HTTP_Status_304;
		break;
	case 305:
		status_str_ptr = HTTP_Status_305;
		break;
	case 306:
		status_str_ptr = HTTP_Status_306;
		break;
	case 307:
		status_str_ptr = HTTP_Status_307;
		break;

	case 400:
		status_str_ptr = HTTP_Status_400;
		break;
	case 401:
		status_str_ptr = HTTP_Status_401;
		break;
	case 402:
		status_str_ptr = HTTP_Status_402;
		break;
	case 403:
		status_str_ptr = HTTP_Status_403;
		break;
	case 404:
		status_str_ptr = HTTP_Status_404;
		break;
	case 405:
		status_str_ptr = HTTP_Status_405;
		break;
	case 406:
		status_str_ptr = HTTP_Status_406;
		break;
	case 407:
		status_str_ptr = HTTP_Status_407;
		break;
	case 408:
		status_str_ptr = HTTP_Status_408;
		break;
	case 409:
		status_str_ptr = HTTP_Status_409;
		break;
	case 410:
		status_str_ptr = HTTP_Status_410;
		break;
	case 411:
		status_str_ptr = HTTP_Status_411;
		break;
	case 412:
		status_str_ptr = HTTP_Status_412;
		break;
	case 413:
		status_str_ptr = HTTP_Status_413;
		break;
	case 414:
		status_str_ptr = HTTP_Status_414;
		break;
	case 415:
		status_str_ptr = HTTP_Status_415;
		break;
	case 416:
		status_str_ptr = HTTP_Status_416;
		break;
	case 417:
		status_str_ptr = HTTP_Status_417;
		break;
	case 418:
		status_str_ptr = HTTP_Status_418;
		break;
	case 421:
		status_str_ptr = HTTP_Status_421;
		break;
	case 422:
		status_str_ptr = HTTP_Status_422;
		break;
	case 423:
		status_str_ptr = HTTP_Status_423;
		break;
	case 424:
		status_str_ptr = HTTP_Status_424;
		break;
	case 425:
		status_str_ptr = HTTP_Status_425;
		break;
	case 426:
		status_str_ptr = HTTP_Status_426;
		break;
	case 449:
		status_str_ptr = HTTP_Status_449;
		break;
	case 451:
		status_str_ptr = HTTP_Status_451;
		break;

	case 500:
		status_str_ptr = HTTP_Status_500;
		break;
	case 501:
		status_str_ptr = HTTP_Status_501;
		break;
	case 502:
		status_str_ptr = HTTP_Status_502;
		break;
	case 503:
		status_str_ptr = HTTP_Status_503;
		break;
	case 504:
		status_str_ptr = HTTP_Status_504;
		break;
	case 505:
		status_str_ptr = HTTP_Status_505;
		break;
	case 506:
		status_str_ptr = HTTP_Status_506;
		break;
	case 507:
		status_str_ptr = HTTP_Status_507;
		break;
	case 509:
		status_str_ptr = HTTP_Status_509;
		break;
	case 510:
		status_str_ptr = HTTP_Status_510;
		break;
	default:
		error = true;
		break;
	}
	
	if (error == false)
	{
		memcpy(buf_ptr, status_str_ptr, strlen(status_str_ptr));
		buf_ptr += strlen(status_str_ptr);
		offset = sprintf((char*)buf_ptr, "\r\n");
		buf_ptr += offset;
		return buf_ptr;
	}
	else
	{
		memset(buf, 0, buf_ptr-buf);
		return buf;
	}
}

uint8_t* HTTPd_Parse_Reguest_Method(uint8_t* buf, HTTPd_Method req_method, uint8_t* req_path)
{
	if (buf == NULL || req_path==NULL)
	{
		return NULL;
	}
	
	uint8_t* buf_ptr = buf;
	uint16_t offset;
	
	switch (req_method)
	{
	case HTTPd_Method_POST:
		memcpy(buf_ptr, HTTP_Method_POST, strlen(HTTP_Method_POST));
		buf_ptr += strlen(HTTP_Method_POST);
		break;
	case HTTPd_Method_GET:
		memcpy(buf_ptr, HTTP_Method_GET, strlen(HTTP_Method_GET));
		buf_ptr += strlen(HTTP_Method_GET);
		break;
	case HTTPd_Method_PUT:
		memcpy(buf_ptr, HTTP_Method_PUT, strlen(HTTP_Method_PUT));
		buf_ptr += strlen(HTTP_Method_PUT);
		break;
	case HTTPd_Method_DELETE:
		memcpy(buf_ptr, HTTP_Method_DELETE, strlen(HTTP_Method_DELETE));
		buf_ptr += strlen(HTTP_Method_DELETE);
		break;
	default:
		return NULL;
	}
	offset = sprintf((char*)buf_ptr, " %s ", (char*)req_path);
	buf_ptr += offset;
	
	memcpy(buf_ptr, HTTP_Version_1P1, strlen(HTTP_Version_1P1));
	buf_ptr += strlen(HTTP_Version_1P1);
	
	memcpy(buf_ptr, "\r\n", strlen("\r\n"));
	buf_ptr += strlen("\r\n");
	
	return buf_ptr;
}

uint8_t* HTTPd_Parse_Headers(uint8_t* buf, const char* header_str, const char* header_val)
{
	memcpy(buf, header_str, strlen(header_str));
	buf += strlen(header_str);
	uint8_t offset;
	offset = sprintf((char*)buf, ": ");
	buf += offset;
	memcpy(buf, header_val, strlen(header_val));
	buf += strlen(header_val);
	offset = sprintf((char*)buf, "\r\n");
	buf += offset;
	return buf;
}

uint8_t* HTTPd_Parse_End_Of_Headers(uint8_t* buf)
{
	uint8_t offset;
	if (strncmp((char*)buf - 2, "\r\n", 2) != 0)
	{
		offset = sprintf((char*)buf, "\r\n\r\n");
		buf += offset;
	}
	else
	{
		offset = sprintf((char*)buf, "\r\n");
		buf += offset;
	}
	return buf;
}

uint8_t* HTTPd_Get_Header_Value(uint8_t* buf, uint8_t* header_str)
{
	uint8_t* buf_ptr = buf;
	do
	{
		buf_ptr = (uint8_t*)strstr((char*)buf_ptr, (char*)header_str);
		if (buf_ptr == NULL)
		{
			return NULL;
		}
                
                if(strncmp((const char*)(buf_ptr-2), "\r\n", 2)!=0 && *(buf_ptr-1)!=' ' && *(buf_ptr-1)!=':' && *(buf_ptr-1)!='\0')
                {
                        buf_ptr = (uint8_t*)strstr((char*)buf_ptr, "\r\n");
                        if(buf_ptr==NULL){break;}
                        continue;
                }
                
		buf_ptr += strlen((char*)header_str);
	
		while (*buf_ptr ==  ' ') {buf_ptr++;}
                
		if (*buf_ptr++ != ':')
		{
			continue;
		}
	
		while (*buf_ptr ==  ' ') {buf_ptr++;}
		
		break;
	} while (1);
	
	return buf_ptr;
}

HTTPd_Method HTTPd_Get_Header_Method(uint8_t* buf)
{
	uint8_t* first_line_end;
	first_line_end = (uint8_t*)strstr((char*)buf, "\r\n");
	if (first_line_end == NULL)
	{
		return HTTPd_Method_UNKNOWN;
	}
	
	uint8_t* method_str_ptr;
	
	method_str_ptr = (uint8_t*)strstr((char*)buf, HTTP_Method_POST);
	if (method_str_ptr != NULL && method_str_ptr < first_line_end)
	{
		return HTTPd_Method_POST;
	}
	
	method_str_ptr = (uint8_t*)strstr((char*)buf, HTTP_Method_GET);
	if (method_str_ptr != NULL && method_str_ptr < first_line_end)
	{
		return HTTPd_Method_GET;
	}
	
	method_str_ptr = (uint8_t*)strstr((char*)buf, HTTP_Method_PUT);
	if (method_str_ptr != NULL && method_str_ptr < first_line_end)
	{
		return HTTPd_Method_PUT;
	}
	
	method_str_ptr = (uint8_t*)strstr((char*)buf, HTTP_Method_DELETE);
	if (method_str_ptr != NULL && method_str_ptr < first_line_end)
	{
		return HTTPd_Method_DELETE;
	}
	
	return HTTPd_Method_UNKNOWN;
}

uint8_t* HTTPd_Get_Header_Path(uint8_t* buf)
{
	uint8_t* first_line_end;
	first_line_end = (uint8_t*)strstr((char*)buf, "\r\n");
	if (first_line_end == NULL)
	{
		return NULL;
	}
	
	uint8_t* path_str_ptr;
	
	path_str_ptr = (uint8_t*)strstr((char*)buf, HTTP_Version_1P0);
	if (path_str_ptr != NULL && path_str_ptr < first_line_end)
	{
		path_str_ptr--;
		while (*path_str_ptr ==  ' ') {path_str_ptr--;}
		while (*path_str_ptr !=  ' ') {path_str_ptr--;}
		path_str_ptr++;
		return path_str_ptr;
	}
	
	path_str_ptr = (uint8_t*)strstr((char*)buf, HTTP_Version_1P1);
	if (path_str_ptr != NULL && path_str_ptr < first_line_end)
	{
		path_str_ptr--;
		while (*path_str_ptr ==  ' ') {path_str_ptr--;}
		while (*path_str_ptr !=  ' ') {path_str_ptr--;}
		path_str_ptr++;
		return path_str_ptr;
	}
	
	return NULL;
}
