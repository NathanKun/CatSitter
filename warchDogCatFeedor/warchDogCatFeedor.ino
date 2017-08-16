#include <avr/sleep.h>
#include <Servo.h>
// This variable is made volatile because it is changed inside an interrupt function
volatile int sleep_count = 0; // Keep track of how many sleep cycles have been completed.
// 每8小时大约只需睡眠470分钟==>每小时需要睡眠58.75分钟
// 8点，15点，22点循环，分别相隔7，7，10小时
// 则各段间隔各睡眠411，411，588分钟。
// 这里从15点开始
const long interval = 470; // Interval in minutes between waking and doing tasks.
// needed before the interval defined above elapses. Not that this does integer math.

const int TURNING_TIME = 700;
const int CW_MIN = 90;
const int CW_MAX = 85;
const int CCW_MIN = 99;
const int CCW_MAX = 107;
const int TURN_STOP = 94;
const long sleep_total = (interval * 60) / 8;

Servo myservo;

void clockwiseStart() {
  for (int i = CW_MIN + 1; i > CW_MAX; i--) {
    myservo.write(i);
    //    delay(50);
  }
}

void conterclockwiseStart() {
  for (int i = CCW_MIN - 1; i < CCW_MAX; i++) {
    myservo.write(i);
    //    delay(25);
  }
}

void turn(const int TURNING_TIME) {
  clockwiseStart();
  delay(TURNING_TIME);
  myservo.write(TURN_STOP);
  delay(1000);
}

void watchdogOn() {
  // Clear the reset flag, the WDRF bit (bit 3) of MCUSR.
  MCUSR = MCUSR & B11110111;
  // Set the WDCE bit (bit 4) and the WDE bit (bit 3) of WDTCSR.
  WDTCSR = WDTCSR | B00011000;
  // Set the watchdog timeout prescaler value to 1024 K
  // which will yeild a time-out interval of about 8.0 s.
  WDTCSR = B00100001;
  // Enable the watchdog timer interupt.
  WDTCSR = WDTCSR | B01000000;
  MCUSR = MCUSR & B11110111;
}

void setup(void) {
  watchdogOn(); // Turn on the watch dog timer.
  // Disable the ADC by setting the ADEN bit (bit 7) to zero.
  ADCSRA = ADCSRA & B01111111;
  // Disable the analog comparator by setting the ACD bit (bit 7) to one.
  ACSR = B10000000;
  // Disable digital input buffers on all analog input pins by setting bits 0-5 to one.
  DIDR0 = DIDR0 | B00111111;
  
  myservo.attach(5);
  turn(TURNING_TIME);
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
  sleep_enable(); // Enable sleep mode.
  sleep_mode(); // Enter sleep mode.
  // After waking from watchdog interrupt the code continues
  // to execute from this point.
  sleep_disable(); // Disable sleep mode after waking.
}

void loop(void) {
  goToSleep(); // ATmega328 goes to sleep for about 8 seconds
  // and continues to execute code when it wakes up
  if (sleep_count == sleep_total) {
    turn(TURNING_TIME);
    sleep_count = 0;
  }
}

ISR(WDT_vect)
{
  sleep_count ++; // keep track of how many sleep cycles have been completed.
}
