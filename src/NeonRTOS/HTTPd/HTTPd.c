
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "NeonRTOS.h"
#include "NeonRtFs.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#if defined(POLARSSL_SSL_TLS_C)

#if !defined(POLARSSL_CONFIG_FILE)
#include "polarssl/config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include "polarssl/debug.h"
#include "polarssl/config.h"
#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include "polarssl/error.h"
#include "polarssl/memory.h"
#include "polarssl/certs.h"

#if defined(POLARSSL_X509_CRT_PARSE_C) && \
    defined(POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE)
#include "polarssl/oid.h"
#endif

#endif

#include "HTTPd_def.h"
#include "HTTPd_Utils.h"
#include "HTTPd.h"

#include "sha1.h"
#include "base64.h"

#ifdef HTTPD_USE_SSL
#include "SSL_Random/SSL_Random.h"
#endif

#define HTTP_TASK_PRIORITY                   3

#define HTTPD_CMD_BUF_SIZE                   1024

#define HTTP_STACK_SIZE                      2*1024
#define HTTPD_DAT_BUF_SIZE                   2*1024

#define HTTPDVER "1.1"

#define max(a,b) ((a)>(b)?(a):(b))  /**< Find the maximum of 2 numbers. */

typedef struct HTTPd_WebSocked_Client_Connection
{
	int socket_id;
#ifdef HTTPD_USE_SSL
	ssl_context client_ssl_ctx;
#endif
        NeonRTOS_TaskHandle clientTaskHandle;
	struct sockaddr client_socket_addr;
        HTTPd_Connection_Mode connetion_mode;
        uint32_t connection_keep_alive_time;
	NeonRTOS_TimerHandle connection_timeout_timer;
	bool connection_destruct_flag;
	bool websocket_client;
	bool websocket_auth;
	uint8_t* client_key;
	HTTPd_Method requestType;
	char *url;
	char *hostName;
        bool isLargeData;
	uint32_t data_len;
        uint8_t* data_buff;
        char accept_encoding[64];
	char *getArgs;
	const void *cgiArg;
	cgiSendCallback cgi;
}HTTPd_WebSocked_Client_Connection;

int HTTP_WebSocket_Server_Socket = -1;

//Connection pool
HTTPd_WebSocked_Client_Connection* HTTPd_WebSocketd_Client_List[HTTPD_MAX_CLIENTS];

static HTTPd_WsServerOnMessageCB wsServerOnMessageCallback = NULL;

#ifdef CONFIG_SUPPORT_OTA
const char    OTA_Update_Page_HTML[]=  "\
<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\r\n\
    <title>Uploade OTA Image</title>\r\n<script type=\"text/javascript\">\r\n\
        var xhr = new XMLHttpRequest();\r\n\
        window.onload=function(e) {\r\n\
            document.getElementById('file-input').addEventListener('change', function(e){\r\n\
                var file = e.target.files[0];\r\n\
                if (!file) {return;}\r\n\
                var formData = new FormData();\r\n\
                formData.append(\"UploadOtaImage\", file);\r\n\
                xhr.open(\"POST\", '/OTA', true);\r\n\
                xhr.onreadystatechange=function() {\r\n\
                        if (xhr.readyState==4) sendInProgress=false;\r\n\
                }\r\n\
                xhr.send(formData);\r\n\
            }, false);\r\n\
        }\r\n\
    </script>\r\n</head>\r\n<body>\r\n<h3>Uploade OTA Image To RTL8710(.bin)</h3>\r\n<input type=\"file\" id=\"file-input\" />\r\n</body>\r\n";
