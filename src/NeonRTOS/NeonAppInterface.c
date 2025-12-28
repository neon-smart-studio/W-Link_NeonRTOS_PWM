
#include <stdbool.h>
#include <stdint.h>

#include "time.h"

#include "lwip/opt.h"
#include "lwip/ip_addr.h"

#include "NeonRTOS.h"
#include "NeonTCPIP.h"

#include "GPIO/GPIO.h"
#include "UART/UART.h"

#include "cJSON/cJSON.h"
#include "HTTPd/HTTPd.h"

//#include "SSDP/SSDP.h"

#include "NeonAppInterface.h"

#define INIT_THREAD_STACK_SIZE  2048
#define INIT_THREAD_PRIORITY    1

typedef struct APP_Topic_CallBack_Item_Inf_t
{
	uint8_t* data_channel_ID;
	APP_Interface_Recv_This_Node_POST_Msg_CB onRecvThisNodePostMsgCB;
	APP_Interface_Recv_This_Node_GET_Msg_CB onRecvThisNodeGetMsgCB;
	struct APP_Topic_CallBack_Item_Inf_t* next;
}APP_Topic_CallBack_Item_Inf;

APP_Topic_CallBack_Item_Inf* app_if_callback_inf_lst = NULL;

int Network_CGI_Root(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int status = HTTPD_CGI_DONE;
    
    uint32_t ip_addr;
    uint32_t gw;
    uint32_t netmask;
    
    char ip_addr_str[20];
    char gw_str[20];
    char netmask_str[20];

    memset(ip_addr_str, 0, 20);
    memset(gw_str, 0, 20);
    memset(netmask_str, 0, 20);

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
            
            cJSON* ip_addr_item = cJSON_GetObjectItem(req_json, "ip_addr");
            cJSON* gateway_item = cJSON_GetObjectItem(req_json, "gateway");
            cJSON* netmask_item = cJSON_GetObjectItem(req_json, "netmask");
            if (ip_addr_item == NULL || gateway_item == NULL || netmask_item == NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            if (ip_addr_item->type != cJSON_String || gateway_item->type != cJSON_String || netmask_item->type != cJSON_String)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }

            uint32_t ip_addr = ip_string_to_u32(ip_addr_item->valuestring);
            uint32_t gw = ip_string_to_u32(gateway_item->valuestring);
            uint32_t netmask = ip_string_to_u32(netmask_item->valuestring);

            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
	    cJSON_AddStringToObject(rsp_json, "status", "success");
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);

            NeonTCPIP_IF_Update_Addresses(ip_addr, netmask, gw);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            ip_addr = NeonTCPIP_IF_Get_IP_Address();
            gw = NeonTCPIP_IF_Get_Gateway_Address();
            netmask = NeonTCPIP_IF_Get_NetMask_Address();

            ip_u32_to_string(ip_addr, ip_addr_str, sizeof(ip_addr_str));
            ip_u32_to_string(gw, gw_str, sizeof(gw_str));
            ip_u32_to_string(netmask, netmask_str, sizeof(netmask_str));

	    cJSON_AddStringToObject(rsp_json, "ip_addr", ip_addr_str);
	    cJSON_AddStringToObject(rsp_json, "gateway", gw_str);
	    cJSON_AddStringToObject(rsp_json, "netmask", netmask_str);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int Network_CGI_IpAddr(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int status = HTTPD_CGI_DONE;
    
    uint32_t ip_addr;
    
    char ip_addr_str[20];

    memset(ip_addr_str, 0, 20);

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
            
            cJSON* ip_addr_item = cJSON_GetObjectItem(req_json, "ip_addr");
            if (ip_addr_item == NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            if (ip_addr_item->type != cJSON_String)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }

            uint32_t ip_addr = ip_string_to_u32(ip_addr_item->valuestring);
            
            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
	    cJSON_AddStringToObject(rsp_json, "status", "success");
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);
        
            NeonTCPIP_IF_Set_IP_Address(ip_addr);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            ip_addr = NeonTCPIP_IF_Get_IP_Address();

            ip_u32_to_string(ip_addr, ip_addr_str, sizeof(ip_addr_str));

	    cJSON_AddStringToObject(rsp_json, "ip_addr", ip_addr_str);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int Network_CGI_Gateway(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int status = HTTPD_CGI_DONE;
    
    uint32_t gw;
    
    char gw_str[20];

    memset(gw_str, 0, 20);

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
            
            cJSON* gateway_item = cJSON_GetObjectItem(req_json, "gateway");
            if (gateway_item == NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            if (gateway_item->type != cJSON_String)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }

            uint32_t gw = ip_string_to_u32(gateway_item->valuestring);
            
            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
	    cJSON_AddStringToObject(rsp_json, "status", "success");
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);
        
            NeonTCPIP_IF_Set_Gateway_Address(gw);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            gw = NeonTCPIP_IF_Get_Gateway_Address();

            ip_u32_to_string(gw, gw_str, sizeof(gw_str));

	    cJSON_AddStringToObject(rsp_json, "gateway", gw_str);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

