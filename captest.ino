int b_pin = 2;
int v_pin = A0;
int v_pin2 = A1;

void setup()
{
  Serial.begin(9600);
  pinMode(b_pin, INPUT);
}

int current = 0;
int current2 = 0;
#define ON 1
#define OFF 0
#define STATE 0;

#define RANGE(A, B, C) ((B) >= (A) - (C) && (B) <= (A) + (C)) ? 1 : 0 // Use this to avoid printing +/- .01

void loop()
{ 
   int b_state = digitalRead(b_pin); 
   
   double v1 = getV(v_pin);
   double v2 = getV(v_pin2);
   
   if (RANGE(v1, current, .01));
     return;
   
   Serial.println(v1, v2);
   current = v1;  
   delay(1);
}

int getV(int pin)
{
  int sensor = analogRead(pin);
  double voltage = (double) sensor / (1023.00 / 5.00);
  return voltage; 
}
