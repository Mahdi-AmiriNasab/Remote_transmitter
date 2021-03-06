/**************private includes***********/
//#include <SPI.h>
#include "LoRa.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "pins_arduino.h"
#include "PS2X_lib.h"  //for v1.6

PS2X remote; //create PS2 Controller Class

/*************Functions*************/
void blink (uint32_t del, uint32_t times)
{
  for(uint32_t i = 0; i < times; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(del);
    digitalWrite(LED_BUILTIN, LOW);
    delay(del);
  }
}

void send_packet(char **to_send_data_ptr)
{
  Serial.print("Sending packet: ");

  // send packet

  LoRa.beginPacket(25);
  LoRa.println(*to_send_data_ptr);
  LoRa.endPacket();
  Serial.println(*to_send_data_ptr);
  
  *to_send_data_ptr = '\0'; // clear the command
  blink(50, 3);
}

/*************Deffinitions***************/
//LoRa pins
#define pin_CE          53
#define pin_RST         49
#define pin_D0          2

//Dual shock comtroller pins
#define PIN_Clock       30
#define PIN_Command     31
#define PIN_Attention   32
#define PIN_Data        33

//Commands
#define CMD_SELECT          PSB_SELECT      
#define COMMAND1            PSB_L3          
#define COMMAND2            PSB_R3          
#define CMD_START           PSB_START       
#define CMD_UP              PSB_PAD_UP      //lift fork
#define COMMAND5            PSB_PAD_RIGHT   
#define CMD_DOWN            PSB_PAD_DOWN    //lower the fork
#define COMMAND7            PSB_PAD_LEFT    
#define COMMAND8            PSB_L2          
#define COMMAND9            PSB_R2          
#define COMMAND10           PSB_L1          
#define CMD_HRN             PSB_R1          //horn          
#define COMMAND12           PSB_GREEN       
#define COMMAND13           PSB_RED         
#define COMMAND14           PSB_BLUE        
#define COMMAND15           PSB_PINK        
#define COMMAND16           PSB_TRIANGLE    
#define CMD_ROT_CLW         PSB_CIRCLE      //clockwise rotation
#define CMD_STP             PSB_CROSS       //fully stops the system       
#define CMD_ROT_CCW         PSB_SQUARE      //counter clockwise rotation

//sticks
#define CMD_RIGHT_X         PSS_RX            
#define CMD_RIGHT_Y         PSS_RY            
#define CMD_LEFT_X          PSS_LX            
#define CMD_LEFT_X          PSS_LY            

//These are analog buttons
// #define CMD_RIGHT           PSAB_PAD_RIGHT 
// #define CMD_UP              PSAB_PAD_UP
// #define CMD_DOWN            PSAB_PAD_DOWN
// #define CMD_LEFT            PSAB_PAD_LEFT

// #define CMD_FORWARD         PSAB_L2
// #define CMD_BACKWARD        PSAB_R2
#define COMMAND30           PSAB_L1
#define COMMAND31           PSAB_R1
#define COMMAND32           PSAB_GREEN
#define COMMAND33           PSAB_RED
#define COMMAND34           PSAB_BLUE
#define COMMAND35           PSAB_PINK
#define COMMAND36           PSAB_TRIANGLE
#define COMMAND37           PSAB_CIRCLE
#define COMMAND38           PSAB_CROSS
#define COMMAND39           PSAB_SQUARE






/***************Variables****************/
int Xaxis = A0;    // select the input pin for the Xaxis Joystick
int Yaxis = A1;    // select the input pin for the Yazis Joystick
uint32_t XValue = 0, YValue = 0;    // analog values of axis accordingly 
char  Buff[200];

typedef enum
{
  button_pressed = 1,
  button_released
}button_status;


button_status BTN_CROSS;
button_status BTN_CIRCLE;
button_status BTN_SQUARE;
button_status BTN_TRIANGLE;

button_status BTN_UP;
button_status BTN_DOWN;

button_status BTN_LEFT;
button_status BTN_RIGHT;
button_status BTN_FORWARD;
button_status BTN_BACKWARD;