#endif
            
                                
const char    HTTP_API_Debug_Tool_Page_HTML[]=  "\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n<head>\r\n\
    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">\r\n\
    <title>W-Link API Debug tool</title>\r\n<style>\r\n\
        body{font-family: verdana;margin: 20px;}\r\n\
        h1{margin: 0px;font-size: 20px;}\r\n\
        h2{font-size: 16px;margin-top: 20px;color: grey;}\r\n\
        #buttons{clear: left;}\r\n\
        input{width: 100%;}\r\n\
        form{background: #e8e8e8;width: 500px;padding: 20px;-webkit-border-radius: 10px;-moz-border-radius: 10px;}\r\n\
        textarea{padding-top: 10px;width: 100%;font-family: monaco,monospace;font-size: 12px;-webkit-border-radius: 10px;-moz-border-radius: 10px;}\r\n\
    </style>\r\n<script>\r\n\
        function getHTML(command) {\r\n\
            if (window.XMLHttpRequest) {\r\n\
                var http = new XMLHttpRequest();\r\n\
                http.open(command, document.commandform.commandurl.value, true);\r\n\
                http.onreadystatechange = function() {\r\n\
                    if(http.readyState == 4) {\r\n\
                        if(http.status==200) {\r\n\
                            document.commandform.response.value=\"Bad JSON: \"+http.responseText;\r\n\
                            document.commandform.response.value=JSON.stringify(JSON.parse(http.responseText), null, '\t');\r\n\
                        } else {\r\n\
                            document.commandform.response.value=\"Error \"+http.status;\r\n\
                        }\r\n\
                    }\r\n\
                }\r\n\
                http.send(document.commandform.messagebody.value);\r\n\
            }\r\n\
            return false;\r\n\
        }\r\n\
        document.addEventListener(\"DOMContentLoaded\", function(event) {\r\n\
            var btnGrp = document.querySelectorAll(\"button\");\r\n\
            Array.from(btnGrp).forEach(function(btn) {\r\n\
                document.getElementById(btn.id).addEventListener(\"click\", function () {\r\n\
                    getHTML(btn.value);\r\n\
                });\r\n\
            });\r\n\
        });\r\n\
    </script>\r\n</head>\r\n<body>\r\n\
        <form name=\"commandform\">\r\n\
            <h1>W-Link API Debugger</h1>\r\n\
            <h2>URL:</h2>\r\n\
            <input name=\"commandurl\" type=\"text\" size=\"60\">\r\n\
            <div id=\"buttons\">\r\n\
                <button type=\"button\" id=\"btnGet\" value=\"GET\">GET</button>\r\n\
                <button type=\"button\" id=\"btnPut\" value=\"PUT\">PUT</button>\r\n\
                <button type=\"button\" id=\"btnPost\" value=\"POST\">POST</button>\r\n\
                <button type=\"button\" id=\"btnDelete\" value=\"DELETE\">DELETE</button>\r\n\
            </div>\r\n\
            <h2>Message Body:</h2>\r\n\
            <textarea name=\"messagebody\" rows=\"10\" cols=\"100\"></textarea>\r\n\
            <h2>Command Response:</h2>\r\n\
            <textarea name=\"response\" rows=\"25\" cols=\"100\"></textarea>\r\n\
        </form>\r\n\
    </body>\r\n</html>\r\n";

int cgiDebugPage(HTTPd_WebSocked_Client_Connection *connData);

const HttpdBuiltInUrl builtInUrls[] = {
	{ "/Debug", cgiDebugPage, NULL },
	{ NULL, NULL, NULL } //end marker
};

HttpdDynamicUrlItem* Registered_URL_List = NULL;

static void unmaskWsPayload(char *maskedPayload, uint64_t payloadLength, uint32_t maskingKey) {
	//the algorith described in IEEE RFC 6455 Section 5.3
	//TODO: this should decode the payload 4-byte wise and do the remainder afterwards
	if (payloadLength > HTTP_WS_SERVER_DAT_BUF_SIZE)
	{
		payloadLength = HTTP_WS_SERVER_DAT_BUF_SIZE;
	}
	for (int i = 0; i < payloadLength; i++) {
		int j = i % 4;
		maskedPayload[i] = maskedPayload[i] ^ ((uint8_t *)&maskingKey)[j];
	}
}

static void closeWebsocketConnection(struct HTTPd_WebSocked_Client_Connection* ws_client) {
	char closeMessage[CLOSE_MESSAGE_LENGTH] = CLOSE_MESSAGE;
	if (ws_client->socket_id != -1)
	{
		//write(ws_client->socket_id, closeMessage, CLOSE_MESSAGE_LENGTH);
                
#ifdef HTTPD_USE_SSL
                ssl_write(&ws_client->client_ssl_ctx, closeMessage, CLOSE_MESSAGE_LENGTH);
#else
		send(ws_client->socket_id, closeMessage, CLOSE_MESSAGE_LENGTH, 0);
#endif
	}
}

static void HTTPd_Connection_Timeout_CB(NeonRTOS_TimerHandle connection_timeout_timer_handle)
{
        uint32_t TimerID;//use timer ID to identify client connection
	NeonRTOS_TimerStop(&connection_timeout_timer_handle);
        if(NeonRTOS_GetTimerID(&connection_timeout_timer_handle, &TimerID)==NeonRTOS_OK)
        {
              TimerID-=HTTPD_TIMER_ID_OFFSET;
              UART_Printf("HTTPD Client Index %d Connection Timeout\n", TimerID);
              if(HTTPd_WebSocketd_Client_List[TimerID])
              {
                    HTTPd_WebSocketd_Client_List[TimerID]->connection_destruct_flag = true;
              }
              else
              {
                    NeonRTOS_TimerDelete(&connection_timeout_timer_handle);
              }
        }
}

static void HTTPd_Restore_Variables(uint8_t client_index)
{
	HTTPd_WebSocketd_Client_List[client_index]->socket_id = -1;
	HTTPd_WebSocketd_Client_List[client_index]->connetion_mode = HTTPd_Connection_Mode_Close;
	HTTPd_WebSocketd_Client_List[client_index]->connection_keep_alive_time = 0;
	HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = false;
	HTTPd_WebSocketd_Client_List[client_index]->websocket_client = false;
	HTTPd_WebSocketd_Client_List[client_index]->websocket_auth = false;
	HTTPd_WebSocketd_Client_List[client_index]->cgi = NULL;
}

static uint8_t GetNumOfHttpdClient()
{
	uint8_t current_http_client = 0;
	for (uint8_t k = 0;k < HTTPD_MAX_CLIENTS; k++)
	{
		if (HTTPd_WebSocketd_Client_List[k]==NULL)
                {
			continue;
                }
		if (HTTPd_WebSocketd_Client_List[k]->socket_id < 0)
		{
			continue;
		}
		current_http_client++;
	}
	return current_http_client;
}

static int FindHttpdClient(int http_client_sockID)
{
	for (uint8_t k = 0;k < HTTPD_MAX_CLIENTS; k++)
	{
		if (HTTPd_WebSocketd_Client_List[k]==NULL)
                {
			continue;
                }
		if (HTTPd_WebSocketd_Client_List[k]->socket_id == http_client_sockID)
		{
			return k;
		}
	}
	return -1;
}

static void HTTPd_WebsocketServer_Send_Message(HTTPd_WebSocked_Client_Connection* ws_client, const char *payload, uint64_t payloadLength, uint8_t flags) 
{
	if(ws_client==NULL)
	{
		return;
	}
	if (ws_client->socket_id < 0 || !(ws_client->websocket_client) || !(ws_client->websocket_auth))
	{
		return;
	}
	if (payloadLength == 0)
	{
		return;
	}
	uint8_t options;
	
	if (flags&WEBSOCK_FLAG_BIN) options = OPCODE_BINARY;
	else options = OPCODE_TEXT;
	if (!(flags&WEBSOCK_FLAG_CONT)) options |= FLAG_FIN;
	if ((flags&WEBSOCK_FLAG_PONG)) options |= OPCODE_PONG;
	
	char buf[14];
	int i = 0;
	buf[i++] = options;
	if (payloadLength > 65535) {
		buf[i++] = 127;
		buf[i++] = 0; buf[i++] = 0; buf[i++] = 0; buf[i++] = 0; 
		buf[i++] = payloadLength >> 24;
		buf[i++] = payloadLength >> 16;
		buf[i++] = payloadLength >> 8;
		buf[i++] = payloadLength;
	}
	else if (payloadLength > 125) {
		buf[i++] = 126;
		buf[i++] = payloadLength >> 8;
		buf[i++] = payloadLength;
	}
	else {
		buf[i++] = payloadLength;
	}
	
#ifdef HTTPD_USE_SSL
        ssl_write(&ws_client->client_destruct_flag, buf, i);
        ssl_write(&ws_client->client_destruct_flag, payload, payloadLength);
#else
        send(ws_client->socket_id, buf, i, 0);
        send(ws_client->socket_id, payload, payloadLength, 0);
#endif
}

static void HTTPd_WebsocketServer_Send_Brocast_Message(const char *payload, uint64_t payloadLength, uint8_t flags) 
{
	uint8_t i;
	for (i = 0;i < HTTPD_MAX_CLIENTS; i++)
	{
		if (HTTPd_WebSocketd_Client_List[i]==NULL)
		{
				continue;
		}
		if (HTTPd_WebSocketd_Client_List[i]->socket_id >= 0)
		{
			HTTPd_WebsocketServer_Send_Message(HTTPd_WebSocketd_Client_List[i], payload, payloadLength, flags);
		}
	}
}

static void HTTPd_WebSocketServerOnMessage(struct HTTPd_WebSocked_Client_Connection *src, const HTTPd_WSFrame *frame_header, uint8_t* msg) 
{
	if (wsServerOnMessageCallback != NULL)
	{
		wsServerOnMessageCallback(src, frame_header, msg);
	}
}

void WebsocketServer_SendMessage(HTTPd_WebSocked_Client_Connection* ws_client, const char *payload, uint64_t payloadLength, uint8_t flags) 
{
	HTTPd_WebsocketServer_Send_Message(ws_client, payload, payloadLength, flags);
}

void WebsocketServer_SendBrocastMessage(const char *payload, uint64_t payloadLength, uint8_t flags) 
{
	HTTPd_WebsocketServer_Send_Brocast_Message(payload, payloadLength, flags);
}

void WebsocketServer_SendJSONMessage(struct HTTPd_WebSocked_Client_Connection* ws_client, cJSON* data, bool need_delete_json) 
{
        if (data == NULL){return;}
        
	char* json_str = cJSON_Print(data);
        
        if(json_str==NULL)
        {
              if (need_delete_json == true)
              {
                      cJSON_Delete(data);
              }
              return;
        }
        
        cJSON_Minify(json_str);
        
	if (need_delete_json == true)
	{
		cJSON_Delete(data);
	}
        
	HTTPd_WebsocketServer_Send_Message(ws_client, (char*)json_str, strlen(json_str), WEBSOCK_FLAG_NONE);
        
	mem_Free(json_str);
}

void WebsocketServer_SendBrocastJSONMessage(cJSON* data, bool need_delete_json) 
{
        if (data == NULL){return;}
        
	char* json_str = cJSON_Print(data);
        if(json_str==NULL)
        {
              if (need_delete_json == true)
              {
                      cJSON_Delete(data);
              }
              return;
        }
        
        cJSON_Minify(json_str);
        
	if (need_delete_json == true)
	{
		cJSON_Delete(data);
	}
        
	HTTPd_WebsocketServer_Send_Brocast_Message((char*)json_str, strlen(json_str), WEBSOCK_FLAG_NONE);
	mem_Free(json_str);
}

void WebsocketServer_RegisterMsgCallback(HTTPd_WsServerOnMessageCB cb)
{
	wsServerOnMessageCallback = cb;
}

int WebSocketServer_CreateAcceptKey(const uint8_t *key, char *buffer, int bufferSize)
{
	char concatenatedBuffer[512];
	concatenatedBuffer[0] = '\0';
	//concatenate the key and the GUID
	strcat(concatenatedBuffer, (char*)key);
	strcat(concatenatedBuffer, WebSocket_UUID);

	sha1nfo s;
	uint8_t* pHash;
	
        sha1_init(&s);
        sha1_write(&s, concatenatedBuffer, strlen(concatenatedBuffer));
        pHash = sha1_result(&s);
	
	//UART_Printf("status = %d, olen = %d, bufferSize = %d, buffer = %s\n", olen, status, bufferSize, buffer);
	return base64_encode(20, pHash, bufferSize, buffer);
}

int WebSocketServer_ReceiveWsFrame(struct HTTPd_WebSocked_Client_Connection *conn, HTTPd_WSFrame *frame)
{
	uint8_t header_buf[2];
	int ret;
	
        int socket_errno;
        const uint32_t socket_errno_optlen = sizeof(socket_errno);
        
        if(conn==NULL)
        {
		return -1;
        }
        
        if(conn->connection_destruct_flag)
        {
		return -1;
        }
        
#ifdef HTTPD_USE_SSL
        ret = ssl_read(&conn->client_ssl_ctx, header_buf, 2);
#else
        if(conn->socket_id<0)
        {
		return -1;
        }
	ret = recv(conn->socket_id, header_buf, 2, 0);
#endif

	if (ret <= 0)
	{
		return ret;
	}
	frame->flags = header_buf[0] & FLAGS_MASK;
	frame->opcode = header_buf[0] & OPCODE_MASK;
	//next byte
	frame->isMasked = header_buf[1] & IS_MASKED;
	frame->payloadLength = header_buf[1] & PAYLOAD_MASK;

	uint8_t i;
	uint64_t payloadLength = frame->payloadLength;

	if (payloadLength == 126) {
		uint8_t len_data_buf[sizeof(uint16_t)];

#ifdef HTTPD_USE_SSL
                ret = ssl_read(&conn->client_ssl_ctx, len_data_buf, sizeof(uint16_t));
#else
		ret = recv(conn->socket_id, len_data_buf, sizeof(uint16_t), 0);
#endif

		if (ret <= 0)
		{
			return ret;
		}
		
		payloadLength = 0;
		for (i = 0; i < sizeof(uint16_t); i++)
		{
			payloadLength <<= 8;
			payloadLength |= len_data_buf[i];
		}
	}
	else if (payloadLength == 127) {
		uint8_t len_data_buf[sizeof(uint64_t)];

#ifdef HTTPD_USE_SSL
                ret = ssl_read(&conn->client_ssl_ctx, len_data_buf, sizeof(uint64_t));
#else
		ret = recv(conn->socket_id, len_data_buf, sizeof(uint64_t), 0);
#endif

		if (ret <= 0)
		{
			return ret;
		}
		
		payloadLength = 0;
		for (i = 0; i < sizeof(uint64_t); i++)
		{
			payloadLength <<= 8;
			payloadLength |= len_data_buf[i];
		}
	}

	frame->payloadLength = payloadLength;

	if (frame->isMasked) {
		uint8_t masking_key_buf[sizeof(uint32_t)];

#ifdef HTTPD_USE_SSL
                ret = ssl_read(&conn->client_ssl_ctx, masking_key_buf, sizeof(uint32_t));
#else
		ret = recv(conn->socket_id, masking_key_buf, sizeof(uint32_t), 0);
#endif

		if (ret <= 0)
		{
			return ret;
		}
		
		memcpy(&frame->maskingKey, masking_key_buf, sizeof(uint32_t));
	}

	return 0;
}

int HTTPd_Parse_InMsg_Headers(uint8_t *header, uint16_t header_len, HTTPd_WebSocked_Client_Connection *connData) {
    
	connData->url = NULL;

	uint8_t* second_line = (uint8_t*)strstr((const char*)header, "\r\n");
	if (second_line == NULL)
	{
		return -1;
	}
	second_line += strlen("\r\n");
	
	connData->requestType = HTTPd_Get_Header_Method((uint8_t*) header);
        if(connData->requestType==HTTPd_Method_UNKNOWN)
	{
		return -1;
	}
	
	connData->url = (char*)HTTPd_Get_Header_Path((uint8_t*) header);
	if (connData->url == NULL)
	{
		return -1;
	}
        
        uint8_t *e = (uint8_t *)strchr(connData->url, ' ');
        if (e == NULL) {
		return -1;
        }
        *e = '\0'; // 正確終止 URL

	connData->getArgs = (char*)strstr(connData->url, "?");
	if (connData->getArgs != 0) {
		*connData->getArgs = 0;
		connData->getArgs++;
		UART_Printf("args = %s\n", connData->getArgs);
	}
	else {
		connData->getArgs = NULL;
	}

	
	connData->hostName = (char*)HTTPd_Get_Host((uint8_t*) second_line);
	if (connData->hostName == NULL)
	{
		connData->hostName = NULL;
	}
		
	uint8_t *conn_str = HTTPd_Get_Header_Value((uint8_t*)second_line, (uint8_t*)HTTP_Connection);
	if (conn_str != NULL) {
		if (strncmp((char*)conn_str, HTTP_Upgrade, strlen(HTTP_Upgrade)) == 0 || strncmp((char*)conn_str, HTTP_Upgrade_LowerCase, strlen(HTTP_Upgrade_LowerCase)) == 0)
		{
			uint8_t *upgrade_ws = HTTPd_Get_Header_Value((uint8_t*)second_line, (uint8_t*)HTTP_Upgrade);
			if (upgrade_ws == NULL)
			{
		                return -1;
			}
		
			if (strncmp((char*)upgrade_ws, WebSocket_Identifier_Upper, strlen(WebSocket_Identifier_Upper)) != 0 && strncmp((char*)upgrade_ws, WebSocket_Identifier_Lower, strlen(WebSocket_Identifier_Lower)) != 0)
			{
		                return -1;
			}
			
			uint8_t *key = HTTPd_Get_Header_Value((uint8_t*)second_line, (uint8_t*)WebSocket_Key_Identifier);
			if (key == NULL) {
		                return -1;
			}
																		//the key ends at the newline
			uint8_t *endSequence = (uint8_t*)strstr((const char*)key, "\r\n");
			if (endSequence == NULL) {
				endSequence = (uint8_t*)strchr((const char*)key, '\0');
				if (endSequence == NULL) {
                                        return -1;
				}
			}
		
			//we can throw away all the other data, only the key is interesting
			key[endSequence - key] = '\0';
		
			connData->client_key = key;
		
			connData->websocket_client = true;
                        
                        connData->connetion_mode = HTTPd_Connection_Mode_Upgrade;
                        
                        NeonRTOS_TimerChangePeriod(&connData->connection_timeout_timer, WEBSOCKET_TIMEOUT_TIMER_MS);
		}
		else
		{
                        connData->connetion_mode = HTTPd_Connection_Mode_Close;
                        if (strncmp((char*)conn_str, HTTP_Keep_Alive, strlen(HTTP_Keep_Alive)) == 0 || strncmp((char*)conn_str, HTTP_Keep_Alive_LowerCase, strlen(HTTP_Keep_Alive_LowerCase)) == 0)
                        {
                                connData->connetion_mode = HTTPd_Connection_Mode_Keep_Alive;
                                
                                uint8_t *keep_alive_time_str = HTTPd_Get_Header_Value(second_line, (uint8_t*)HTTP_Keep_Alive);
                                uint8_t *keep_alive_time_end_str = NULL;
                                if (keep_alive_time_str != NULL) {
                                        keep_alive_time_end_str = (uint8_t*)strstr((char*)keep_alive_time_str, "\r\n");
                                }
                                if (keep_alive_time_str != NULL && keep_alive_time_end_str != NULL) {
                                        uint8_t keep_alive_time_str_buf[20];
                                        memset(keep_alive_time_str_buf, 0, 20);
                                        memcpy(keep_alive_time_str_buf, keep_alive_time_str, keep_alive_time_end_str-keep_alive_time_str);
                                        connData->connection_keep_alive_time = atoi((char *)keep_alive_time_str_buf);
                                } else {
                                        connData->connection_keep_alive_time = HTTP_TIMEOUT_TIMER_MS / 1000; // 預設 60 秒
                                }
                                NeonRTOS_TimerChangePeriod(&connData->connection_timeout_timer, connData->connection_keep_alive_time * 1000);
                        }
                        
			uint8_t *content_len_str = HTTPd_Get_Header_Value(second_line, (uint8_t*)HTTP_Content_Length);
			if (content_len_str != NULL)
			{
                                uint8_t *cont_len_end_ptr = (uint8_t*)strstr((char*)content_len_str, "\r\n");
                                if (cont_len_end_ptr != NULL)
                                {
                                        uint8_t content_len_str_buf[20];
                                        memset(content_len_str_buf, 0, 20);
                                        memcpy(content_len_str_buf, content_len_str, cont_len_end_ptr-content_len_str);
                                        connData->data_len = atoi((char *)content_len_str_buf);
                                        connData->isLargeData = false;
                                        connData->data_buff = NULL;
                                }
			}
                        //the key ends at the newline
                        
			uint8_t *accept_encoding = HTTPd_Get_Header_Value(second_line, (uint8_t*)HTTP_Accept_Encoding);
			if (accept_encoding != NULL)
			{
                                uint8_t *accept_encoding_end_ptr = (uint8_t*)strstr((char*)accept_encoding, "\r\n");
                                if (accept_encoding_end_ptr != NULL)
                                {
                                        memset(connData->accept_encoding, 0, sizeof(connData->accept_encoding));
                                        memcpy(connData->accept_encoding, accept_encoding, accept_encoding_end_ptr-accept_encoding);
                                }
			}	
                        
			uint8_t *cont_type = HTTPd_Get_Header_Value(second_line, (uint8_t*)HTTP_Content_Type);
			if (cont_type != NULL)
			{
				//not support
			}	
		}
	}
        
        return 0;
}

int HTTPd_Register_CGI_URL_Callback(const char *url, cgiSendCallback cgiCb, void* cgiArg)
{
      bool exist = false;
      HttpdDynamicUrlItem* pItemLast = NULL;
      HttpdDynamicUrlItem* pItemCurrent = Registered_URL_List;
      
      while(pItemCurrent!=NULL)
      {
              if(strlen(url)==strlen(pItemCurrent->url))
              {
                      if(strncmp(url, pItemCurrent->url, strlen(url))==0)
                      {
                            exist = true;
                            break;
                      }
              }
              pItemLast = pItemCurrent;
              pItemCurrent = pItemCurrent->next;
      }
      
      if(exist)
      {
              pItemCurrent->cgiCb = cgiCb;
              pItemCurrent->cgiArg = cgiArg;
              
              return 0;
      }
      
      HttpdDynamicUrlItem* pItemNew = mem_Malloc(sizeof(HttpdDynamicUrlItem));
      if(pItemNew==NULL)
      {
              return -1;
      }
      
      pItemNew->url = url;
      pItemNew->cgiCb = cgiCb;
      pItemNew->cgiArg = cgiArg;
      pItemNew->next = NULL;
      
      if(pItemLast==NULL && pItemCurrent==NULL)
      {
              Registered_URL_List = pItemNew;
      }
      else{
              pItemLast->next = pItemNew;
      }
      
      return 0;
}

int HTTPd_UnRegister_CGI_URL_Callback(const char *url)
{
      bool exist = false;
      HttpdDynamicUrlItem* pItemLast = NULL;
      HttpdDynamicUrlItem* pItemCurrent = Registered_URL_List;
      
      while(pItemCurrent!=NULL)
      {
              if(strlen(url)==strlen(pItemCurrent->url))
              {
                      if(strncmp(url, pItemCurrent->url, strlen(url))==0)
                      {
                            exist = true;
                            
                            pItemLast->next = pItemCurrent->next;
                            
                            mem_Free(pItemCurrent);
                            
                            break;
                      }
              }
              pItemLast = pItemCurrent;
              pItemCurrent = pItemCurrent->next;
      }
      
      if(exist)
      {
              return 0;
      }
      return -1;
}

HTTPd_Method HTTPd_Get_CGI_Request_Type(HTTPd_WebSocked_Client_Connection *connData)
{
      return (HTTPd_Method) connData->requestType;
}

const char* HTTPd_Get_CGI_Request_URL(HTTPd_WebSocked_Client_Connection *connData)
{
      return (const char*) connData->url;
}

const uint8_t* HTTPd_Get_CGI_Request_Data(HTTPd_WebSocked_Client_Connection *connData)
{
      return (const uint8_t*) connData->data_buff;
}

int HTTPd_CGI_PathStartsWith(const char *url,
                          const char *prefix,
                          const char **rest)      // 可為 NULL
{
    if (!url || !prefix) return -1;

    size_t plen = strlen(prefix);
    if (strncmp(url, prefix, plen) != 0) return -1;

    if (rest) *rest = url + plen;
    return 0;
}

int HTTPd_CGI_ParseUint(const char *s)
{
    if (!s || !*s) return 0;
    char *endptr;
    long val = strtol(s, &endptr, 10);
    if (*endptr != '\0' || val < 0 || val > INT_MAX) return 0;
    return (int)val;
}

int HTTPd_Send_CGI_Response(HTTPd_WebSocked_Client_Connection *connData, uint16_t status_code, const char* rsp_type, uint8_t* rsp_data, uint64_t rsp_len)
{
        int socket_status;
        
        if(connData==NULL)
        {
                return HTTPD_CGI_DONE;
        }
        
        uint8_t* http_cmd_buf = mem_Malloc(HTTPD_CMD_BUF_SIZE);
        if (http_cmd_buf == NULL)
        {
                return HTTPD_CGI_DONE;
        }
        memset(http_cmd_buf, 0, HTTPD_CMD_BUF_SIZE);
        
        uint8_t* buf_ptr;
        
        buf_ptr = HTTPd_Parse_Status_Code(http_cmd_buf, status_code);
        buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Server, "NeonRT - httpd / "HTTPDVER"");
        
        switch(connData->connetion_mode)
        {
        case HTTPd_Connection_Mode_Close:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Close);
                break;
        case HTTPd_Connection_Mode_Keep_Alive:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Keep_Alive);
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Keep_Alive, "timeout="HTTP_TIMEOUT_STR", max=1");
                break;
        case HTTPd_Connection_Mode_Upgrade:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Upgrade);
                break;
        default:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Close);
                break;
        }
        
        bool match = false;
        uint8_t index = 0;
        while(mimeTypes[index].ext!=NULL)
        {
                if(strlen(rsp_type)==strlen(mimeTypes[index].ext))
                {
                        if(strncmp(rsp_type, mimeTypes[index].ext, strlen(rsp_type))==0)
                        {
                                match = true;
                                break;
                        }
                }
                index++;
        }
        
        if(match)
        {
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Content_Type, mimeTypes[index].mimetype);
        }
        else{
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Content_Type, "text/html");
        }
        
        buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Access_Control_Allow_Origin, "*");
        
        buf_ptr = HTTPd_Parse_End_Of_Headers(buf_ptr);
        
