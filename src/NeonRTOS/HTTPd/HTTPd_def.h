
#ifndef HTTPD_DEF_H
#define HTTPD_DEF_H

/* from IEEE RFC6455 sec 5.2
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
*/

#define FLAG_FIN (1 << 7)
#define FLAG_RSV1 (1 << 6)
#define FLAG_RSV2 (1 << 5)
#define FLAG_RSV3 (1 << 4)

#define OPCODE_CONTINUE 0x0
#define OPCODE_TEXT 0x1
#define OPCODE_BINARY 0x2
#define OPCODE_CLOSE 0x8
#define OPCODE_PING 0x9
#define OPCODE_PONG 0xA

#define FLAGS_MASK ((uint8_t)0xF0)
#define OPCODE_MASK ((uint8_t)0x0F)
#define IS_MASKED ((uint8_t)(1<<7))
#define PAYLOAD_MASK ((uint8_t)0x7F)

#define STATUS_OPEN 0
#define STATUS_CLOSED 1
#define STATUS_UNINITIALISED 2

#define CLOSE_MESSAGE {FLAG_FIN | OPCODE_CLOSE, IS_MASKED /* + payload = 0*/, 0 /* + masking key*/}
#define CLOSE_MESSAGE_LENGTH 3
 
//Struct to keep extension->mime data in
typedef struct {
    const char *ext;
    const char *mimetype;
} MimeMap;

typedef enum HTTPd_Method_t
{
    HTTPd_Method_POST = 1,
    HTTPd_Method_GET = 2,
    HTTPd_Method_PUT = 3,
    HTTPd_Method_DELETE = 4,
    HTTPd_Method_UNKNOWN = 0,
}HTTPd_Method;

typedef enum HTTPd_Connection_Mode_t
{
    HTTPd_Connection_Mode_Close = 0,
    HTTPd_Connection_Mode_Keep_Alive = 1,
    HTTPd_Connection_Mode_Upgrade = 2,
    HTTPd_Connection_Mode_MAX = 3,
}HTTPd_Connection_Mode;

extern const MimeMap mimeTypes [];

extern const char HTTP_Host_Name[];

extern const char HTTP_Status_100[];
extern const char HTTP_Status_101[];
extern const char HTTP_Status_102[];

extern const char HTTP_Status_200[];
extern const char HTTP_Status_201[];
extern const char HTTP_Status_202[];
extern const char HTTP_Status_203[];
extern const char HTTP_Status_204[];
extern const char HTTP_Status_205[];
extern const char HTTP_Status_206[];
extern const char HTTP_Status_207[];

extern const char HTTP_Status_300[];
extern const char HTTP_Status_301[];
extern const char HTTP_Status_302[];
extern const char HTTP_Status_303[];
extern const char HTTP_Status_304[];
extern const char HTTP_Status_305[];
extern const char HTTP_Status_306[];
extern const char HTTP_Status_307[];

extern const char HTTP_Status_400[];
extern const char HTTP_Status_401[];
extern const char HTTP_Status_402[];
extern const char HTTP_Status_403[];
extern const char HTTP_Status_404[];
extern const char HTTP_Status_405[];
extern const char HTTP_Status_406[];
extern const char HTTP_Status_407[];
extern const char HTTP_Status_408[];
extern const char HTTP_Status_409[];
extern const char HTTP_Status_410[];
extern const char HTTP_Status_411[];
extern const char HTTP_Status_412[];
extern const char HTTP_Status_413[];
extern const char HTTP_Status_414[];
extern const char HTTP_Status_415[];
extern const char HTTP_Status_416[];
extern const char HTTP_Status_417[];
extern const char HTTP_Status_418[];
extern const char HTTP_Status_421[];
extern const char HTTP_Status_422[];
extern const char HTTP_Status_423[];
extern const char HTTP_Status_424[];
extern const char HTTP_Status_425[];
extern const char HTTP_Status_426[];
extern const char HTTP_Status_449[];
extern const char HTTP_Status_451[];

extern const char HTTP_Status_500[];
extern const char HTTP_Status_501[];
extern const char HTTP_Status_502[];
extern const char HTTP_Status_503[];
extern const char HTTP_Status_504[];
extern const char HTTP_Status_505[];
extern const char HTTP_Status_506[];
extern const char HTTP_Status_507[];
extern const char HTTP_Status_509[];
extern const char HTTP_Status_510[];

// HTTP method line strings
extern const char HTTP_Method_POST[];
extern const char HTTP_Method_GET[];
extern const char HTTP_Method_PUT[];
extern const char HTTP_Method_DELETE[];
extern const char HTTP_Version_1P0[];
extern const char HTTP_Version_1P1[];

// HTTP request/response header line strings
extern const char HTTP_Content_Type[];
extern const char HTTP_Content_Disposition[];
extern const char HTTP_Upgrade_Increase_Request[];
extern const char HTTP_Content_Length[];
extern const char HTTP_Accept_Encoding[];
extern const char HTTP_Access_Control_Allow_Origin[];
extern const char HTTP_Authorization[];
extern const char HTTP_Connection[];
extern const char HTTP_Gzip[];
extern const char HTTP_Close[];
extern const char HTTP_Close_LowerCase[];
extern const char HTTP_Keep_Alive[];
extern const char HTTP_Keep_Alive_LowerCase[];
extern const char HTTP_Upgrade [];
extern const char HTTP_Upgrade_LowerCase[];
extern const char HTTP_Location[];
extern const char HTTP_Content_Encoding[];
extern const char HTTP_Server[];
extern const char HTTP_Host[];

extern const char WebSocket_UUID[];
extern const char WebSocket_Version[];
extern const char WebSocket_Identifier_Upper[];
extern const char WebSocket_Identifier_Lower[];
extern const char WebSocket_Key_Identifier[];
extern const char WebSocket_Accept_Identifier[];
extern const char WebSocket_Version_Identifier[];

#endif //HTTPD_DEF_H