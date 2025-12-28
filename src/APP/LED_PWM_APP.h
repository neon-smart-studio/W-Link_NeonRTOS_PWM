
#ifndef LED_PWM_APP_H
#define LED_PWM_APP_H

#include "NeonAppInterface.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define LED_PWM_MIN  0
#define LED_PWM_MAX  255

#define APP_TOPIC_NAME "LED_PWM"
#define DEVICE_TYPE_WITH_UNDERLINE_UPPER "LED_PWM"

int LED_PWM_Do_Report(bool report_via_websocket, bool report_via_mqtt);

int Process_LED_PWM_APP_Incoming_JSON_POST_Msg(App_Interface_Protocol protocol, cJSON *in_json);
int Process_LED_PWM_APP_Incoming_JSON_GET_Msg(App_Interface_Protocol protocol, cJSON *in_json, cJSON *rsp_json);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif //LED_PWM_APP_H