int Network_CGI_Netmask(HTTPd_WebSocked_Client_Connection *connData)
{
    if(connData==NULL) return HTTPD_CGI_DONE;
    
    int status = HTTPD_CGI_DONE;
    
    uint32_t netmask;
    
    char netmask_str[20];

    memset(netmask_str, 0, 20);

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
            
            cJSON* netmask_item = cJSON_GetObjectItem(req_json, "netmask");
            if (netmask_item == NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }
            if (netmask_item->type != cJSON_String)
            {
                    return HTTPd_Send_CGI_Response(connData, 422, "txt", NULL, 0);
            }

            uint32_t netmask = ip_string_to_u32(netmask_item->valuestring);
            
            cJSON_Delete(req_json);
            
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                    return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
	    cJSON_AddStringToObject(rsp_json, "status", "success");
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 201, rsp_json, true);
        
            NeonTCPIP_IF_Set_NetMask_Address(netmask);
    }
    else if(HTTPd_Get_CGI_Request_Type(connData)==HTTPd_Method_GET)
    {
            cJSON* rsp_json = cJSON_CreateObject();
            if(rsp_json==NULL)
            {
                  return HTTPd_Send_CGI_Response(connData, 500, "txt", NULL, 0);
            }
            
            netmask = NeonTCPIP_IF_Get_NetMask_Address();

            ip_u32_to_string(netmask, netmask_str, sizeof(netmask_str));

	    cJSON_AddStringToObject(rsp_json, "netmask", netmask_str);
            
            status = HTTPd_Send_CGI_JSON_Response(connData, 200, rsp_json, true);
    }
    else{
            status = HTTPd_Send_CGI_Response(connData, 404, "txt", NULL, 0);
    }
    
    return status;
}

