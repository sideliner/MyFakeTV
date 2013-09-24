//used idea from http://forum.arduino.cc/index.php/topic,148967.0.html
//heavily modified/tuned
//use of Narcoleptic library is not really beneficial here as power is supplied from a wall plug

/*
hardware setup:
calibration button - PD2 (dig. 2) to ground
green LED1 - PD6 (dig. 6)
green LED2 - PD3 (dig. 3)
blue LED - PD5 (dig. 5)
red LED1 - PB2 (dig. 10)
red LED2 - PB1 (dig. 9)
white LED - PB3 (dig. 11)
superbright white LED - PD4 (dig. 4)
ldr (photo resistor) - VCC to A1
10k pot (calibration) - A1 to ground

ATmega328p controls LEDs via ULN2803 driver,
LEDs are connected from +9...12V to ULN2803 outputs
through current limiters/drivers NSI45030AT1G (30 mA)

*/
#define SECOND 1000
#define MINUTE 60000
#define HOUR 3600000

#include <Narcoleptic.h>
#include <TrueRandom.h>

const int calibrButton = 2;    // digital pin 2 / INT0
const int ledBlue = 5;         // digital pin 5 / PWM
const int ledGreen1 = 6;       // digital pin 6 / PWM
const int ledGreen2 = 3;       // digital pin 3 / PWM
const int ledRed1 = 10;        // digital pin 10 / PWM
const int ledRed2 = 9;         // digital pin 9 / PWM
const int ledWhite = 11;       // digital pin 11 / PWM
const int ledWhiteBright = 4;  // digital pin 4 / on-off (no PWM)

const unsigned int lumThreshold = 180;            //arbitrary sensor value threshold when it's considered "dark"
const unsigned int sensorDebounceCount = 4;       //ambience dark for 4 consequetive sensor readings

unsigned long sensorCheckInterval = MINUTE*1; //check sensor reading every 1 minute
unsigned long onTime = HOUR*5;         //turn on TV/light for 5 hours
unsigned long offTime = HOUR*16;

unsigned int lumSensorValue = 1023;      //highest possible (very bright ambience)
unsigned int randpwm = 0;
unsigned long currTime = 0;
unsigned long prevTime = 0;
unsigned int sensorCheckCounter = 0;
unsigned int randomSwitch = 0;

void setup() {
  pinMode(calibrButton,INPUT);
  digitalWrite(calibrButton,HIGH);
  pinMode(ledBlue,OUTPUT);
  pinMode(ledGreen1,OUTPUT);
  pinMode(ledGreen2,OUTPUT);
  pinMode(ledRed1,OUTPUT);
  pinMode(ledRed2,OUTPUT);
  pinMode(ledWhite,OUTPUT);
  pinMode(ledWhiteBright,OUTPUT);
  switchOffLeds();
}

void loop() {
  
  currTime = millis();
  if(currTime - prevTime >= sensorCheckInterval)
  {
      prevTime = currTime;
      lumSensorValue = analogRead(1);
      if (lumSensorValue < lumThreshold)
      {
          sensorCheckCounter++;
      } else {
          sensorCheckCounter = 0;
      }
      
      if (sensorCheckCounter >= sensorDebounceCount)
      {
          randomSwitch = TrueRandom.random(1,10);
          if(randomSwitch > 3)
          {
              playScenes(currTime);
          } else {
              turnOnLight(currTime);
          }
          switchOffLeds();
          longDelay(offTime);
          prevTime = 0;
      }
      
      while (digitalRead(calibrButton) == LOW)
      {
          lumSensorValue = analogRead(1);
          if(lumSensorValue < lumThreshold)
          {
              digitalWrite(ledBlue,HIGH);
          }
          delay(100);
          switchOffLeds();
      }
  }
  delay(100);
}

void switchOffLeds()
{
      digitalWrite(ledBlue,LOW);
      digitalWrite(ledGreen1,LOW);
      digitalWrite(ledGreen2,LOW);
      digitalWrite(ledRed1,LOW);
      digitalWrite(ledRed2,LOW);
      digitalWrite(ledWhite,LOW);
      digitalWrite(ledWhiteBright,LOW);
}

void longDelay(long milliseconds)
{
   while(milliseconds > 0) {
      if(milliseconds > 8000)
      {
          milliseconds -= 8000;
          Narcoleptic.delay(8000);
      } else {
          Narcoleptic.delay(milliseconds);
          break;
      }
   }
}