/*************Initializations*************/
void setup()
{
  //config LED
  pinMode(LED_BUILTIN, OUTPUT);

  //it's mandatory to configure pins before start
  LoRa.setPins(pin_CE, pin_RST, pin_D0);

  if(remote.config_gamepad(PIN_Clock, PIN_Command, PIN_Attention ,PIN_Data ,true, true) != 0)
  {
    Serial.println("No controller found!\n reset the chip");
    while(1)
      blink(1000, 1);
  }

  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if(!LoRa.begin(433000000))
  {
    Serial.println("initialization FAILED");
    blink(1000, 1);
      
  }

  //set the TX power to maximum
  LoRa.setTxPowerMAX();

  LoRa.setFrequency(433000000);
  LoRa.setSignalBandwidth(500000);
  LoRa.setSpreadingFactor(8);

  Serial.println("initialization SUCCESSFUL");
  blink(300, 2);
  
}
char commands[][10] = {"FORWARD","BACKWARD","UP", "DOWN", "RIGHT", "LEFT"};
char uncommands[][10] = {"_FORWARD","_BACKWARD","_UP", "_DOWN", "_RIGHT", "_LEFT"};
char *cmd_ptr = '\0';
uint8_t stick_rx,  stick_ry,  stick_lx,  stick_ly;
uint8_t i = 0;
void loop() 
{
  Start:
  remote.read_gamepad(false ,0);
  
  stick_ly = remote.Analog(PSS_LY);
  stick_lx = remote.Analog(PSS_LX);
  stick_rx = remote.Analog(PSS_RX);
  stick_ry = remote.Analog(PSS_RY);

  //left stick comparison
  if(stick_lx > 127 + 20)
  {
    //considered as right 
  }
  else if(stick_lx < 127 - 20)
  {
    //considered as left
  }
  else if(stick_ly < 127 - 20)
  {
    //considered as up
    sprintf(Buff, "FORWARD=%d",stick_ly);
    cmd_ptr = Buff;
    BTN_FORWARD = button_pressed;
  }
  else if(stick_ly > 127 + 20)
  {
    //considered as down
    sprintf(Buff, "BACKWARD=%d",stick_ly);
    cmd_ptr = Buff; 
    BTN_BACKWARD = button_pressed;
  }
  else
  {
    if(BTN_BACKWARD == button_pressed)
    {
      BTN_BACKWARD = button_released;
      cmd_ptr = "_BACKWARD";
    }
    if(BTN_FORWARD == button_pressed)
    {
      BTN_FORWARD = button_released;
      cmd_ptr = "_FORWARD";
    }
  }
  
  

  //right stick comparison
  if(stick_rx > 127 + 20)
  {
    //considered as right 
    sprintf(Buff, "RIGHT=%d",stick_rx);
    cmd_ptr = Buff;
    BTN_RIGHT = button_pressed;
  }
  else if(stick_rx < 127 - 20)
  { 
    //considered as left
    sprintf(Buff, "LEFT=%d",stick_rx);
    cmd_ptr = Buff;
    BTN_LEFT = button_pressed;
  }
  else if(stick_ry < 127 - 20)
  {
    //considered as down
    
  }
  else if(stick_ry > 127 + 20)
  {
    //considered as up
    
  }
  else
  {
    if(BTN_RIGHT == button_pressed)
    {
      BTN_RIGHT = button_released;
      cmd_ptr = "_RIGHT";
    }
    if(BTN_LEFT == button_pressed)
    {
      BTN_LEFT = button_released;
      cmd_ptr = "_LEFT";
    }
  }
  

  // if(remote.NewButtonState())
  // {
    if(remote.Button(CMD_UP))
    {
      BTN_UP = button_pressed;
      cmd_ptr = "UP";
    }
    else if(remote.ButtonReleased(CMD_UP))
    {
      BTN_UP = button_released;
      cmd_ptr = "_UP";
    }

    else if(remote.Button(CMD_DOWN))
    {
      BTN_DOWN = button_pressed;
      cmd_ptr = "DOWN";
    }
    else if(remote.ButtonReleased(CMD_DOWN))
    {
      BTN_DOWN = button_released;
      cmd_ptr = "_DOWN";
    }

    //clockwise rotation
    else if(remote.Button(CMD_ROT_CLW)) 
    {
      BTN_DOWN = button_pressed;
      cmd_ptr = "ROT_CLW";
    }
    else if(remote.ButtonReleased(CMD_ROT_CLW))
    {
      BTN_DOWN = button_released;
      cmd_ptr = "_ROT_CLW";
    }

    //counter clockwise rotation
    else if(remote.Button(CMD_ROT_CCW))
    {
      BTN_DOWN = button_pressed;
      cmd_ptr = "ROT_CCW";
    }
    else if(remote.ButtonReleased(CMD_ROT_CCW))
    {
      BTN_DOWN = button_released;
      cmd_ptr = "_ROT_CCW";
    }

    //horn
    else if(remote.Button(CMD_HRN))
    {
      BTN_DOWN = button_pressed;
      cmd_ptr = "HORN";
    }
    else if(remote.ButtonReleased(CMD_HRN))
    {
      BTN_DOWN = button_released;
      cmd_ptr = "_HORN";
    }

    //stop button
    else if(remote.Button(CMD_STP))
    {
      BTN_DOWN = button_pressed;
      cmd_ptr = "STOP";
    }
    else if(remote.ButtonReleased(CMD_STP))
    {
      BTN_DOWN = button_released;
      cmd_ptr = "_STOP";
    }


    // else if(remote.Button(CMD_LEFT))
    // {
    //   BTN_LEFT = button_pressed;
    //   cmd_ptr = "LEFT";
    // }
    // else if(remote.ButtonReleased(CMD_LEFT))
    // {
    //   BTN_LEFT = button_released;
    //   cmd_ptr = "_LEFT";
    // }
    // else if(remote.Button(CMD_RIGHT))
    // {
    //   BTN_RIGHT = button_pressed;
    //   cmd_ptr = "RIGHT";
    // }
    // else if(remote.ButtonReleased(CMD_RIGHT))
    // {
    //   BTN_RIGHT = button_released;
    //   cmd_ptr = "_RIGHT";
    // }
    // else if(remote.Button(CMD_FORWARD))
    // {
    //   BTN_FORWARD = button_pressed;
    //   cmd_ptr = "FORWARD";
    // }
    
    // else if(remote.ButtonReleased(CMD_FORWARD))
    // {
    //   BTN_FORWARD = button_released;
    //   cmd_ptr = "_FORWARD";
    // }

    // else if(remote.Button(CMD_BACKWARD))
    // {
    //   BTN_BACKWARD = button_pressed;
    //   cmd_ptr = "BACKWARD";
    // }
    // else if(remote.ButtonReleased(CMD_BACKWARD))
    // {
    //   BTN_BACKWARD = button_released;
    //   cmd_ptr = "_BACKWARD";
    // }

    // make the packet ready
    // else
    // {
    //   Serial.println("undefined key pressed");
    //   goto Start;
    // }
        
  // }
  
  if(cmd_ptr != '\0')
    send_packet(&cmd_ptr);
  
}

