/**************private includes***********/
//#include <SPI.h>
#include <LoRa.h>
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "pins_arduino.h"

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

/*************Deffinitions***************/
#define avr_dem   10
#define pin_CE        53
#define pin_RST       49  
#define pin_D0        47


/***************Variables****************/
int Xaxis = A0;    // select the input pin for the Xaxis Joystick
int Yaxis = A1;    // select the input pin for the Yazis Joystick
uint32_t XValue = 0, YValue = 0;    // analog values of axis accordingly 
char  Buff[20];

/*************Initializations*************/
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  //it's mandatory to configure pins before start
  LoRa.setPins(pin_CE, pin_RST, pin_D0);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  while(!LoRa.begin(433000000))
  {
    Serial.println("initialization FAILED");
    blink(50, 2);

    delay(500);
      
  }

  //set the TX power to maximum
  LoRa.setTxPowerMAX();

  LoRa.setFrequency(433000000);
  LoRa.setSignalBandwidth(500000);
  LoRa.setSpreadingFactor(8);

  Serial.println("initialization SUCCESSFUL");
  blink(300, 2);
  
}

void loop() 
{
 sprintf(Buff, " XValue: %d, YValue: %d\n", XValue, YValue);

  // make the packet ready
  Serial.print("Sending packet: ");

  // send packet
  LoRa.beginPacket();
  LoRa.print((char *)Buff);
  LoRa.endPacket();
  Serial.print((char *)Buff);
  blink(20, 5);
  
  delay(500);

}