#ifdef HTTPD_USE_SSL
        socket_status = ssl_write(&connData->client_ssl_ctx, http_cmd_buf, buf_ptr - http_cmd_buf);
#else
        socket_status = send(connData->socket_id, http_cmd_buf, buf_ptr - http_cmd_buf, 0);
#endif
        
        mem_Free(http_cmd_buf);
        
        if (socket_status < 0)
        {
                return HTTPD_CGI_DONE;
        }
                
        if (rsp_len > 0 )
        {
#ifdef HTTPD_USE_SSL
                socket_status = ssl_write(&connData->client_ssl_ctx, rsp_data, rsp_len);
#else
                socket_status = send(connData->socket_id, rsp_data, rsp_len, 0);
#endif
        }
        
        return HTTPD_CGI_DONE;
}

int HTTPd_Send_CGI_JSON_Response(HTTPd_WebSocked_Client_Connection *connData, uint16_t status_code, cJSON* rsp_dat, bool need_delete_json)
{
      if (connData == NULL){return HTTPD_CGI_DONE;}
      
      if (rsp_dat == NULL){return HTTPD_CGI_DONE;}
      
      int status;
      char* json_str = cJSON_Print(rsp_dat);
      
      if(json_str==NULL)
      {
              if (need_delete_json == true)
              {
                      cJSON_Delete(rsp_dat);
              }
              return HTTPD_CGI_DONE;
      }
        
      cJSON_Minify(json_str);
      
      if (need_delete_json == true)
      {
              cJSON_Delete(rsp_dat);
      }
      
      status = HTTPd_Send_CGI_Response(connData, status_code, "json", json_str, strlen(json_str));
      
      mem_Free(json_str);
      
      return status;
}

