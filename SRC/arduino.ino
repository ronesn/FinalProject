/*
  Controls engines by 4 commands: right, left, forward, brake
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int DCMotor1 = 6;
int DCMotor2 = 9;
int leftWheel = 5;
int rightWheel = 3;
int breaksLed = 13;
char input = 0;
int prevInput = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
   Serial.begin(9600);
  pinMode(DCMotor1, OUTPUT);  
  pinMode(DCMotor2, OUTPUT);
  pinMode(leftWheel, OUTPUT);
  pinMode(rightWheel, OUTPUT);
  pinMode(breaksLed, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available() > 0)
  {
     input = Serial.read();
	 
	 //turn left
     if (input == '2') 
       {
        breaks();
        digitalWrite(DCMotor1, LOW);// turn the LED on (HIGH is the voltage level)
        digitalWrite(DCMotor2, 100);
        digitalWrite(leftWheel, HIGH);
        digitalWrite(rightWheel, LOW);
        delay(150); 
      }
  
	  //turn right
      if (input == '1') 
      {
        breaks();
        digitalWrite(DCMotor1,LOW );// turn the LED on (HIGH is the voltage level)
        digitalWrite(DCMotor2, 100);
        digitalWrite(leftWheel, LOW);
        digitalWrite(rightWheel, HIGH);// turn the LED off by making the voltage LOW
        delay(150);  
      }
	  
	  //driving straight
      if (input == '3') 
      {
        breaks();
        digitalWrite(DCMotor1,LOW );// turn the LED on (HIGH is the voltage level)
        digitalWrite(DCMotor2, 100);
       digitalWrite(leftWheel, LOW);
        digitalWrite(rightWheel, LOW);
        delay(150); 
      }
	  
	  //stop if too close or stoped detecting LCP
      if (input == '4') 
      {
       digitalWrite(breaksLed,HIGH);
        digitalWrite(DCMotor1,LOW );// turn the LED on (HIGH is the voltage level)
        digitalWrite(DCMotor2, LOW);
       digitalWrite(leftWheel, LOW);
        digitalWrite(rightWheel, LOW);
        delay(150); 
      }
  }  
  else   
  {
        digitalWrite(DCMotor1,LOW );// turn the LED on (HIGH is the voltage level)
        digitalWrite(DCMotor2, LOW);
       digitalWrite(leftWheel, LOW);
        digitalWrite(rightWheel, LOW);
        delay(50); 
  }
  prevInput = input;
}
void breaks()
{
     if(input != prevInput)
     {
       digitalWrite(breaksLed,HIGH);
       digitalWrite(DCMotor1, HIGH);    // turn the LED on (HIGH is the voltage level)
       digitalWrite(DCMotor2, LOW);
       delay(50);
     }
      digitalWrite(breaksLed,LOW);
}