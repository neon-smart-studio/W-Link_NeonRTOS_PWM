
function SSDP_Search_Nearby_Nodes()
{
    var cmd = {
        command:"search nearby devices"
    };
    
    Websocket_Send_POST_Command('SSDP', cmd);
}

function SSDP_Search_Nearby_Node_Same_Model()
{
    var cmd = {
        command:"search nearby same model nodes"
    };
    
    Websocket_Send_POST_Command('SSDP', cmd);
}

function SSDP_Search_Nearby_Node_Specific_Node_Type(node_type)
{
    var cmd = {
        command:"search WiFiIOT specific type node",
        device_type: node_type
    };
    
    Websocket_Send_POST_Command('SSDP', cmd);
}

function SSDP_Get_Search_Result(callback)
{
    var cmd = {
        command:"get search result"
    };
    
    Websocket_Send_GET_Command('SSDP', cmd, callback);
}