//Returns a static char* to a mime type for a given url to a file.
const char   *HTTPdGetMimetype(char *url) {
	int i = 0;
	//Go find the extension
	char *ext = url + (strlen(url) - 1);
	while (ext != url && *ext != '.') ext--;
	if (*ext == '.') ext++;
    
	//ToDo: strcmp is case sensitive; we may want to do case-intensive matching here...
	while (mimeTypes[i].ext != NULL && strcmp(ext, mimeTypes[i].ext) != 0) i++;
	return mimeTypes[i].mimetype;
}

static int HTTP_SendHeader(HTTPd_WebSocked_Client_Connection *connData, int statusCode, const char* pContentTypeStr, uint32_t contentLength, bool isGzip, const char* pNewUrl)
{
	char content_len_str[16+1];
	uint16_t content_len_str_len = sprintf(content_len_str, "%d", contentLength);
	content_len_str[content_len_str_len] = '\0';
	
	uint8_t* buf_ptr;
	uint8_t* http_cmd_buf = mem_Malloc(HTTPD_CMD_BUF_SIZE);
	if (http_cmd_buf == NULL)
	{
		return HTTPD_CGI_DONE;
	}
	memset(http_cmd_buf, 0, HTTPD_CMD_BUF_SIZE);

	buf_ptr = HTTPd_Parse_Status_Code(http_cmd_buf, statusCode);
        buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Server, "NeonRT - httpd / "HTTPDVER"");
        
        switch(connData->connetion_mode)
        {
        case HTTPd_Connection_Mode_Close:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Close);
                break;
        case HTTPd_Connection_Mode_Keep_Alive:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Keep_Alive);
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Keep_Alive, "timeout="HTTP_TIMEOUT_STR", max=1");
                break;
        case HTTPd_Connection_Mode_Upgrade:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Upgrade);
                break;
        default:
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Close);
                break;
        }
        if(pNewUrl)
        {
	        buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Location, pNewUrl);
        }
	buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Content_Type, pContentTypeStr);
	buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Content_Length, content_len_str);
        if (isGzip)
        {
                buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Content_Encoding, "gzip");
        }
	buf_ptr = HTTPd_Parse_End_Of_Headers(buf_ptr);
	
	int socket_status;
	
#ifdef HTTPD_USE_SSL
        socket_status = ssl_write(&connData->client_ssl_ctx, http_cmd_buf, buf_ptr - http_cmd_buf);
#else
	socket_status = send(connData->socket_id, http_cmd_buf, buf_ptr - http_cmd_buf, 0);
#endif
	
	if (socket_status < 0)
	{
	        mem_Free(http_cmd_buf);
		return HTTPD_CGI_DONE;
	}
	
	mem_Free(http_cmd_buf);

        return HTTPD_CGI_MORE;
}

int HTTPd_SendWebFile(HTTPd_WebSocked_Client_Connection *connData)
{
        if(connData==NULL) return HTTPD_CGI_DONE;
        
	if (connData->url == NULL)
	{
		return HTTPD_CGI_DONE;
	}
	
	int content_len;

	int socket_status;
	
	const char* web_file_content_type_str = HTTPdGetMimetype(connData->url);

	NeonRtFsFile* send_file = NULL;
        send_file = NeonRtFsOpen(connData->url);
	if (send_file == NULL)
	{
                return HTTPD_CGI_NOTFOUND;
	}

        content_len = send_file->header.fileLenDecomp;
	
        int r;
        bool isGzip = NeonRtFsFlags(send_file) & FLAG_GZIP;
        r = HTTP_SendHeader(connData, 200, web_file_content_type_str, content_len, isGzip, NULL);
        if(r!=HTTPD_CGI_MORE)
        {
                NeonRtFsClose(send_file);
                return HTTPD_CGI_DONE;
        }
	
        uint8_t* web_send_buf = mem_Malloc(HTTPD_DAT_BUF_SIZE);
        if (web_send_buf != NULL)
        {
                uint32_t fs_rd_len;
        
                do
                {
                        fs_rd_len = NeonRtFsRead(send_file, web_send_buf, HTTPD_DAT_BUF_SIZE);
#ifdef HTTPD_USE_SSL
                        socket_status = ssl_write(&connData->client_ssl_ctx, web_send_buf, fs_rd_len);
#else
                        socket_status = send(connData->socket_id, web_send_buf, fs_rd_len, 0);
#endif
                }
                while (socket_status > 0 && fs_rd_len>=HTTPD_DAT_BUF_SIZE);
                
                mem_Free(web_send_buf);
        }
        
        NeonRtFsClose(send_file);

	return HTTPD_CGI_DONE;
}

int HTTPd_Redirect(HTTPd_WebSocked_Client_Connection *connData, char *newUrl)
{
	int content_len;
	bool web_file_exist;
	int socket_status;
        const char* pRedirMsg = "Redirecting\r\n";
	
        if(connData==NULL) return HTTPD_CGI_DONE;
        
	NeonRtFsFile* redir_web_file = NeonRtFsOpen("Redirect.html");
	if (redir_web_file == NULL)
	{
		web_file_exist = false;
		content_len = strlen(pRedirMsg);
	}
	else
	{
		web_file_exist = true;
		content_len = redir_web_file->header.fileLenDecomp;
	}
	
        int r;
        bool isGzip = NeonRtFsFlags(redir_web_file) & FLAG_GZIP;
        if (isGzip) {
                if (strstr(connData->accept_encoding, "gzip") == NULL){
                        const char* pRetMsg = "Your browser does not accept gzip-compressed data.\r\n";

                        r = HTTP_SendHeader(connData, 501, "text/plain", strlen(pRetMsg), false, NULL);
                        if(r!=HTTPD_CGI_MORE)
                        {
                                return HTTPD_CGI_DONE;
                        }

#ifdef HTTPD_USE_SSL
                        socket_status = ssl_write(&connData->client_ssl_ctx, pRetMsg, strlen(pRetMsg));
#else
                        socket_status = send(connData->socket_id, pRetMsg, strlen(pRetMsg), 0);
#endif

                        NeonRtFsClose(redir_web_file);
                        
                        return HTTPD_CGI_DONE;
                }
        }
        
	if (web_file_exist == true)
	{
                r = HTTP_SendHeader(connData, 302, HTTPdGetMimetype("Redirect.html"), content_len, isGzip, newUrl);
	}
	else
	{
                r = HTTP_SendHeader(connData, 302, "text/plain", content_len, isGzip, newUrl);
	}
        if(r!=HTTPD_CGI_MORE)
        {
		if (web_file_exist == true)
		{
                        NeonRtFsClose(redir_web_file);
		}
                return HTTPD_CGI_DONE;
        }
	
	if (web_file_exist == true)
	{
		uint8_t* web_send_buf = mem_Malloc(HTTPD_DAT_BUF_SIZE);
		if (web_send_buf != NULL)
		{
                        uint32_t fs_rd_len;
                
                        do
                        {
                                fs_rd_len = NeonRtFsRead(redir_web_file, web_send_buf, HTTPD_DAT_BUF_SIZE);
                                
#ifdef HTTPD_USE_SSL
                                socket_status = ssl_write(&connData->client_ssl_ctx, web_send_buf, fs_rd_len);
#else
                                socket_status = send(connData->socket_id, web_send_buf, fs_rd_len, 0);
#endif
                        }
                        while (socket_status > 0 && fs_rd_len>=HTTPD_DAT_BUF_SIZE);
                        
		        mem_Free(web_send_buf);
                }
                
		NeonRtFsClose(redir_web_file);
	}
	else
	{
#ifdef HTTPD_USE_SSL
                socket_status = ssl_write(&connData->client_ssl_ctx, pRedirMsg, strlen(pRedirMsg));
#else
		socket_status = send(connData->socket_id, pRedirMsg, strlen(pRedirMsg), 0);
#endif
	}
	return HTTPD_CGI_DONE;
}