/*
int App_Interface_Process_MQTT_Post_Message(MQTT_Address_Type address_mode, const char* callDevice_ID, const char* msg_topic, cJSON* in_msg)
{
        if(address_mode>=MQTT_Address_Type_MAX){return -1;}
        if(address_mode!=MQTT_Address_Type_Broadcast && callDevice_ID==NULL){return -1;}
        
        int status = -1;
        
        bool data_channel_ID_match = false;
        
        APP_Topic_CallBack_Item_Inf* current = app_if_callback_inf_lst;
        while (current != NULL)
        {
                if (strlen((char*)current->data_channel_ID) == strlen((char*)msg_topic))
                {
                        if (memcmp(current->data_channel_ID, msg_topic, strlen((char*)msg_topic)) == 0)
                        {
                                data_channel_ID_match = true;
                                break;
                        }
                }
                current = current->next;
        }
        
        if(data_channel_ID_match)
        {
                if(address_mode==MQTT_Address_Type_Broadcast)
                {
                        if(current->onRecvThisNodePostMsgCB!=NULL)
                        {
                              current->onRecvThisNodePostMsgCB(MQTT, in_msg);
                        }
                        
                        status = 0;
                }
                
                if(address_mode==MQTT_Address_Type_Multiicast)
                {
                        if(current->onRecvThisNodePostMsgCB!=NULL)
                        {
                              status = current->onRecvThisNodePostMsgCB(MQTT, in_msg);
                        }
                }
                  
                if(address_mode==MQTT_Address_Type_Unicast)
                {
                        //determine self or zigbee node
                        if(strlen(callDevice_ID)<strlen(Node_Device_ID)){return -1;}
                        if(strncmp(callDevice_ID, Node_Device_ID, strlen(Node_Device_ID))!=0){return -1;}
                        
                        if(strlen(callDevice_ID)==strlen(Node_Device_ID))
                        {
                                if(current->onRecvThisNodePostMsgCB!=NULL)
                                {
                                      status = current->onRecvThisNodePostMsgCB(MQTT, in_msg);
                                }
                        }
                }
        }
        
        return status;
}

int App_Interface_Process_MQTT_Get_Message(MQTT_Address_Type address_mode, const char* callDevice_ID, const char* msg_topic, cJSON* in_msg, cJSON* rsp_msg)
{
        if(address_mode>=MQTT_Address_Type_MAX){return -1;}
        
        int status = -1;
        
        if(address_mode!=MQTT_Address_Type_Unicast)
        {
                return -1;
        }
        if(strlen(callDevice_ID)<strlen(Node_Device_ID)){return -1;}
        if(strncmp(callDevice_ID, Node_Device_ID, strlen(Node_Device_ID))!=0){return -1;}
        
        bool data_channel_ID_match = false;
        
        APP_Topic_CallBack_Item_Inf* current = app_if_callback_inf_lst;
        while (current != NULL)
        {
                if (strlen((char*)current->data_channel_ID) == strlen((char*)msg_topic))
                {
                        if (memcmp(current->data_channel_ID, msg_topic, strlen((char*)msg_topic)) == 0)
                        {
                                data_channel_ID_match = true;
                                break;
                        }
                }
                current = current->next;
        }
        
        if(data_channel_ID_match)
        {
                if(address_mode==MQTT_Address_Type_Unicast)
                {
                        if(current->onRecvThisNodeGetMsgCB!=NULL)
                        {
                              status = current->onRecvThisNodeGetMsgCB(MQTT, in_msg, rsp_msg);
                        }
                }
        }
        
        return status;
}

void MQTT_Client_Registered_Data_Channel_ID_Message_CallBack(const char* dataChannel_ID, MQTT_Address_Type address_mode, const char* callDevice_ID, uint8_t* msg, uint16_t len)
{
	if (address_mode>=MQTT_Address_Type_MAX)
	{
		return;
	}
	
	cJSON* msg_json = cJSON_Parse((const char*)msg);
	if (!msg_json)
	{
		return;
	}

	cJSON* method = cJSON_GetObjectItem(msg_json, "method");
	cJSON* sender = cJSON_GetObjectItem(msg_json, "sender");
	
	if (NULL != method && NULL != sender)
        {
              if (method->type==cJSON_String && sender->type==cJSON_String)
              {		
                    if (strcmp(sender->valuestring, "Server") == 0)
                    {
                          cJSON* rsp_json;
                          int status;
                                          
                          if (strcmp(method->valuestring, "POST") == 0)
                          {
                                  status = App_Interface_Process_MQTT_Post_Message(address_mode, callDevice_ID, dataChannel_ID, msg_json);;
                          }
                          else if (strcmp(method->valuestring, "GET") == 0)
                          {
                                  rsp_json = cJSON_CreateObject();
                                  if (rsp_json != NULL)
                                  {
                                          status = App_Interface_Process_MQTT_Get_Message(address_mode, callDevice_ID, dataChannel_ID, msg_json, rsp_json);
                                          if(status>=0)
                                          {
                                                  cJSON_AddStringToObject(rsp_json, "sender", "Client");
                                                  cJSON_AddStringToObject(rsp_json, "method", "GET_RSP");
                                                  MqttClient_Publish_Self_APP_JSON_Data(dataChannel_ID, rsp_json, true);
                                          }
                                          else{
                                                  cJSON_Delete(rsp_json);
                                          }
                                  }
                          }
                    }
              }
        }
	
	if (msg_json != NULL)
	{
		cJSON_Delete(msg_json);
	}
}
*/
int Register_Neon_APP_Interface_Msg_CallBack(const char* topic, APP_Interface_Recv_This_Node_POST_Msg_CB postCB, APP_Interface_Recv_This_Node_GET_Msg_CB getCB)
{
	bool exist = false;
	APP_Topic_CallBack_Item_Inf* last = NULL;
	APP_Topic_CallBack_Item_Inf* current = app_if_callback_inf_lst;
	
	while (current != NULL)
	{
		if (strlen((char*)current->data_channel_ID) == strlen((char*)topic))
		{
			if (memcmp(current->data_channel_ID, topic, strlen((char*)topic)) == 0)
			{
				exist = true;
				break;
			}
		}
		
		last = current;
		current = current->next;
	}
	
	if (exist == true)
	{
		//exist -> so update it
		current->onRecvThisNodePostMsgCB = postCB;
		current->onRecvThisNodeGetMsgCB = getCB;
		return 1;
	}
	
        current = mem_Malloc(sizeof(APP_Topic_CallBack_Item_Inf));
        if (current == NULL)
        {
                return -1;
        }
        
        current->data_channel_ID = mem_Malloc(strlen(topic)+1);
        if (current->data_channel_ID == NULL)
        {
                mem_Free(current);
                return -1;
        }
        
        memcpy(current->data_channel_ID, topic, strlen(topic));
        current->data_channel_ID[strlen(topic)] = '\0';
        
        current->next = NULL;
        current->onRecvThisNodePostMsgCB = postCB;
        current->onRecvThisNodeGetMsgCB = getCB;
        
	if (app_if_callback_inf_lst == NULL)
        {
              app_if_callback_inf_lst = current;
        }
        else{
              last->next = current;
        }
        
	return 0;
	//return MqttClient_Register_Data_Channel_And_CallBack(topic, MQTT_Client_Registered_Data_Channel_ID_Message_CallBack);
}

