
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "HTTPd_def.h"

const char HTTP_Host_Name[] = "NeonRT.config.com";

const char HTTP_Status_100[] = "Continue";
const char HTTP_Status_101[] = "Switching Protocols";
const char HTTP_Status_102[] = "Processing";

const char HTTP_Status_200[] = "OK";
const char HTTP_Status_201[] = "Created";
const char HTTP_Status_202[] = "Accepted";
const char HTTP_Status_203[] = "Non - Authoritative Information";
const char HTTP_Status_204[] = "No Content";
const char HTTP_Status_205[] = "Reset Content";
const char HTTP_Status_206[] = "Partial Content";
const char HTTP_Status_207[] = "Multi - Status";

const char HTTP_Status_300[] = "Multiple Choices";
const char HTTP_Status_301[] = "Moved Permanently";
const char HTTP_Status_302[] = "Found";
const char HTTP_Status_303[] = "See Other";
const char HTTP_Status_304[] = "Not Modified";
const char HTTP_Status_305[] = "Use Proxy";
const char HTTP_Status_306[] = "Switch Proxy";
const char HTTP_Status_307[] = "Temporary Redirect";

const char HTTP_Status_400[] = "Bad Request";
const char HTTP_Status_401[] = "Unauthorized";
const char HTTP_Status_402[] = "Payment Required";
const char HTTP_Status_403[] = "Forbidden";
const char HTTP_Status_404[] = "Not Found";
const char HTTP_Status_405[] = "Method Not Allowed";
const char HTTP_Status_406[] = "Not Acceptable";
const char HTTP_Status_407[] = "Proxy Authentication Required";
const char HTTP_Status_408[] = "Request Timeout";
const char HTTP_Status_409[] = "Conflict";
const char HTTP_Status_410[] = "Gone";
const char HTTP_Status_411[] = "Length Required";
const char HTTP_Status_412[] = "Precondition Failed";
const char HTTP_Status_413[] = "Request Entity Too Large";
const char HTTP_Status_414[] = "Request - URI Too Long";
const char HTTP_Status_415[] = "Unsupported Media Type";
const char HTTP_Status_416[] = "Requested Range Not Satisfiable";
const char HTTP_Status_417[] = "Expectation Failed";
const char HTTP_Status_418[] = "I'm a teapot";
const char HTTP_Status_421[] = "There are too many connections from your internet address";
const char HTTP_Status_422[] = "Unprocessable Entity";
const char HTTP_Status_423[] = "Locked";
const char HTTP_Status_424[] = "Failed Dependency";
const char HTTP_Status_425[] = "Unordered Collection";
const char HTTP_Status_426[] = "Upgrade Required";
const char HTTP_Status_449[] = "Retry With";
const char HTTP_Status_451[] = "Unavailable For Legal Reasons";

const char HTTP_Status_500[] = "Internal Server Error";
const char HTTP_Status_501[] = "Not Implemented";
const char HTTP_Status_502[] = "Bad Gateway";
const char HTTP_Status_503[] = "Service Unavailable";
const char HTTP_Status_504[] = "Gateway Timeout";
const char HTTP_Status_505[] = "HTTP Version Not Supported";
const char HTTP_Status_506[] = "Variant Also Negotiates";
const char HTTP_Status_507[] = "Insufficient Storage";
const char HTTP_Status_509[] = "Bandwidth Limit Exceeded";
const char HTTP_Status_510[] = "Not Extended";

// HTTP method line strings
const char    HTTP_Method_POST[]              = "POST";
const char    HTTP_Method_GET[]               = "GET";
const char    HTTP_Method_PUT[]               = "PUT";
const char    HTTP_Method_DELETE[]            = "DELETE";
const char    HTTP_Version_1P0[]              = "HTTP/1.0";
const char    HTTP_Version_1P1[]              = "HTTP/1.1";

// HTTP request/response header line strings
const char    HTTP_Content_Type[]                = "Content-Type";
const char    HTTP_Content_Disposition[]         = "Content-Disposition";
const char    HTTP_Upgrade_Increase_Request[]    = "Upgrade-Insecure-Requests";
const char    HTTP_Content_Length[]              = "Content-Length";
const char    HTTP_Accept_Encoding[]             = "Accept-Encoding";
const char    HTTP_Access_Control_Allow_Origin[] = "Access-Control-Allow-Origin";
const char    HTTP_Authorization[]               = "Authorization";
const char    HTTP_Connection[]                  = "Connection";
const char    HTTP_Gzip[]                        = "gzip";
const char    HTTP_Close[]                       = "Close";
const char    HTTP_Close_LowerCase[]             = "close";
const char    HTTP_Keep_Alive[]                  = "Keep-Alive";
const char    HTTP_Keep_Alive_LowerCase[]        = "keep-alive";
const char    HTTP_Upgrade []                    = "Upgrade";
const char    HTTP_Upgrade_LowerCase[]           = "upgrade";
const char    HTTP_Location[]                    = "Location";
const char    HTTP_Content_Encoding[]            = "Content-Encoding";
const char    HTTP_Server[]                      = "Server";
const char    HTTP_Host[]                        = "Host";

const char    WebSocket_UUID[]                   = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const char    WebSocket_Version[]                = "13";
const char    WebSocket_Identifier_Upper[]       = "Websocket";
const char    WebSocket_Identifier_Lower[]       = "websocket";
const char    WebSocket_Key_Identifier[]         = "Sec-WebSocket-Key";
const char    WebSocket_Accept_Identifier[]      = "Sec-WebSocket-Accept";
const char    WebSocket_Version_Identifier[]     = "Sec-WebSocket-Version";

//The mappings from file extensions to mime types. If you need an extra mime type,
//add it here.
const MimeMap mimeTypes [] = {
    {"htm", "text/htm"},
    {"html", "text/html"},
    {"css", "text/css"},
    {"js", "text/javascript"},
    {"txt", "text/plain"},
    {"csv",  "text/csv"},
    {"ico",  "image/x-icon"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"svg", "image/svg+xml"},
    {"xml", "text/xml"},
    {"json", "application/json"},
    {NULL, "text/html"}, //default value
};