int HTTPd_NotFound(HTTPd_WebSocked_Client_Connection *connData)
{
	int content_len;
	bool web_file_exist;
	int socket_status;
        const char* pNotFoundMsg = "Not Found.\r\n";
	
        if(connData==NULL) return HTTPD_CGI_DONE;
        
	NeonRtFsFile* notfound_web_file = NeonRtFsOpen("NotFound.html");
	if (notfound_web_file == NULL)
	{
		web_file_exist = false;
		content_len = strlen(pNotFoundMsg);
	}
	else
	{
		web_file_exist = true;
		content_len = notfound_web_file->header.fileLenDecomp;
	}
	
        int r;
        bool isGzip = NeonRtFsFlags(notfound_web_file) & FLAG_GZIP;
        if (isGzip) {
                if (strstr(connData->accept_encoding, "gzip") == NULL){
                        const char* pRetMsg = "Your browser does not accept gzip-compressed data.\r\n";

                        r = HTTP_SendHeader(connData, 501, "text/plain", strlen(pRetMsg), false, NULL);
                        if(r!=HTTPD_CGI_MORE)
                        {
                                return HTTPD_CGI_DONE;
                        }

#ifdef HTTPD_USE_SSL
                        socket_status = ssl_write(&connData->client_ssl_ctx, pRetMsg, strlen(pRetMsg));
#else
                        socket_status = send(connData->socket_id, pRetMsg, strlen(pRetMsg), 0);
#endif

                        NeonRtFsClose(notfound_web_file);
                        
                        return HTTPD_CGI_DONE;
                }
        }
        
	if (web_file_exist == true)
	{
                r = HTTP_SendHeader(connData, 404, HTTPdGetMimetype("NotFound.html"), content_len, isGzip, NULL);
	}
	else
	{
                r = HTTP_SendHeader(connData, 404, "text/plain", content_len, isGzip, NULL);
	}
        if(r!=HTTPD_CGI_MORE)
        {
		if (web_file_exist == true)
		{
                        NeonRtFsClose(notfound_web_file);
		}
                return HTTPD_CGI_DONE;
        }
	
	if (web_file_exist == true)
	{
		uint8_t* web_send_buf = mem_Malloc(HTTPD_DAT_BUF_SIZE);
		if (web_send_buf != NULL)
		{
                        uint32_t fs_rd_len;
                
                        do
                        {
                                fs_rd_len = NeonRtFsRead(notfound_web_file, web_send_buf, HTTPD_DAT_BUF_SIZE);
                                
#ifdef HTTPD_USE_SSL
                                socket_status = ssl_write(&connData->client_ssl_ctx, web_send_buf, HTTPD_DAT_BUF_SIZE);
#else
                                socket_status = send(connData->socket_id, web_send_buf, HTTPD_DAT_BUF_SIZE, 0);
#endif
                        }
                        while (socket_status > 0 && fs_rd_len>=HTTPD_DAT_BUF_SIZE);
                        
		        mem_Free(web_send_buf);
                }
                
		NeonRtFsClose(notfound_web_file);
	}
	else
	{
#ifdef HTTPD_USE_SSL
                socket_status = ssl_write(&connData->client_ssl_ctx, pNotFoundMsg, strlen(pNotFoundMsg));
#else
		socket_status = send(connData->socket_id, pNotFoundMsg, strlen(pNotFoundMsg), 0);
#endif
	}
	return HTTPD_CGI_DONE;
}

int HTTPdSendApiDebugToolPage(HTTPd_WebSocked_Client_Connection *connData)
{
        if(connData==NULL) return HTTPD_CGI_DONE;
        
	if (connData->url == NULL)
	{
		return HTTPD_CGI_DONE;
	}
	
	int content_len;

	const char* api_dbg_page_content_type_str = HTTPdGetMimetype(connData->url);

	content_len = strlen(HTTP_API_Debug_Tool_Page_HTML);
	
        int r = HTTP_SendHeader(connData, 200, api_dbg_page_content_type_str, content_len, false, NULL);
        if(r!=HTTPD_CGI_MORE)
        {
		return HTTPD_CGI_DONE;
        }

#ifdef HTTPD_USE_SSL
        ssl_write(&connData->client_ssl_ctx, HTTP_API_Debug_Tool_Page_HTML, strlen(HTTP_API_Debug_Tool_Page_HTML));
#else
	send(connData->socket_id, HTTP_API_Debug_Tool_Page_HTML, strlen(HTTP_API_Debug_Tool_Page_HTML), 0);
#endif
	
	return HTTPD_CGI_DONE;
}

int cgiDebugPage(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    switch (connData->requestType)
    {
    case HTTPd_Method_POST:
                return HTTPD_CGI_DONE;
                break;
    case HTTPd_Method_GET:
                HTTPdSendApiDebugToolPage(connData);
                break;
    }
    
    return HTTPD_CGI_DONE;
}

static bool HTTPd_isMatch_CGI(HTTPd_WebSocked_Client_Connection *connData)
{
	int i = 0;
        
        if(connData==NULL) return false;
        
	if (connData->url == NULL) {
		return false; //Shouldn't happen
	}

        bool match = false;
	while (builtInUrls[i].url != NULL) {
		//See if there's a literal match
                if(strlen(builtInUrls[i].url)==strlen(connData->url))
                {
                        if (strcmp(builtInUrls[i].url, connData->url) == 0) {
                                match = true;
                                UART_Printf("Is url index %d, %s\n", i, builtInUrls[i].url);
                                connData->cgi = builtInUrls[i].cgiCb;
                                connData->cgiArg = builtInUrls[i].cgiArg;
                                break;
                        }
                        
			if (builtInUrls[i].url[strlen(builtInUrls[i].url)-1]=='*' &&
                            strncmp(builtInUrls[i].url, connData->url, strlen(builtInUrls[i].url)-1)==0){
                                match = true;
                                connData->cgi = builtInUrls[i].cgiCb;
                                connData->cgiArg = builtInUrls[i].cgiArg;
                                break;
                        }
                        
                        //See if there's a wildcard match
                }
		i++;
	}

        HttpdDynamicUrlItem* pItemCurrent = Registered_URL_List;
        while(pItemCurrent!=NULL)
        {
                if(strlen(pItemCurrent->url)==strlen(connData->url))
                {
                        if (strcmp(pItemCurrent->url, connData->url) == 0) {
                                match = true;
                                connData->cgi = pItemCurrent->cgiCb;
                                connData->cgiArg = pItemCurrent->cgiArg;
                                break;
                        }
                        
			if (pItemCurrent->url[strlen(pItemCurrent->url)-1]=='*' &&
                            strncmp(pItemCurrent->url, connData->url, strlen(pItemCurrent->url)-1)==0){
                                match = true;
                                connData->cgi = pItemCurrent->cgiCb;
                                connData->cgiArg = pItemCurrent->cgiArg;
                                break;
                        }
                }
                pItemCurrent = pItemCurrent->next;
        }

        return match;
}

int HTTPd_Process_POST_Request(HTTPd_WebSocked_Client_Connection *connData)
{
	int r;

	if (HTTPd_isMatch_CGI(connData))
	{
                int recvBlockTime = 0;
                
                recvBlockTime = 10000;
                if(setsockopt(connData->socket_id, SOL_SOCKET, SO_RCVTIMEO, &recvBlockTime, sizeof(recvBlockTime) < 0)) {
                        return HTTPD_CGI_DONE;
                }
                
		r = connData->cgi(connData);
                
                connData->cgi = NULL;
                connData->cgiArg = NULL;
                
                recvBlockTime = 500;
                if(setsockopt(connData->socket_id, SOL_SOCKET, SO_RCVTIMEO, &recvBlockTime, sizeof(recvBlockTime) < 0)) {
                        return HTTPD_CGI_DONE;
                }
                
                if(r==HTTPD_CGI_NOTFOUND)
                {
                      r = HTTPd_NotFound(connData);
                }
	}
        else{
                r = HTTPd_NotFound(connData);
        }
        
        return HTTPD_CGI_DONE;
}

int HTTPd_Process_GET_Request(HTTPd_WebSocked_Client_Connection *connData) {
	if (connData->websocket_client == true)
	{
		char* acceptKey = mem_Malloc(100);
		if (acceptKey == NULL) {
                        return HTTPD_CGI_DONE;
		}

		WebSocketServer_CreateAcceptKey(connData->client_key, acceptKey, 100);
                
		uint8_t* ws_cmd_send_buf = mem_Malloc(WS_SERVER_CMD_BUF_SIZE);
		if (ws_cmd_send_buf == NULL) {
			mem_Free(acceptKey);
                        return HTTPD_CGI_DONE;
		}
		
		memset(ws_cmd_send_buf, 0, WS_SERVER_CMD_BUF_SIZE);
                
		uint8_t* buf_ptr;
		buf_ptr = HTTPd_Parse_Status_Code(ws_cmd_send_buf, 101);
		buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Upgrade, WebSocket_Identifier_Upper);
		buf_ptr = HTTPd_Parse_Headers(buf_ptr, HTTP_Connection, HTTP_Upgrade);
		buf_ptr = HTTPd_Parse_Headers(buf_ptr, WebSocket_Accept_Identifier, acceptKey);
		buf_ptr = HTTPd_Parse_End_Of_Headers(buf_ptr);

#ifdef HTTPD_USE_SSL
                ssl_write(&connData->client_ssl_ctx, ws_cmd_send_buf, buf_ptr - ws_cmd_send_buf);
#else
		send(connData->socket_id, ws_cmd_send_buf, buf_ptr - ws_cmd_send_buf, 0);
#endif
		
		mem_Free(acceptKey);
		
		mem_Free(ws_cmd_send_buf);

		connData->websocket_auth = true;

                return HTTPD_CGI_MORE;
	}
	else
	{
		int r;
		
                if (HTTPd_isMatch_CGI(connData))
                {
                        r = connData->cgi(connData);
                        
                        connData->cgi = NULL;
                        connData->cgiArg = NULL;
                        
                        if(r==HTTPD_CGI_NOTFOUND)
                        {
                              r = HTTPd_NotFound(connData);
                        }
                        
		}
		else
		{
                        r = HTTPD_CGI_DONE;
			if (strncmp(connData->url, "/", 1) == 0 && strlen(connData->url) == strlen("/"))
			{
                                connData->url = "index.html";
                                r = HTTPd_SendWebFile(connData);
			}
			else
			{
				r = HTTPd_SendWebFile(connData);
                                if(r==HTTPD_CGI_NOTFOUND)
                                {
                                      HTTPd_NotFound(connData);
                                }
			}
		}

                return HTTPD_CGI_DONE;
	}
}

