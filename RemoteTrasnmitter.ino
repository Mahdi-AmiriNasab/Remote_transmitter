/**************private includes***********/
//#include <SPI.h>
#include <LoRa.h>
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "pins_arduino.h"


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
  LoRa.setPins(pin_CE, pin_RST, pin_D0);
  
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!\n reset the chip\n");
  }

}

void loop() {


while (1)
{
 if (!LoRa.begin(915E6))
  {
    Serial.println("Starting LoRa failed!");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);

    delay(500);
      
  }
  else
  {
    Serial.println("successfully initialized");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  delay(100);  
}






  for(uint8_t i = 0; i < 10; i++)
  {
    XValue += analogRead(Xaxis);
    YValue += analogRead(Yaxis);
  }

  XValue /= avr_dem;
  YValue /= avr_dem;

  
 sprintf(Buff, " XValue: %d, YValue: %d\n", XValue, YValue);
 

  // make the packet ready
  Serial.print("Sending packet: ");

  // send packet
  LoRa.beginPacket();
  LoRa.print((char *)Buff);
  LoRa.endPacket();

  Serial.print((char *)Buff);

  delay(500);
}