void playScenes(long startTime)
{
    int brightnessFactor = 2;        //reduced to 1 when it gets very dark
    while (millis() - startTime < onTime)
    {
        for(int i=0;i<10;i++)  //play scene 1 multiple times
        {
            playScene1(brightnessFactor);
        }
        if (TrueRandom.random(1,3) == 1)  //possibly call scene 2
        {
            playScene2(brightnessFactor);
        }
        if (TrueRandom.random(1,3) == 1)  //possibly call scene 3
        {
            playScene3(brightnessFactor);
        }
        if (TrueRandom.random(1,3) == 1)  //possibly call scene 4
        {
            playScene4(brightnessFactor);
        }
        if (TrueRandom.random(1,4) == 1)  //possibly do a commercial break
        {
            playCommercial(brightnessFactor);
        }
        
        //reduce brightness after 1 hour
        if (millis() - startTime > 1*HOUR)
        {
            brightnessFactor = 1;
        }   
    }
}

void playScene1(int factor)
// Changes random light levels and linger-times 
// of all colors to simulate "normal" TV action
{
    analogWrite(ledRed1,TrueRandom.random(10*factor,128*factor)); 
    analogWrite(ledRed2,TrueRandom.random(10*factor,128*factor)); 
    analogWrite(ledGreen1,TrueRandom.random(10*factor,128*factor)); 
    analogWrite(ledGreen2,TrueRandom.random(10*factor,128*factor)); 
    analogWrite(ledBlue,TrueRandom.random(5*factor,112*factor)); 
    analogWrite(ledWhite,TrueRandom.random(5*factor,90*factor));
    delay(TrueRandom.random(500,2000));
}

void playScene2(int factor)
// increases intensity of white,blue (fade-in)
{
    delay(SECOND);
    for(int i=2;i<(127*factor);i++)
    {
        analogWrite(ledBlue,i); 
        analogWrite(ledWhite,i);
        delay(25);
    }
}

void playScene3(int factor)
// flickers white,blue for a flickering scene effect
{
    boolean sw = HIGH;
    for(int i=0;i<30;i++)
    {
        if (sw)
        {
            analogWrite(ledWhite,127*factor);
            analogWrite(ledBlue,127*factor);
        } else {
            analogWrite(ledWhite,TrueRandom.random(2*factor,16*factor)); 
            analogWrite(ledBlue,TrueRandom.random(2*factor,16*factor));
        }
        sw = !sw;
        delay(TrueRandom.random(50,300));
    }
}

void playScene4(int factor)
// changes red/green light levels only
// white/blue are almost off
{
    analogWrite(ledWhite,TrueRandom.random(2*factor,12*factor));
    analogWrite(ledBlue,TrueRandom.random(2*factor,12*factor));
    for(int i=0;i<12;i++)
    {
        analogWrite(ledRed1,TrueRandom.random(10*factor,127*factor)); 
        analogWrite(ledRed2,TrueRandom.random(10*factor,127*factor)); 
        analogWrite(ledGreen1,TrueRandom.random(10*factor,127*factor)); 
        analogWrite(ledGreen2,TrueRandom.random(10*factor,127*factor)); 
        delay(TrueRandom.random(200,2000));
    }
}

void playCommercial(int factor)
// simulates a switch to or from a commercial break 
{
    analogWrite(ledRed1,TrueRandom.random(5*factor,10*factor)); 
    analogWrite(ledRed2,TrueRandom.random(2*factor,8*factor)); 
    analogWrite(ledGreen1,TrueRandom.random(2*factor,12*factor));
    analogWrite(ledGreen2,TrueRandom.random(2*factor,8*factor));
    analogWrite(ledBlue,TrueRandom.random(2*factor,6*factor));
    digitalWrite(ledWhite,LOW);
    delay(TrueRandom.random(1000,2500));
}

void turnOnLight(long startTime)
//white light with random variations
{
    while (millis() - startTime < onTime)
    {
        digitalWrite(ledWhiteBright,HIGH);
        digitalWrite(ledWhite,HIGH);
        delay(MINUTE);
        if (TrueRandom.random(1,5) == 1)  //variation in brightness
        {
            digitalWrite(ledWhite,LOW);
            delay(3*SECOND);
            digitalWrite(ledWhite,HIGH);
        }
        if (TrueRandom.random(1,5) == 1)  //variation in brightness
        {
            for(int i=2;i<255;i++)
            {
                analogWrite(ledWhite,i);
            }
        }
        if (TrueRandom.random(1,10) == 1)  //variation in brightness
        {
            digitalWrite(ledWhiteBright,LOW);
            delay(3*SECOND);
            digitalWrite(ledWhiteBright,HIGH);
        }
        if (TrueRandom.random(1,7) == 1)  //variation in brightness
        {
            digitalWrite(ledWhiteBright,LOW);
            digitalWrite(ledWhite,HIGH);
            delay(7*SECOND);
            digitalWrite(ledWhiteBright,HIGH);
            digitalWrite(ledWhite,LOW);
            delay(12*SECOND);
        }
        if (TrueRandom.random(1,8) == 1)  //variation in brightness
        {
            for(int i=255;i>2;i--)
            {
                analogWrite(ledWhite,i);
            }
        }
    }
}
