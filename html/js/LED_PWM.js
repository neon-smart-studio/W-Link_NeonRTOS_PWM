
function GET_Num_Of_LEDS(callback)
{
    var cmd = {
        "command":"get num of leds"
    }
    Websocket_Send_GET_Command("LED_PWM", cmd, callback);
}

function GET_Individual_LED_Status(callback)
{
    var cmd = {
        "command":"get individual led status"
    }
    Websocket_Send_GET_Command("LED_PWM", cmd, callback);
}

function GET_All_LED_Status(callback)
{
    var cmd = {
        "command":"get all led status"
    }
    Websocket_Send_GET_Command("LED_PWM", cmd, callback);
}

function Set_Individual_LED_On_Off(led_index, on_off)
{
    var cmd = {
        "command":"set individual led on/off state",
        "led_index":Number(led_index),
        "on_off":on_off,
    }
    Websocket_Send_POST_Command("LED_PWM", cmd);
}

function Set_Individual_LED_level(led_index, level)
{
    var cmd = {
        "command":"set individual led level state",
        "led_index":Number(led_index),
        "level":Number(level),
    }
    Websocket_Send_POST_Command("LED_PWM", cmd);
}
