
#ifndef HTTPD_H
#define HTTPD_H

#include "cJSON/cJSON.h"

#include "NeonRTOS.h"

#include "HTTPd_def.h"

#ifdef	__cplusplus
extern "C" {
#endif

//#define HTTPD_USE_SSL

#define HTTPD_RECV_BLOCK_INTERVAL            100

#ifdef HTTPD_USE_SSL
#define HTTP_PORT                            443
#else
#define HTTP_PORT                            80
#endif

#define HTTPD_TIMER_ID_OFFSET                1

#define HTTP_TIMEOUT_STR                     "5"
#define HTTP_TIMEOUT_S                       5
#define HTTP_TIMEOUT_TIMER_MS                5000
#define WEBSOCKET_TIMEOUT_TIMER_MS           300000

#define WS_SERVER_CMD_BUF_SIZE               1024

#define HTTPD_MAX_CLIENTS	             	 20
#define HTTP_WS_SERVER_DAT_BUF_SIZE          1*1024

#define HTTPD_CGI_MORE 0
#define HTTPD_CGI_DONE 1
#define HTTPD_CGI_NOTFOUND 2
#define HTTPD_CGI_AUTHENTICATED 3

#define WEBSOCK_FLAG_NONE 0
#define WEBSOCK_FLAG_CONT (1<<0) //Set if the data is not the final data in the message; more follows
#define WEBSOCK_FLAG_BIN (1<<1) //Set if the data is binary instead of text
#define WEBSOCK_FLAG_PONG (1<<2)

typedef struct HTTPd_WebSocked_Client_Connection HTTPd_WebSocked_Client_Connection;

typedef int(* cgiSendCallback)(HTTPd_WebSocked_Client_Connection *connData);

struct HTTPd_WSFrame {
	uint8_t flags;
	uint8_t opcode;
	uint8_t isMasked;
	uint64_t payloadLength;
	uint32_t maskingKey;
};

typedef struct HTTPd_WSFrame HTTPd_WSFrame;

typedef void(* HTTPd_WsServerOnMessageCB)(struct HTTPd_WebSocked_Client_Connection *, const HTTPd_WSFrame *, uint8_t*);

typedef struct {
	const char *url;
	cgiSendCallback cgiCb;
	const void *cgiArg;
} HttpdBuiltInUrl;

typedef struct HttpdDynamicUrlItem_t{
	const char *url;
	cgiSendCallback cgiCb;
	void *cgiArg;
	struct HttpdDynamicUrlItem_t* next;
} HttpdDynamicUrlItem;

int HTTPd_Init();

#ifdef HTTPD_USE_SSL
ssl_context* HTTPd_Get_SSL_Context(HTTPd_WebSocked_Client_Connection *connData);
ssl_context* HTTPd_Get_SSL_Context_By_Socket_ID(int socket_ID);
#else //HTTPD_USE_SSL
int HTTPd_Get_Socket_ID(HTTPd_WebSocked_Client_Connection *connData);
#endif //HTTPD_USE_SSL

uint8_t* HTTPd_Parse_Status_Code(uint8_t* buf, int status_code);
uint8_t* HTTPd_Parse_Reguest_Method(uint8_t* buf, HTTPd_Method req_method, uint8_t* req_path);
uint8_t* HTTPd_Parse_Headers(uint8_t* buf, const char* header_str, const char* header_val);
uint8_t* HTTPd_Parse_End_Of_Headers(uint8_t* buf);
HTTPd_Method HTTPd_Get_Header_Method(uint8_t* buf);
uint8_t* HTTPd_Get_Header_Path(uint8_t* buf);
uint8_t* HTTPd_Get_Header_Value(uint8_t* buf, uint8_t* header_str);

int HTTPd_CGI_PathStartsWith(const char *url,
                          const char *prefix,
                          const char **rest);      // 可為 NULL
int HTTPd_CGI_ParseUint(const char *s);
int HTTPd_Register_CGI_URL_Callback(const char *url, cgiSendCallback cgiCb, void* cgiArg);
int HTTPd_UnRegister_CGI_URL_Callback(const char *url);
HTTPd_Method HTTPd_Get_CGI_Request_Type(HTTPd_WebSocked_Client_Connection *connData);
const char* HTTPd_Get_CGI_Request_URL(HTTPd_WebSocked_Client_Connection *connData);
const uint8_t* HTTPd_Get_CGI_Request_Data(HTTPd_WebSocked_Client_Connection *connData);
int HTTPd_Send_CGI_Response(HTTPd_WebSocked_Client_Connection *connData, uint16_t status_code, const char* rsp_type, uint8_t* rsp_data, uint64_t rsp_len);
int HTTPd_Send_CGI_JSON_Response(HTTPd_WebSocked_Client_Connection *connData, uint16_t status_code, cJSON* rsp_dat, bool need_delete_json);

void WebsocketServer_RegisterMsgCallback(HTTPd_WsServerOnMessageCB cb);
void WebsocketServer_SendMessage(struct HTTPd_WebSocked_Client_Connection* ws_client, const char *payload, uint64_t payloadLength, uint8_t flags);
void WebsocketServer_SendJSONMessage(struct HTTPd_WebSocked_Client_Connection* ws_client, cJSON* data, bool need_delete_json);
void WebsocketServer_SendBrocastMessage(const char *payload, uint64_t payloadLength, uint8_t flags);
void WebsocketServer_SendBrocastJSONMessage(cJSON* data, bool need_delete_json);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //HTTPD_H