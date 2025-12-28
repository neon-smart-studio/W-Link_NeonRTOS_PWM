
#include <stdbool.h>
#include <stdint.h>

#include "time.h"

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#include "NeonRTOS.h"

#include "cJSON/cJSON.h"

#include "HTTPd/HTTPd.h"

#include "GPIO/GPIO.h"
#include "PWM/PWM.h"

#include "NeonAppInterface.h"

#include "LED_PWM_API.h"
#include "LED_PWM_APP.h"

int LED_PWM_Get_All_LED_Status(cJSON *pcjson)
{
	cJSON * pSubJson_main = cJSON_CreateObject();
	if (NULL == pSubJson_main)
	{
		return -1;
	}
	
	cJSON * pSubJson_individual_lst = cJSON_CreateArray();
	if (NULL == pSubJson_individual_lst)
	{
		return -1;
	}
        
        cJSON_AddNumberToObject(pcjson, "num_of_leds", MAX_NUM_OF_LED_PWM_SW);
	cJSON_AddItemToObject(pcjson, "all_led_status", pSubJson_individual_lst);
	
	cJSON * pSubJson_individual[MAX_NUM_OF_LED_PWM_SW];
		
	for (uint8_t i = 0; i < MAX_NUM_OF_LED_PWM_SW; i++)
	{
		pSubJson_individual[i] = cJSON_CreateObject();
		if (NULL == pSubJson_individual[i])
		{
			return -1;
		}
		cJSON_AddItemToArray(pSubJson_individual_lst, pSubJson_individual[i]);
		cJSON_AddNumberToObject(pSubJson_individual[i], "led_index", i);
		cJSON_AddBoolToObject(pSubJson_individual[i], "on_off", All_LED_Status[i].on_off);
		cJSON_AddNumberToObject(pSubJson_individual[i], "level", All_LED_Status[i].level/10);
	}
	
	return 0;
}