int UnRegister_Neon_APP_Interface_Msg_CallBack(const char* topic)
{
        bool found = false;
	APP_Topic_CallBack_Item_Inf* last = NULL;
	APP_Topic_CallBack_Item_Inf* current = app_if_callback_inf_lst;
	
	while (current != NULL)
	{
		if (strlen((char*)current->data_channel_ID) == strlen((char*)topic))
		{
			if (memcmp(current->data_channel_ID, topic, strlen((char*)topic)) == 0)
			{
				if (last == NULL)
				{
					app_if_callback_inf_lst = current->next;
				}
				else
				{
					last->next = current->next;
				}
				
				if (current->data_channel_ID != NULL)
				{
					mem_Free(current->data_channel_ID);
				}
				
				if (current != NULL)
				{
					mem_Free(current);
				}
				
                                found = true;
                                
				break;
			}
		}
		
		last = current;
		current = current->next;
	}
	
        if(found)
        {
                return 0;
                //return MqttClient_Unregister_Data_Channel_And_CallBack(topic);
        }
        return -1;
}

static int Process_WebSocket_Post_Message(const char* msg_topic, cJSON* message)
{
        int status = -1;
        
        APP_Topic_CallBack_Item_Inf* current = app_if_callback_inf_lst;
        
        while (current != NULL)
        {
                if (strlen((char*)current->data_channel_ID) == strlen((char*)msg_topic))
                {
                        if (memcmp(current->data_channel_ID, msg_topic, strlen((char*)msg_topic)) == 0)
                        {
                                if(current->onRecvThisNodePostMsgCB!=NULL)
                                {
                                        status = current->onRecvThisNodePostMsgCB(WebSocket, message);
                                }
                                break;
                        }
                }
                current = current->next;
        }
        
        return status;
}

static int Process_WebSocket_Get_Message(const char* msg_topic, cJSON* message, cJSON* rsp_json)
{
        int status = -1;
        bool match_cmd;
        
        status = -1;
        
        APP_Topic_CallBack_Item_Inf* current = app_if_callback_inf_lst;
        
        while (current != NULL)
        {
                if (strlen((char*)current->data_channel_ID) == strlen((char*)msg_topic))
                {
                        if (memcmp(current->data_channel_ID, msg_topic, strlen((char*)msg_topic)) == 0)
                        {
                                if(current->onRecvThisNodeGetMsgCB!=NULL)
                                {
                                        status = current->onRecvThisNodeGetMsgCB(WebSocket, message, rsp_json);
                                }
                                break;
                        }
                }
                current = current->next;
        }
        
        if (status >= 0)
        {
              cJSON_AddStringToObject(rsp_json, "method", "GET_RSP");
              cJSON_AddStringToObject(rsp_json, "topic", msg_topic);
        }
        
        return status;
}

