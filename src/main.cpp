#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ServoManager.h>
#include <State.h>
#include "CD4017.h"
#include <EEPROM.h>
#include <HeartbeatLED.h>
#include "Timer.h"

static const uint8_t TXLED = TX;
static const uint8_t RST_4017 = D6;
static const uint8_t CLK_4017 = D7;
volatile uint8_t clk4017ClockUpduration = 0;
volatile uint8_t debugPin = HIGH;
// CLK, RST
CD4017 cd4017(CLK_4017, RST_4017);

ServoManager *servoManager;

volatile bool isrCalled = false;
volatile unsigned long isrCounter = 0;
 Timer   *counter = new Timer();

HeartbeatLED heartbeat(LED_BUILTIN);

// ISR to Fire when Timer is triggered do not move the function it must be declared before the setupIsr
void IRAM_ATTR onIsr()
{

  uint32_t ticks = 50000;

  isrCalled = true;
  isrCounter++;
  if(servoManager->isFirst()) {
      counter->reset();
  }
  
  if(!servoManager->isAllDone()) {
    Servo *servo = servoManager->getCurrent();
    State next = servo->nextState();
    switch (next)
    {
    case startHigh:
      // TODO: get min instead of hardcoded
      ticks = 50000;
      cd4017.clockHigh();
      break;
  
    case variablePhase:
      cd4017.clockLow();            // low does nothing to the decades gates, just prepares the next high
      ticks = servo->getPosition(); // TODO: skip if position is zero
      break;
  
    case finishLow:
      cd4017.clockLow();
      servoManager->nextServo();
      break;
  
    default:
      // optional: fail-safe or logging
      break;
    }
  
  } else {
    if (servoManager->isAllDone())
    {
      cd4017.resetHigh();
    } else {
      cd4017.resetLow(); 
    }
  
  }
 

  timer1_write(ticks);
}