int HTTPd_Process_PUT_Request(HTTPd_WebSocked_Client_Connection *connData)
{
	int r;
		
	if (HTTPd_isMatch_CGI(connData))
	{
		r = connData->cgi(connData);
                
                connData->cgi = NULL;
                connData->cgiArg = NULL;
                
                if(r==HTTPD_CGI_NOTFOUND)
                {
                      r = HTTPd_NotFound(connData);
                } 
	}
        else{
                r = HTTPd_NotFound(connData);
        }
        
	connData->cgi = NULL; //mark for destruction

        return HTTPD_CGI_DONE;
}

int HTTPd_Process_DELETE_Request(HTTPd_WebSocked_Client_Connection *connData)
{
	int r;

	if (HTTPd_isMatch_CGI(connData))
	{
		r = connData->cgi(connData);
                
                connData->cgi = NULL;
                connData->cgiArg = NULL;
                
                if(r==HTTPD_CGI_NOTFOUND)
                {
                      r = HTTPd_NotFound(connData);
                } 
	}
        else{
                r = HTTPd_NotFound(connData);
        }

	connData->cgi = NULL; //mark for destruction

        return HTTPD_CGI_DONE;
}

void HTTP_Server_Task(void *pvParameters)
{
        int32_t ret;
        bool add_HTTP_client;
        
        //struct timeval timeout;
        struct sockaddr_in server_addr;
        int current_http_socketID;
        struct sockaddr current_http_socket_addr;
        socklen_t sin_size = sizeof(current_http_socket_addr);
        
        /* Construct local address structure */
        memset(&server_addr, 0, sizeof(server_addr)); /* Zero out structure */
        server_addr.sin_family = AF_INET;            /* Internet address family */
        server_addr.sin_addr.s_addr = INADDR_ANY;   /* Any incoming interface */
        server_addr.sin_len = sizeof(server_addr);
        server_addr.sin_port = htons(HTTP_PORT); /* Local port */
        int socket_errno;
        const u32_t socket_errno_optlen = sizeof(socket_errno);

        
#ifdef HTTPD_USE_SSL
        x509_crt server_x509;
        pk_context server_pk;
        ssl_context ssl;
        
        memset(&server_x509, 0, sizeof(x509_crt));
        memset(&server_pk, 0, sizeof(pk_context));
        memset(&ssl, 0, sizeof(ssl_context));
        x509_crt_init(&server_x509);
        pk_init(&server_pk);

        if((ret = x509_crt_parse(&server_x509, (const unsigned char *)test_srv_crt, strlen(test_srv_crt))) != 0) {
                UART_Printf(" failed\n  ! x509_crt_parse returned %d\n\n", ret);
                goto exit;
        }

        if((ret = x509_crt_parse(&server_x509, (const unsigned char *)test_ca_list, strlen(test_ca_list))) != 0) {
                UART_Printf(" failed\n  ! x509_crt_parse returned %d\n\n", ret);
                goto exit;
        }

        if((ret = pk_parse_key(&server_pk, test_srv_key, strlen(test_srv_key), NULL, 0)) != 0) {
                UART_Printf(" failed\n  ! pk_parse_key returned %d\n\n", ret);
                goto exit;
        }

#endif
        
        if(HTTP_WebSocket_Server_Socket>=0)
        {
                NeonRTOS_TaskDelete(NULL);
                return;
        }
        
        /* Create socket for incoming connections */
        HTTP_WebSocket_Server_Socket = socket(AF_INET, SOCK_STREAM, 0);
        if (HTTP_WebSocket_Server_Socket < 0)
        {
                NeonRTOS_TaskDelete(NULL);
                return;
        }

        int enable = 1;
        setsockopt(HTTP_WebSocket_Server_Socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
        
        /* Bind to the local port */
        ret = bind(HTTP_WebSocket_Server_Socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret < 0)
        {
                close(HTTP_WebSocket_Server_Socket);
                NeonRTOS_TaskDelete(NULL);
                return;
        }
        
        /* Listen to the local connection */
        ret = listen(HTTP_WebSocket_Server_Socket, HTTPD_MAX_CLIENTS);
        if (ret < 0)
        {
                close(HTTP_WebSocket_Server_Socket);
                NeonRTOS_TaskDelete(NULL);
                return;
        }
        
        ret = fcntl(HTTP_WebSocket_Server_Socket, F_GETFL, 0);
        if(ret < 0)
        {
                close(HTTP_WebSocket_Server_Socket);
                NeonRTOS_TaskDelete(NULL);
                return;
        }

        ret = fcntl(HTTP_WebSocket_Server_Socket, F_SETFL, ret | O_NONBLOCK); 
        if(ret < 0)
        {
                close(HTTP_WebSocket_Server_Socket);
                NeonRTOS_TaskDelete(NULL);
                return;
        }

        while(true){
                
                add_HTTP_client = false;

                do{
                        socket_errno = 0;
                        ret = accept(HTTP_WebSocket_Server_Socket, (struct sockaddr *) &current_http_socket_addr, &sin_size);
                        if (ret >= 0) 
                        {
                                UART_Printf("[Server] accept = %d!\n", ret);
                                add_HTTP_client = true;
                        }
                        else{
                                ret = getsockopt(HTTP_WebSocket_Server_Socket, SOL_SOCKET, SO_ERROR, &socket_errno, &socket_errno_optlen);
                                if(ret<0)
                                {
                                        NeonRTOS_Sleep(500);
                                        continue;
                                }
                                if(socket_errno>0)
                                {
                                        UART_Printf("[Server] accept socket_errno = %d!\n", socket_errno);

                                        if(socket_errno != EAGAIN && socket_errno != EWOULDBLOCK)
                                        {
                                                NeonRTOS_Sleep(500);
                                                continue;
                                        }
                                }
                        }
                }while(ret<0 && socket_errno>=0);
                
                if(add_HTTP_client)
                {
                        int8_t client_index = -1;
                        
                        if(GetNumOfHttpdClient()<=HTTPD_MAX_CLIENTS)
                        {
                                current_http_socketID = ret;
                                
                                //clientTaskHandle
                                
                                uint8_t j = 0;
                                UART_Printf("current_http_socketID = %d --> AddHttpClient\n", current_http_socketID);

                                do
                                {
                                        if (GetNumOfHttpdClient() < HTTPD_MAX_CLIENTS)
                                        {
                                                while (j < HTTPD_MAX_CLIENTS)
                                                {
                                                        if(HTTPd_WebSocketd_Client_List[j]==NULL)
                                                        {
                                                                HTTPd_WebSocketd_Client_List[j] = mem_Malloc(sizeof(HTTPd_WebSocked_Client_Connection));
                                                                if(HTTPd_WebSocketd_Client_List[j]==NULL)
                                                                {
                                                                        UART_Printf("current_http_socketID = %d mem_Malloc failed\n", current_http_socketID);
                                                                        client_index = -1;
                                                                        break;
                                                                }
                                                                
                                                                HTTPd_Restore_Variables(j);
                                                                
                                                                if(NeonRTOS_TimerCreate(&HTTPd_WebSocketd_Client_List[j]->connection_timeout_timer, "Connection Timeout", HTTP_TIMEOUT_TIMER_MS, 1, j+HTTPD_TIMER_ID_OFFSET, HTTPd_Connection_Timeout_CB)!=NeonRTOS_OK)
                                                                {
                                                                        mem_Free(HTTPd_WebSocketd_Client_List[j]);
                                                                        HTTPd_WebSocketd_Client_List[j] = NULL;
                                                                        UART_Printf("current_http_socketID = %d NeonRTOS_TimerCreate failed\n", current_http_socketID);
                                                                        client_index = -1;
                                                                        break;
                                                                }
                                                                NeonRTOS_TimerStop(&HTTPd_WebSocketd_Client_List[j]->connection_timeout_timer);
                                                                
                                                                HTTPd_WebSocketd_Client_List[j]->socket_id = current_http_socketID;
                                                                memcpy(&HTTPd_WebSocketd_Client_List[j]->client_socket_addr, &current_http_socket_addr, sizeof(struct sockaddr));
                                                                HTTPd_WebSocketd_Client_List[j]->connection_destruct_flag = false;
                                                                
                                                                HTTPd_WebSocketd_Client_List[j]->data_len = 0;
                                                                HTTPd_WebSocketd_Client_List[j]->isLargeData = false;
                                                                HTTPd_WebSocketd_Client_List[j]->data_buff = NULL;
                                                                client_index = j;
                                                                break;
                                                        }
                                                        j++;
                                                }
                                        }
                                        else
                                        {
                                                UART_Printf("current_http_socketID = %d too many clients wait...\n", current_http_socketID);
                                                NeonRTOS_Sleep(500);
                                        }
                                }while(client_index<0);
                                UART_Printf("AddHttpClient --> client_index = %d\n", client_index);
                        }
                        

                        int socket_errno;
                        const u32_t socket_errno_optlen = sizeof(socket_errno);

                        int keepAlive = 1; //enable keepalive
                        int ret = setsockopt(current_http_socketID, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
                        if(ret < 0)
                        {
                                HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = true;
                                continue;
                        }
                        
                        int enable = 1;
                        int keepIdle = HTTP_TIMEOUT_S; //10s
                        int keepInterval = 5; //10s
                        int keepCount = 5; //retry times
                        
                        ret = setsockopt(current_http_socketID, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
                        if(ret < 0)
                        {
                                HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = true;
                                continue;
                        }

                        ret = setsockopt(current_http_socketID, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
                        if(ret < 0)
                        {
                                HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = true;
                                continue;
                        }
                        
                        ret = setsockopt(current_http_socketID, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
                        if(ret < 0)
                        {
                                HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = true;
                                continue;
                        }
                        
                        ret = setsockopt(current_http_socketID, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
                        if(ret < 0)
                        {
                                HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = true;
                                continue;
                        }
        
#ifdef HTTPD_USE_SSL
                        if((ret = ssl_init(&ssl)) != 0) {
                                UART_Printf(" failed\n  ! ssl_init returned %d\n\n", ret);
                                HTTPd_WebSocketd_Client_List[client_index].client_destruct_flag = true;
                                continue;
                        }

                        ssl_set_endpoint(&ssl, SSL_IS_SERVER);
                        ssl_set_ca_chain(&ssl, server_x509.next, NULL, NULL);
                        ssl_set_authmode(&ssl, SSL_VERIFY_NONE);
                        ssl_set_rng(&ssl, SSL_Random, NULL);
                        ssl_set_bio(&ssl, lwip_read, &HTTPd_WebSocketd_Client_List[client_index].socket_id, lwip_write, &HTTPd_WebSocketd_Client_List[client_index].socket_id);
                        if((ret = ssl_set_own_cert(&ssl, &server_x509, &server_pk)) != 0) {
                                UART_Printf(" failed\n  ! ssl_set_own_cert returned %d\n\n", ret);
                                HTTPd_WebSocketd_Client_List[client_index].client_destruct_flag = true;
                                continue;
                        }
                        
                        if((ret = ssl_handshake(&ssl)) != 0) {
                                UART_Printf(" failed\n  ! ssl_handshake returned %d\n\n", ret);
                                HTTPd_WebSocketd_Client_List[client_index].client_destruct_flag = true;
                                continue;
                        }
#endif

                        int recvBlockTime = HTTPD_RECV_BLOCK_INTERVAL/HTTPD_MAX_CLIENTS;

                        ret = setsockopt(current_http_socketID, SOL_SOCKET, SO_RCVTIMEO, &recvBlockTime, sizeof(recvBlockTime));

                        if(ret < 0)
                        { 
                                HTTPd_WebSocketd_Client_List[client_index]->connection_destruct_flag = true;
                                continue;
                        }
                        
                        struct linger so_linger;
                        so_linger.l_onoff = 1;  // 啟用 linger
                        so_linger.l_linger = 10; // 10 秒內還沒發送完就強制關閉
                        setsockopt(current_http_socketID, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
                        
                        NeonRTOS_TimerStart(&HTTPd_WebSocketd_Client_List[client_index]->connection_timeout_timer);
                        UART_Printf("[%d] httpserver acpt sockfd %d!\n", client_index, current_http_socketID);
                }

                if(GetNumOfHttpdClient()<=0)
                {
                        NeonRTOS_Sleep(500);
                        continue;
                }
            
                for(uint8_t i = 0; i<HTTPD_MAX_CLIENTS; i++)
                {
                        if(HTTPd_WebSocketd_Client_List[i]==NULL)
                        {
                                continue;
                        }

                        int current_http_socketID = HTTPd_WebSocketd_Client_List[i]->socket_id;

                        if(HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag)
                        {
                                UART_Printf("[%d] Client socketID %d close\n", i, current_http_socketID);
                                        
                                if (HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer != NULL)
                                {
                                        NeonRTOS_TimerStop(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);
                                        NeonRTOS_TimerDelete(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);
                                        HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer = NULL;
                                }
                                        
                                if(HTTPd_WebSocketd_Client_List[i]->data_buff != NULL)
                                {
                                        mem_Free(HTTPd_WebSocketd_Client_List[i]->data_buff);

                                        HTTPd_WebSocketd_Client_List[i]->isLargeData = false;
                                        HTTPd_WebSocketd_Client_List[i]->data_buff = NULL;
                                }
                                        
                                mem_Free(HTTPd_WebSocketd_Client_List[i]);
                                HTTPd_WebSocketd_Client_List[i] = NULL;
                                
                                close(current_http_socketID);

                                continue;
                        }

                        if (HTTPd_WebSocketd_Client_List[i]->websocket_client == false)
                        {
                                uint8_t* http_cmd_buf = mem_Malloc(HTTPD_CMD_BUF_SIZE);
                                if (http_cmd_buf == NULL)
                                {
                                        UART_Printf("[%d] cmdbuf malloc err!\n", i);
                                        continue;
                                }
                                
                                memset(http_cmd_buf, 0, HTTPD_CMD_BUF_SIZE);
                                
                                uint16_t recv_len = 0;
                                uint8_t* header_ptr = (uint8_t*)http_cmd_buf;
                                uint8_t* data_ptr = NULL;
                                
                                bool client_close = false;
                                do{
#ifdef HTTPD_USE_SSL
                                        ret = ssl_read(&ssl, &http_cmd_buf[recv_len], 1);
#else
                                        ret = recv(HTTPd_WebSocketd_Client_List[i]->socket_id, &http_cmd_buf[recv_len], 1, 0);
#endif

                                        if (ret < 0)
                                        {
                                                ret = getsockopt(HTTPd_WebSocketd_Client_List[i]->socket_id, SOL_SOCKET, SO_ERROR, &socket_errno, &socket_errno_optlen);
                                                if(ret<0)
                                                {
                                                        client_close = true;
                                                        UART_Printf("[%d] socket err sockfd %d!\n", i, current_http_socketID);
                                                        break;
                                                }
                                                
                                                if (HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag)
                                                {
                                                        UART_Printf("[%d] socket timeout sockfd %d!\n", i, current_http_socketID);
                                                        client_close = true;
                                                        break;
                                                }

                                                if(socket_errno == EAGAIN || socket_errno == EWOULDBLOCK)
                                                {
                                                        UART_Printf("[%d] socket errno: 0 EAGAIN --> Wait!\n", i);
                                                        NeonRTOS_Sleep(100);
                                                        continue;
                                                }
                                                
                                                if(socket_errno==0)
                                                {
                                                        UART_Printf("[%d] socket errno: 0 sockfd %d --> Wait!\n", i, current_http_socketID);
                                                        NeonRTOS_Sleep(100);
                                                        continue;
                                                }

                                                client_close = true;
                                                UART_Printf("[%d] socket errno: %d sockfd %d!\n", i, socket_errno, current_http_socketID);
                                                
                                                break;
                                        }
                                        else if(ret==0)
                                        {
                                                UART_Printf("[%d] socket remote close sockfd %d!\n", i, current_http_socketID);
                                                client_close = true; 
                                                break;  
                                        }
                                        
                                        recv_len+=ret;
                                }
                                while((data_ptr=(uint8_t*)strstr((char*)http_cmd_buf, "\r\n\r\n"))==NULL && recv_len<HTTPD_CMD_BUF_SIZE);

                                if(client_close)
                                {
                                        mem_Free(http_cmd_buf);
                                        
                                        HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                        continue;
                                }
                                
                                if (data_ptr != NULL && recv_len>0)
                                {
                                        NeonRTOS_TimerStop(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);
                                        
                                        data_ptr += strlen("\r\n\r\n");
                                        
                                        HTTPd_Parse_InMsg_Headers(header_ptr, data_ptr - header_ptr, HTTPd_WebSocketd_Client_List[i]);
                                        
                                        if(HTTPd_WebSocketd_Client_List[i]->data_len>0)
                                        {
                                                if(HTTPd_WebSocketd_Client_List[i]->data_len>4096)
                                                {
                                                        HTTPd_WebSocketd_Client_List[i]->isLargeData = true;
                                                }
                                                else
                                                {
                                                        HTTPd_WebSocketd_Client_List[i]->data_buff = mem_Malloc(sizeof(uint8_t)*HTTPd_WebSocketd_Client_List[i]->data_len);
                                                        
                                                        if(HTTPd_WebSocketd_Client_List[i]->data_buff==NULL)
                                                        {
                                                                mem_Free(http_cmd_buf);
                                                                
                                                                UART_Printf("[%d] err request data too large\n", i);
                                                                continue;
                                                        }

                                                        uint16_t cgi_recv_len = 0;
                                                        bool cgi_client_close = false;
                                                        do{
#ifdef HTTPD_USE_SSL
                                                                ret = ssl_read(&ssl, &HTTPd_WebSocketd_Client_List[i]->data_buff[cgi_recv_len], 1);
#else
                                                                ret = recv(HTTPd_WebSocketd_Client_List[i]->socket_id, &HTTPd_WebSocketd_Client_List[i]->data_buff[cgi_recv_len], HTTPd_WebSocketd_Client_List[i]->data_len, 0);
#endif

                                                                if (ret < 0)
                                                                {
                                                                        ret = getsockopt(HTTPd_WebSocketd_Client_List[i]->socket_id, SOL_SOCKET, SO_ERROR, &socket_errno, &socket_errno_optlen);
                                                                        if(ret<0)
                                                                        {
                                                                                cgi_client_close = true;
                                                                                UART_Printf("[%d] cgi socket err sockfd %d!\n", i, current_http_socketID);
                                                                                break;
                                                                        }
                                                                        
                                                                        if (HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag)
                                                                        {
                                                                                UART_Printf("[%d] cgi socket timeout sockfd %d!\n", i, current_http_socketID);
                                                                                cgi_client_close = true;
                                                                                break;
                                                                        }

                                                                        if(socket_errno == EAGAIN || socket_errno == EWOULDBLOCK)
                                                                        {
                                                                                UART_Printf("[%d] cgi socket errno: 0 EAGAIN --> Wait!\n", i);
                                                                                NeonRTOS_Sleep(100);
                                                                                continue;
                                                                        }
                                                                        
                                                                        if(socket_errno==0)
                                                                        {
                                                                                UART_Printf("[%d] cgi socket errno: 0 sockfd %d --> Wait!\n", i, current_http_socketID);
                                                                                NeonRTOS_Sleep(100);
                                                                                continue;
                                                                        }

                                                                        cgi_client_close = true;
                                                                        UART_Printf("[%d] cgi socket errno: %d sockfd %d!\n", i, socket_errno, current_http_socketID);
                                                                        
                                                                        break;
                                                                }
                                                                else if(ret==0)
                                                                {
                                                                        UART_Printf("[%d] cgi socket remote close sockfd %d!\n", i, current_http_socketID);
                                                                        cgi_client_close = true; 
                                                                        break;  
                                                                }
                                                                
                                                                cgi_recv_len+=ret;
                                                        }
                                                        while(cgi_recv_len<HTTPd_WebSocketd_Client_List[i]->data_len);

                                                        if(cgi_client_close)
                                                        {
                                                                if(HTTPd_WebSocketd_Client_List[i]->data_buff!=NULL)
                                                                {
                                                                        UART_Printf("[%d] cgi HTTPd_WebSocketd_Client_List[%d]->data_buff!=NULL\n", i, i);

                                                                        mem_Free(HTTPd_WebSocketd_Client_List[i]->data_buff);
                                                                        
                                                                        HTTPd_WebSocketd_Client_List[i]->isLargeData = false;
                                                                        HTTPd_WebSocketd_Client_List[i]->data_buff = NULL;
                                                                }

                                                                mem_Free(http_cmd_buf);
                                                                
                                                                UART_Printf("[%d] cgi client_close sockfd %d!\n", i, current_http_socketID);

                                                                HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                                                continue;
                                                        }
                                                }
                                        }
                                        
                                        UART_Printf("[%d] Request URL = %s\n", i, HTTPd_WebSocketd_Client_List[i]->url);
                                        
                                        int r = HTTPD_CGI_DONE;
                                        
                                        switch (HTTPd_WebSocketd_Client_List[i]->requestType)
                                        {
                                        case HTTPd_Method_POST:
                                                if (HTTPd_WebSocketd_Client_List[i]->data_len > 0)
                                                {
                                                        r = HTTPd_Process_POST_Request(HTTPd_WebSocketd_Client_List[i]);
                                                }
                                                break;
                                        case HTTPd_Method_GET:
                                                r = HTTPd_Process_GET_Request(HTTPd_WebSocketd_Client_List[i]);
                                                break;
                                        case HTTPd_Method_PUT:
                                                if (HTTPd_WebSocketd_Client_List[i]->data_len > 0)
                                                {
                                                        r = HTTPd_Process_PUT_Request(HTTPd_WebSocketd_Client_List[i]);
                                                }
                                                break;
                                        case HTTPd_Method_DELETE:
                                                r = HTTPd_Process_DELETE_Request(HTTPd_WebSocketd_Client_List[i]);
                                                break;
                                        }
                                        
                                        if (HTTPd_WebSocketd_Client_List[i]->url != NULL)
                                        {
                                                if(HTTPd_WebSocketd_Client_List[i]->cgi != NULL)
                                                {
                                                        ret = HTTPd_WebSocketd_Client_List[i]->cgi(HTTPd_WebSocketd_Client_List[i]); //Execute cgi fn.
                                                        if (ret == HTTPD_CGI_DONE) {
                                                                UART_Printf("[%d] HTTPD_CGI_DONE\n", i);
                                                        }
                                                        if (ret == HTTPD_CGI_NOTFOUND || ret == HTTPD_CGI_AUTHENTICATED) {
                                                                UART_Printf("[%d] ERROR! CGI fn returns code %d after sending data! Bad CGI!\n", i, ret);
                                                        }
                                                }
                                        }

                                        if(HTTPd_WebSocketd_Client_List[i]->data_buff!=NULL)
                                        {
                                                UART_Printf("[%d] HTTPd_WebSocketd_Client_List[i]->data_buff!=NULL\n", i);

                                                mem_Free(HTTPd_WebSocketd_Client_List[i]->data_buff);
                                                
                                                HTTPd_WebSocketd_Client_List[i]->isLargeData = false;
                                                HTTPd_WebSocketd_Client_List[i]->data_buff = NULL;
                                        }

                                        if(r==HTTPD_CGI_DONE)
                                        {
                                                UART_Printf("[%d] URL = %s CGI DONE\n", i, HTTPd_WebSocketd_Client_List[i]->url);
                                                
                                                NeonRTOS_TimerStop(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);
                                                
                                                mem_Free(http_cmd_buf);
                                                
                                                HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                                continue;
                                        }
                                }

                                mem_Free(http_cmd_buf);
                        }
                        else
                        {
                                HTTPd_WSFrame frame;
                                
                                if(WebSocketServer_ReceiveWsFrame(HTTPd_WebSocketd_Client_List[i], &frame)<0)
                                {
                                        NeonRTOS_Sleep(100);
                                        continue;
                                }
                                
                                if (frame.payloadLength == 0)
                                {
                                        NeonRTOS_Sleep(100);
                                        continue;
                                }
                                
                                uint16_t rec_dat_len = frame.payloadLength;
                                
                                if (rec_dat_len >= HTTP_WS_SERVER_DAT_BUF_SIZE)
                                {
                                        rec_dat_len = HTTP_WS_SERVER_DAT_BUF_SIZE;
                                }
                                
                                uint8_t* WebSocketServerDataBuf = mem_Malloc(rec_dat_len + 1);
                                if (WebSocketServerDataBuf == NULL) {
                                        continue;
                                }
                        
#ifdef HTTPD_USE_SSL
                                ret = ssl_read(&ssl, WebSocketServerDataBuf, rec_dat_len);
#else   
                                ret = recv(HTTPd_WebSocketd_Client_List[i]->socket_id, WebSocketServerDataBuf, rec_dat_len, 0);
#endif
                                
                                if (ret < 0)
                                { 
                                        ret = getsockopt(HTTPd_WebSocketd_Client_List[i]->socket_id, SOL_SOCKET, SO_ERROR, &socket_errno, &socket_errno_optlen);
                                        if(ret<0)
                                        {
                                                mem_Free(WebSocketServerDataBuf);
                                                UART_Printf("[%d] ws socket err sockfd %d!\n", i, current_http_socketID);
                                                
                                                HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                                continue;
                                        }

                                        if(socket_errno == EAGAIN || socket_errno == EWOULDBLOCK)
                                        {
                                                mem_Free(WebSocketServerDataBuf);
                                                if (HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag)
                                                {
                                                        UART_Printf("[%d] ws timeout sockfd %d!\n", i, current_http_socketID);
                                                }
                                                continue;
                                        }
                                        
                                        mem_Free(WebSocketServerDataBuf);
                                        
                                        UART_Printf("[%d] ws socket err:%d sockfd %d!\n", i, socket_errno, current_http_socketID);
                                        break;
                                }
                                else if(ret==0)
                                {
                                        mem_Free(WebSocketServerDataBuf);
                                        
                                        UART_Printf("[%d] ws socket remote close sockfd %d!\n", i, current_http_socketID);
                                        
                                        HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                        continue;
                                }
                                
                                if (ret > HTTP_WS_SERVER_DAT_BUF_SIZE)
                                { 
                                        mem_Free(WebSocketServerDataBuf);

                                        HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                        continue;
                                }
                                
                                WebSocketServerDataBuf[ret] = 0;
                                
                                NeonRTOS_TimerStop(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);
                                
                                if (frame.isMasked)
                                {
                                        unmaskWsPayload((char*)WebSocketServerDataBuf, rec_dat_len, frame.maskingKey);
                                }
                                        
                                if (frame.opcode == OPCODE_PING)
                                {
                                        //sendWsMessage(wsConnection, frame.payloadData, frame.payloadLength, FLAG_FIN | OPCODE_PONG);
                                        
                                        WebsocketServer_SendMessage(HTTPd_WebSocketd_Client_List[i], (char*)WebSocketServerDataBuf, rec_dat_len, WEBSOCK_FLAG_PONG);
                                        
                                        /*restart the sock handle watchout timer */
                                        NeonRTOS_TimerReStart(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);

                                        mem_Free(WebSocketServerDataBuf);

                                        continue;
                                }
                                else if (frame.opcode == OPCODE_CLOSE)
                                {
                                        // shut down the connection gracefully
                                        mem_Free(WebSocketServerDataBuf);
                                        
                                        UART_Printf("[%d] ws close sockfd %d!\n", i, current_http_socketID);
                                        
                                        HTTPd_WebSocketd_Client_List[i]->connection_destruct_flag = true;
                                        continue;
                                }
                                else if (rec_dat_len > 0)
                                {
                                        HTTPd_WebSocketServerOnMessage(HTTPd_WebSocketd_Client_List[i], &frame, WebSocketServerDataBuf);
                                }
                                
                                mem_Free(WebSocketServerDataBuf);
                                
                                /*restart the sock handle watchout timer */
                                NeonRTOS_TimerReStart(&HTTPd_WebSocketd_Client_List[i]->connection_timeout_timer);
                        }
                }
        }
    
        if(HTTP_WebSocket_Server_Socket>=0)
        {
                close(HTTP_WebSocket_Server_Socket);
                HTTP_WebSocket_Server_Socket = -1;
        }
    
#ifdef HTTPD_USE_SSL
        x509_crt_free(&server_x509);
        pk_free(&server_pk);
#endif
        
        NeonRTOS_TaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_webserver_start
 * Description  : Httpd initialization routine. Call this to kick off webserver functionality.
 * Parameters   : noe
 * Returns      : none
*******************************************************************************/
int HTTPd_Init()
{
    if(NeonRtFsInit()!=NEONRTFS_INIT_RESULT_OK)
    {
          UART_Printf("HTTP Server No Web Image in Flash\n");
    }
    
    if(NeonRTOS_TaskCreate(HTTP_Server_Task, (signed const char *)"httpd", HTTP_STACK_SIZE, NULL, HTTP_TASK_PRIORITY, NULL)!=NeonRTOS_OK)
    {
          return -1;
    }
    
    return 0;
}