void Process_Websocket_Incomming_Message(struct HTTPd_WebSocked_Client_Connection *src, const HTTPd_WSFrame* frame_header, uint8_t* msg)
{
        if((frame_header->payloadLength)<=0 || msg==NULL)
        {
		return;
        }

	cJSON* msg_json = cJSON_Parse((const char*)msg);
	if (msg_json==NULL)
	{
		return;
	}
        
	cJSON* method = cJSON_GetObjectItem(msg_json, "method");
	cJSON* topic = cJSON_GetObjectItem(msg_json, "topic");
	
	if (NULL != method && method->type == cJSON_String) {
              if (NULL != topic && topic->type == cJSON_String) {
                      if (strcmp(method->valuestring, "POST") == 0)
                      {
                              Process_WebSocket_Post_Message(topic->valuestring, msg_json);
                      }
                      if (strcmp(method->valuestring, "GET") == 0)
                      {
                              cJSON* rsp_json = cJSON_CreateObject();
                              if(rsp_json==NULL){
                                    return;
                              }
                              if (Process_WebSocket_Get_Message(topic->valuestring, msg_json, rsp_json)>=0)
                              {
                                    WebsocketServer_SendJSONMessage(src, rsp_json, true);
                              }
                              else{
                                    cJSON_Delete(rsp_json);
                              }
                      }
              }
	}
	
        //printf("cJSON_Delete(msg_json);\r\n");
	cJSON_Delete(msg_json);
}

void Init_Thread(void* p)
{
    NeonTCPIP_init(NULL, NULL, NULL, NULL);

    HTTPd_Init();

    HTTPd_Register_CGI_URL_Callback("/Network", Network_CGI_Root, NULL);
    HTTPd_Register_CGI_URL_Callback("/Network/IP_Addr", Network_CGI_IpAddr, NULL);
    HTTPd_Register_CGI_URL_Callback("/Network/Gateway", Network_CGI_Gateway, NULL);
    HTTPd_Register_CGI_URL_Callback("/Network/Netmask", Network_CGI_Netmask, NULL);

    Neon_APP_Device_Init();

    //NeonRTOS_TaskCreate(Display_Task, (const signed char*)"Display Task", 2048, NULL, 1, NULL);

    GPIO_Pin_Init(LED_R, hwGPIO_Direction_Output, hwGPIO_Pull_Mode_Up);
    GPIO_Pin_Init(LED_B, hwGPIO_Direction_Output, hwGPIO_Pull_Mode_Up);
    GPIO_Pin_Init(LED_G, hwGPIO_Direction_Output, hwGPIO_Pull_Mode_Up);

    while (1)
    {
        if(NeonTCPIP_IF_isLinkUp())
        {
#if LWIP_DHCP
            if(NeonTCPIP_Get_DHCP_State()==DHCP_ADDRESS_ASSIGNED)
            {
                GPIO_Pin_Write(LED_B, 1);
                NeonRTOS_Sleep(500);

                GPIO_Pin_Write(LED_B, 0);
                NeonRTOS_Sleep(500);
            }
            else
            {
                GPIO_Pin_Write(LED_G, 1);
                NeonRTOS_Sleep(500);

                GPIO_Pin_Write(LED_G, 0);
                NeonRTOS_Sleep(500);
            }
#else
                GPIO_Pin_Write(LED_B, 1);
                NeonRTOS_Sleep(500);

                GPIO_Pin_Write(LED_B, 0);
                NeonRTOS_Sleep(500);
#endif
        }
        else
        {
            GPIO_Pin_Write(LED_R, 1);
            NeonRTOS_Sleep(200);

            GPIO_Pin_Write(LED_R, 0);
            NeonRTOS_Sleep(200);  
        }

        size_t freeHeapSize = NeonRTOS_GetFreeHeapSize();
        UART_Printf("Remain Heap Size %d bytes\n", freeHeapSize);
    }
}

void Neon_App_Init(void)
{
        UART_Open(LOG_UART_INDEX, 115200, false);

        WebsocketServer_RegisterMsgCallback(Process_Websocket_Incomming_Message);
    
#if INIT_THREAD_DEBUG==1
        printf("[Init Thread] Creating Threaf: Init_Thread()\n");
#endif
	if(NeonRTOS_TaskCreate(Init_Thread, (signed char *)"Init Thread", INIT_THREAD_STACK_SIZE, NULL, INIT_THREAD_PRIORITY, NULL)!=NeonRTOS_OK)
        {
#if INIT_THREAD_DEBUG==1
                printf("[main] Panic: Cannot Create Initialize Thread!\n");
#endif
        }
}