int LED_PWM_CGI_Root(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int status = HTTPD_CGI_DONE;
    
    if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            LED_PWM_Get_All_LED_Status(rsp_json);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int LED_PWM_CGI_LED_Num(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int status = HTTPD_CGI_DONE;
    
    if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            cJSON_AddNumberToObject(rsp_json, "num_of_leds", MAX_NUM_OF_LED_PWM_SW);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int LED_PWM_CGI_LED_Any_Number(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int led_index;
    
    const char* req_url = HTTPd_Get_CGI_Request_URL(connData);
    const char *rest = NULL;      // 指向 "/LED_PWM/" 之後的位置

    if (HTTPd_CGI_PathStartsWith(req_url, "/LED_PWM/", &rest)<0)
    {
        return HTTPD_CGI_NOTFOUND;
    }

    led_index = HTTPd_CGI_ParseUint(rest);
    
    if(led_index<0 || led_index>=MAX_NUM_OF_LED_PWM_SW)
    {
            return HTTPD_CGI_NOTFOUND;
    }
    
    int status = HTTPD_CGI_DONE;
    
    if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_POST)
    {
            const uint8_t* req_data = HTTPd_Get_CGI_Request_Data(connData);
            if(req_data==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            cJSON* req_json = cJSON_Parse((const char*)req_data);
            if (req_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            
            bool sw_on_off = false;
            uint8_t sw_level = 0;
            
            cJSON* on_off = NULL;
            cJSON* level = NULL;
            sw_on_off = true;

            on_off = cJSON_GetObjectItem(req_json, "on_off");
            if (on_off == NULL)
            {
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            level = cJSON_GetObjectItem(req_json, "level");
            if (level == NULL)
            {
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
        
            if(on_off->type == cJSON_True)
            {
                    sw_on_off = true;
            }
            else if(on_off->type == cJSON_False)
            {
                    sw_on_off = false;
            }
            else{
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }

            if(level->type == cJSON_Number)
            {
                    sw_level = level->valueint;
            }
            else{
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            LED_PWM_Set_LED_On_Off_Status(led_index, sw_on_off);
            LED_PWM_Set_LED_Level_Status(led_index, sw_level * 10);
            
            cJSON_AddBoolToObject(rsp_json, "led_index", led_index);
            cJSON_AddBoolToObject(rsp_json, "on_off", sw_on_off);
            cJSON_AddNumberToObject(rsp_json, "level", sw_level);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            cJSON_AddNumberToObject(rsp_json, "led_index", led_index);
            cJSON_AddBoolToObject(rsp_json, "on_off", All_LED_Status[led_index].on_off);
            cJSON_AddNumberToObject(rsp_json, "level", All_LED_Status[led_index].level/10);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int LED_PWM_CGI_LED_OnOff_Any_Number(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int led_index;
    
    const char* req_url = HTTPd_Get_CGI_Request_URL(connData);
    const char *rest = NULL;      // 指向 "/LED_PWM/" 之後的位置

    if (HTTPd_CGI_PathStartsWith(req_url, "/LED_PWM/", &rest)<0)
    {
        return HTTPD_CGI_NOTFOUND;
    }

    led_index = HTTPd_CGI_ParseUint(rest);
    
    if(led_index<0 || led_index>=MAX_NUM_OF_LED_PWM_SW)
    {
            return HTTPD_CGI_NOTFOUND;
    }
    
    int status = HTTPD_CGI_DONE;
    
    if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_POST)
    {
            const uint8_t* req_data = HTTPd_Get_CGI_Request_Data(connData);
            if(req_data==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            cJSON* req_json = cJSON_Parse((const char*)req_data);
            if (req_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            
            bool sw_on_off = false;
            
            cJSON* on_off = cJSON_GetObjectItem(req_json, "on_off");
            if (on_off == NULL)
            {
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            if(on_off->type == cJSON_True)
            {
                    sw_on_off = true;
            }
            else if(on_off->type == cJSON_False)
            {
                    sw_on_off = false;
            }
            else{
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            LED_PWM_Set_LED_On_Off_Status(led_index, sw_on_off);
            
            cJSON_AddBoolToObject(rsp_json, "led_index", led_index);
            cJSON_AddBoolToObject(rsp_json, "on_off", sw_on_off);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            cJSON_AddNumberToObject(rsp_json, "led_index", led_index);
            cJSON_AddBoolToObject(rsp_json, "on_off", All_LED_Status[led_index].on_off);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int LED_PWM_CGI_LED_Level_Any_Number(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int led_index;
    
    const char* req_url = HTTPd_Get_CGI_Request_URL(connData);
    const char *rest = NULL;      // 指向 "/LED_PWM/" 之後的位置

    if (HTTPd_CGI_PathStartsWith(req_url, "/LED_PWM/", &rest)<0)
    {
        return HTTPD_CGI_NOTFOUND;
    }

    led_index = HTTPd_CGI_ParseUint(rest);
    
    if(led_index<0 || led_index>=MAX_NUM_OF_LED_PWM_SW)
    {
            return HTTPD_CGI_NOTFOUND;
    }
    
    int status = HTTPD_CGI_DONE;
    
    if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_POST)
    {
            const uint8_t* req_data = HTTPd_Get_CGI_Request_Data(connData);
            if(req_data==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            cJSON* req_json = cJSON_Parse((const char*)req_data);
            if (req_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            
            cJSON* level = cJSON_GetObjectItem(req_json, "level");
            if (level == NULL)
            {
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            bool sw_level = 0;
            
            if(level->type == cJSON_Number)
            {
                    sw_level = level->valueint;
            }
            else{
                    cJSON_Delete(req_json);
                    return HTTPd_Send_CGI_Response(connData, 400, "txt", NULL, 0);
            }
            
            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            LED_PWM_Set_LED_Level_Status(led_index, sw_level*10);
            
            cJSON_AddBoolToObject(rsp_json, "led_index", led_index);
            cJSON_AddBoolToObject(rsp_json, "level", sw_level);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            cJSON_AddNumberToObject(rsp_json, "led_index", led_index);
            cJSON_AddNumberToObject(rsp_json, "level", All_LED_Status[led_index].level/10);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int Process_LED_PWM_APP_JSON_POST_Commands(cJSON *in_json)
{
	int8_t status = 0;
	cJSON * pJson_cmd;
	cJSON * pJson_on_off_status;
	cJSON * pJson_level_status;
	cJSON * pJson_led_index;
	
	pJson_cmd = cJSON_GetObjectItem(in_json, "command");
	if (NULL != pJson_cmd && pJson_cmd->type == cJSON_String) {
		if (strcmp(pJson_cmd->valuestring, "set individual led on/off state") == 0)
		{
			pJson_on_off_status = cJSON_GetObjectItem(in_json, "on_off");
			pJson_led_index = cJSON_GetObjectItem(in_json, "led_index");
			if (NULL == pJson_on_off_status || NULL == pJson_led_index || pJson_led_index->type != cJSON_Number) {return -1;}

			if(pJson_led_index->valueint < MAX_NUM_OF_LED_PWM_SW && pJson_led_index->valueint >= 0)
			{
				if (pJson_on_off_status->type == cJSON_True)
				{
					LED_PWM_Set_LED_On_Off_Status(pJson_led_index->valueint, true);
				}
				else if (pJson_on_off_status->type == cJSON_False)
				{
					LED_PWM_Set_LED_On_Off_Status(pJson_led_index->valueint, false);
				}
				else
				{
					status = -1;
				}
			}
			else
			{
				status = -1;
			}
		}
		if (strcmp(pJson_cmd->valuestring, "set individual led level state") == 0)
		{
			pJson_level_status = cJSON_GetObjectItem(in_json, "level");
			pJson_led_index = cJSON_GetObjectItem(in_json, "led_index");
			if (NULL == pJson_level_status || NULL == pJson_led_index || pJson_led_index->type != cJSON_Number || pJson_level_status->type != cJSON_Number) {return -1;}

			if(pJson_led_index->valueint < MAX_NUM_OF_LED_PWM_SW && pJson_led_index->valueint >= 0 && pJson_level_status->valueint <= 100 && pJson_level_status->valueint >= 0)
			{
				LED_PWM_Set_LED_Level_Status(pJson_led_index->valueint, pJson_level_status->valueint * 10);
			}
			else
			{
				status = -1;
			}
		}
		if (strcmp(pJson_cmd->valuestring, "toggle individual switch on/off state") == 0)
		{
			pJson_led_index = cJSON_GetObjectItem(in_json, "led_index");
			if (NULL == pJson_led_index || pJson_led_index->type != cJSON_Number) {return -1;}

			if(pJson_led_index->valueint < MAX_NUM_OF_LED_PWM_SW && pJson_led_index->valueint >= 0)
			{
                                LED_PWM_Toggle_LED_On_Off_Status(pJson_led_index->valueint);
			}
			else
			{
				status = -1;
			}
		}
	}
	
	return status;
}

int Process_LED_PWM_APP_JSON_GET_Commands(cJSON *in_json, cJSON *rsp_json)
{
	int8_t status = 0;
	cJSON* cmd = cJSON_GetObjectItem(in_json, "command");
	cJSON* pJson_led_index;
	
	if (cmd != NULL && cmd->type == cJSON_String)
	{
		if (strcmp(cmd->valuestring, "get num of leds") == 0)
		{
			status = 0;
			cJSON_AddStringToObject(rsp_json, "command", "get num of leds");
			cJSON_AddNumberToObject(rsp_json, "num_of_leds", MAX_NUM_OF_LED_PWM_SW);
		}
		if (strcmp(cmd->valuestring, "get individual led status") == 0)
		{
			pJson_led_index = cJSON_GetObjectItem(in_json, "led_index");
			cJSON_AddStringToObject(rsp_json, "command", "get individual led tatus");
			if (pJson_led_index != NULL && pJson_led_index->valueint < MAX_NUM_OF_LED_PWM_SW && pJson_led_index->type == cJSON_Number)
			{
				status = 0;
				cJSON_AddNumberToObject(rsp_json, "led_index", pJson_led_index->valueint);
				cJSON_AddBoolToObject(rsp_json, "on_off", All_LED_Status[pJson_led_index->valueint].on_off);
				cJSON_AddNumberToObject(rsp_json, "level", All_LED_Status[pJson_led_index->valueint].level/10);
			}
		}
		if (strcmp(cmd->valuestring, "get all led status") == 0)
		{
			status = LED_PWM_Get_All_LED_Status(rsp_json);
			cJSON_AddStringToObject(rsp_json, "command", "get all led status");
		}
	}
	return status;
}

int LED_PWM_Do_Report(bool report_via_websocket, bool report_via_mqtt)
{
        cJSON* brocastJSON_dat = cJSON_CreateObject();
        if (brocastJSON_dat != NULL)
        {
                cJSON_AddStringToObject(brocastJSON_dat, "method", "POST");
                cJSON_AddStringToObject(brocastJSON_dat, "topic", APP_TOPIC_NAME);
                
                cJSON_AddStringToObject(brocastJSON_dat, "command", "report leds status change");
        
                LED_PWM_Get_All_LED_Status(brocastJSON_dat);
                
                if (report_via_websocket == true)
                {
                        WebsocketServer_SendBrocastJSONMessage(brocastJSON_dat, false);
                }

                /*
                if (report_via_mqtt == true)
                {
                        cJSON* topic_del_JSON = cJSON_DetachItemFromObject(brocastJSON_dat, "topic");
                        if (topic_del_JSON != NULL)
                        {
                                cJSON_Delete(topic_del_JSON);
                        }
                        cJSON_AddStringToObject(brocastJSON_dat, "sender", "Client");
                        MqttClient_Publish_Self_APP_JSON_Data(MQTT_DATA_CHANNEL_TYPE, brocastJSON_dat, true);
                }
                else
                {
                        cJSON_Delete(brocastJSON_dat);
                }
                */

                cJSON_Delete(brocastJSON_dat);
        }
      
      return -1;
}

int Process_LED_PWM_APP_Incoming_JSON_POST_Msg(App_Interface_Protocol protocol, cJSON *in_json)
{
    int status;
    switch(protocol)
    {
    case WebSocket:
    case Telnet:
        status = Process_LED_PWM_APP_JSON_POST_Commands(in_json);
	if (status>=0)
	{
		status = LED_PWM_Do_Report(true, true);
	}
        break;
    case MQTT:
        status = Process_LED_PWM_APP_JSON_POST_Commands(in_json);
	if (status>=0)
	{
		status = LED_PWM_Do_Report(true, false);
	}
        break;
    default:
        status = -1;
        break;
    }
    
    return status;
}

int Process_LED_PWM_APP_Incoming_JSON_GET_Msg(App_Interface_Protocol protocol, cJSON *in_json, cJSON *rsp_json)
{
    switch(protocol)
    {
    case WebSocket:
    case Telnet:
    case MQTT:
        return Process_LED_PWM_APP_JSON_GET_Commands(in_json, rsp_json);
        break;
    default:
        return -1;
    }
}


int Neon_APP_Device_Init()
{
        HTTPd_Register_CGI_URL_Callback("/LED_PWM", LED_PWM_CGI_Root, NULL);
        HTTPd_Register_CGI_URL_Callback("/LED_PWM/Num", LED_PWM_CGI_LED_Num, NULL);
        HTTPd_Register_CGI_URL_Callback("/LED_PWM/*", LED_PWM_CGI_LED_Any_Number, NULL);
        HTTPd_Register_CGI_URL_Callback("/LED_PWM/Level/*", LED_PWM_CGI_LED_Level_Any_Number, NULL);
        HTTPd_Register_CGI_URL_Callback("/LED_PWM/On_Off/*", LED_PWM_CGI_LED_OnOff_Any_Number, NULL);
        
        LED_PWM_Init();

        return Register_Neon_APP_Interface_Msg_CallBack(APP_TOPIC_NAME, Process_LED_PWM_APP_Incoming_JSON_POST_Msg, Process_LED_PWM_APP_Incoming_JSON_GET_Msg);
}
