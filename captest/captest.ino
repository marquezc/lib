/* Untested */

#include <stdio.h>
#include <math.h>

/* Pin Assignment */
const int b_pin = 2;
const int v_pin = A0;
const int v_pin2 = A1;
const int v_pin3 = A2;

/* Voltage Settings */
const float v_tol = .02;
const float alog_max = 1023.0;

#define ON 1
#define OFF 0

void setup(void);
void loop(void);
float getV(int);
void oputV(float, float, float);

void setup()
{
  Serial.begin(9600);
  pinMode(b_pin, INPUT);
}

void loop()
{
  int   b_state = digitalRead(b_pin);
  float v1 = getV(v_pin);
  float v2 = getV(v_pin2);
  float v3 = getV(v_pin3);

  if (b_state == ON)
    oputV(v1, v2, v3);
    
  delay(1);
}

float getV(int pin)
{
  float  voltage = analogRead(pin) / (alog_max / 5);
  return voltage;
}

void oputV(float v1, float v2, float v3)
{
  char databuf[32];
  sprintf(databuf, "%.02f %.02f %.02f", v1, v2, v3);
  Serial.println(databuf);  
}