void setupIsr()
{

  /* Dividers:
    TIM_DIV1 = 0,   //80MHz (80 ticks/us - 104857.588 us max)
    TIM_DIV16 = 1,  //5MHz (5 ticks/us - 1677721.4 us max)
    TIM_DIV256 = 3  //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
  Reloads:
    TIM_SINGLE	0 //on interrupt routine you need to write a new value to start the timer again
    TIM_LOOP	1 //on interrupt the counter will start with the same value again

    with TIM_DIV256 :
        for 80MHZ
          80MHz = 80000000Hz
          80000000/256=312500Hz = 321.5Khz simplified formula : 1/(80/256)=3.2
          1s/312500 = 3.2us
        for 160MHz
          160000000/256 =  625000
          160000000/16 = 10000000
          1s/625000 = 0.0000016s = 1.6us
    the timer is 16bit 2¹⁶ = 65536
        max timer interrupt tick 1.6*65536 = 4294967296us = 4294.967296s = 1 hr 11 min 34 sec
    A servo has a period between 1ms and 2 ms so a range of 1ms = 1000us
      1000us/1.6 = 625 step (for 80MHZ -> 312 STEPS)
      180°/625 = 0.288°
      0.288 degree is the precision per positionning if the timer compute or each
      step witch position it should be

    frequency  MHZ	      divider	    fequency HZ	    period us	    step per 180°	    precision degree°	    max period ms   usability
    80	                  1	         80000000	        0.0125	         80000.0	      0.00225	                 0.81920      ok
    80	                  16	        5000000	        0.2	              5000.0	      0.036	                 13.10720       ok
    80	                  256	         312500	        3.2	               312.5	      0.576	                209.71520       ok
    160	                  1	        160000000	        0.00625	        160000.0	      0.001125	              0.40960       ko
    160	                  16	       10000000	        0.1	             10000.0	      0.018	                  6.55360       OK perfect
    160	                  256	         625000	        1.6	               625.0	      0.288	                104.85760       OK good

2.5*8=20 we should manage 8 servo from 0.5 to 2.5 ms to leverage the one with extra rotation angles
500us/0.0125 = 40000 tick = 0.5ms
we choosen 160MHZ divider 16 so :
  500us/0.1=5000
  5ms =5000us
  */
  // TODO verify that it dont mess with the wifi
  // CallBack function
  timer1_isr_init();
  timer1_attachInterrupt(onIsr);

  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(5000);
}
void setupSerial()
{

  Serial.begin(115200);
  delay(2000);
}
void setupPin()
{

  const uint8_t controlledPins[] = {D0, D1, D5, D6, D7};
  for (uint8_t i = 0; i < sizeof(controlledPins); i++)
  {
    pinMode(controlledPins[i], OUTPUT);
    digitalWrite(controlledPins[i], LOW);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  cd4017.init();
}

void setupServo()
{
  servoManager = new ServoManager();
}
bool isIsrReady = false;
void setup()
{

  delay(100);
  timer1_detachInterrupt();

  setupSerial();
  yield();

  delay(1000);
  Serial.println("");
  Serial.println("Reset reason: ");
  Serial.println(ESP.getResetReason());
  Serial.println(ESP.getResetInfo());

  //store boot reason in eprom to avoid loop boot after reprogramation
  EEPROM.begin(1);
  if (ESP.getResetReason() == "External System" && EEPROM.read(0) != 42)
  {
    Serial.println("First boot after upload — forcing clean restart...");
    EEPROM.write(0, 42);
    EEPROM.commit();
    Serial.println("Restarting in 1s...");
    delay(1000); 
    ESP.restart();
  }
  EEPROM.write(0, 0); 
  EEPROM.commit();

  setupPin();
  setupServo();

  Serial.println(">> I AM Alive !");
  delay(1000);
  Serial.print("CLK_4017 GPIO = ");
  // doit afficher 13
  Serial.println(CLK_4017);

  // flash the led
  heartbeat.setFrequency(50, 1000);

  Serial.println("Setup done");


}

void serialpplot()
{
  Serial.print(">");

  Serial.print("var1:");
  Serial.print(D6);
  Serial.print(",");

  Serial.print("var2:");
  Serial.print(D7);
  Serial.print(",");

  delay(1000);
}

/**
 * @brief Oscillates a single servo between minPercent and maxPercent of its configured range,
 *        using a sinusoidal profile over a given period in milliseconds.
 *
 * @param servoIndex Index of the servo to control (0–7)
 * @param periodMs Duration of one full oscillation cycle (e.g. 4000 ms for 4 seconds)
 * @param minPercent Minimum position as a percentage (e.g. 25.0)
 * @param maxPercent Maximum position as a percentage (e.g. 75.0)
 */
void oscillateServoBetweenPercents(uint8_t servoIndex, unsigned long periodMs, float minPercent, float maxPercent)
{
  if (servoIndex >= 8)
    return;
  if (minPercent > maxPercent)
    return;

  unsigned long now = millis();
  float phase = (now % periodMs) / (float)periodMs; // [0.0 .. 1.0]
  float sine = sin(phase * 2 * PI);                 // [-1 .. +1]

  float range = maxPercent - minPercent;
  float percent = minPercent + 0.5f * range * (sine + 1.0f); // [minPercent .. maxPercent]

  servoManager->getServo(servoIndex)->setPosition(percent);
  yield();
}
unsigned long lastIsrCount = 0;

unsigned long servoFps = 20; // refres every 20ms
void loop()
{
  yield();
  ESP.wdtFeed();

  if (!isIsrReady)
  {
    setupIsr();
    isIsrReady = true;
  }

  heartbeat.update();

  if (isrCounter != lastIsrCount)
  {
    Serial.print("ISR count: ");
    Serial.println(isrCounter);
    lastIsrCount = isrCounter;
  }
  delay(20);
  /** 
  for (int i = 0; i < 8; i++)
  {
    yield();
    // Optional: debug output
    Serial.print("servo ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(servoManager->getServo(i)->getPosition());
    Serial.println("");
  }*/

  oscillateServoBetweenPercents(0, 500, 25.0f, 75.0f);
  oscillateServoBetweenPercents(1, 3000, 10.0f, 90.0f);
  oscillateServoBetweenPercents(2, 3000, 10.0f, 90.0f);
  oscillateServoBetweenPercents(3, 5000, 10.0f, 90.0f);
  oscillateServoBetweenPercents(4, 10000, 10.0f, 90.0f);
  oscillateServoBetweenPercents(5, 3000, 10.0f, 90.0f);
  oscillateServoBetweenPercents(6, 3000, 10.0f, 90.0f);
  oscillateServoBetweenPercents(7, 3000, 10.0f, 90.0f);

  // serialpplot();
}
