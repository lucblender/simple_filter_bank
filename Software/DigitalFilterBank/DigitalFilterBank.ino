#include "DaisyDuino.h"

#include "Adafruit_MPR121.h"

using namespace daisysp;


#define FEEDBACK_FACTOR_NONE 0.0f
#define FEEDBACK_FACTOR_LOW 0.10f
#define FEEDBACK_FACTOR_HIGH 0.15f

#define FILTER_00 A11
#define FILTER_01 A10
#define FILTER_03 A8
#define FILTER_02 A9
#define FILTER_04 A7
#define FILTER_05 A6
#define FILTER_06 A5
#define FILTER_07 A4
#define FILTER_08 A3
#define FILTER_09 A2
#define FILTER_10 A1
#define FILTER_11 A0


#define SLIDER_RIGHT A7
#define SLIDER_LEFT A6

uint8_t filterSliderPins[12] = { FILTER_00, FILTER_01, FILTER_02, FILTER_03, FILTER_04, FILTER_05, FILTER_06, FILTER_07, FILTER_08, FILTER_09, FILTER_10, FILTER_11 };

#define SW_FILTER_00 0
#define SW_FILTER_01 1
#define SW_FILTER_02 2
#define SW_FILTER_03 3
#define SW_FILTER_04 4
#define SW_FILTER_05 5
#define SW_FILTER_06 6
#define SW_FILTER_07 7
#define SW_FILTER_08 8
#define SW_FILTER_09 9
#define SW_FILTER_10 10
#define SW_FILTER_11 11

uint32_t switchSliderPins[12] = { SW_FILTER_00, SW_FILTER_01, SW_FILTER_02, SW_FILTER_03, SW_FILTER_04, SW_FILTER_05, SW_FILTER_06, SW_FILTER_07, SW_FILTER_08, SW_FILTER_09, SW_FILTER_10, SW_FILTER_11 };


#define SW_MODE_0 12
#define SW_MODE_1 13

#define SW_FDBK0_0 29
#define SW_FDBK0_1 30

#define SW_FDBK1_0 26
#define SW_FDBK1_1 27

//#define DEBUG_ANALOG_PIN
//#define DEBUG_DIGITAL_PIN

float sample_rate;

float filterFactors[12];
uint8_t filterSwitchStatus[12];

uint8_t switchMode = 0;

uint8_t switchfdbk0 = 2;
uint8_t switchfdbk1 = 0;

ReverbSc verb;

// ----------------- Capacitive sensor ---------------------------
Adafruit_MPR121 cap = Adafruit_MPR121();
#define MPR_TOUCH_IRQ 7

/* 
---------------------------------------------------------
-----------          Mode Description         -----------
-----                                               -----
-----   0 : input 0 goes to out 0, input 1 goes     -----
-----       to out 1. Switch enable or disable      -----
-----       filters on both output                  -----
-----                                               -----
-----   1 : input 0 goes to out 0 and 1 isn't used  -----
-----       Switch enable or disable  filters       -----
-----       only on output 1, output 0 has all      -----
-----       filters enabled                         -----
-----                                               -----
-----   1 : input 0 and 1 are mixed to out 0 and 1  -----
-----       Switch enable or disable  filters       -----
-----       only on output 1, output 0 has all      -----
-----       filters enabled                         -----
-----                                               -----
-----------                                   -----------
--------------------------------------------------------- 
 */

Svf* filters[12];
Svf* secondFilters[12];

float resonances[12] = { 0, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85, 0 };
float drives[12] = { 0, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0 };
//float filterFrequencies[12] = { 100, 158, 249, 392, 618, 975, 1538, 2425, 3825, 6032, 9512, 15000 };
float filterFrequencies[12] = { 98, 147, 220, 330, 494, 740, 1109, 1661, 2489, 3729, 5588, 8372 };


float touchValueFactors[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int touchIndex[12] = { 4, 5, 9, 1, 3, 6, 8, 10, 2, 7, 0, 11 };
int initialValues[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

float channel0LastSample = 0.0f;
float channel1LastSample = 0.0f;

float leftSliderValue = 0.0f;
float rightSliderValue = 0.0f;


void ProcessAudio(float** in, float** out, size_t size) {
  
  verb.SetFeedback(0.6f+0.25f*leftSliderValue);

  for (size_t i = 0; i < size; i++) {

    float in_0;
    float in_1;

    float fdbk_factor_0 = FEEDBACK_FACTOR_HIGH;
    float fdbk_factor_1 = FEEDBACK_FACTOR_HIGH;

    /*
    if (switchfdbk0 == 0) {
      fdbk_factor_0 = FEEDBACK_FACTOR_NONE;
    } else if (switchfdbk0 == 1) {
      fdbk_factor_0 = FEEDBACK_FACTOR_LOW;
    } else if (switchfdbk0 == 2) {
      fdbk_factor_0 = FEEDBACK_FACTOR_HIGH;
    }

    if (switchfdbk1 == 0) {
      fdbk_factor_1 = FEEDBACK_FACTOR_NONE;
    } else if (switchfdbk1 == 1) {
      fdbk_factor_1 = FEEDBACK_FACTOR_LOW;
    } else if (switchfdbk1 == 2) {
      fdbk_factor_1 = FEEDBACK_FACTOR_HIGH;
    }*/
    switchMode = 0;
    if (switchMode == 0) {
      in_0 = in[0][i] + channel0LastSample * fdbk_factor_0;
      in_1 = in[1][i] + channel1LastSample * fdbk_factor_1;
    } else if (switchMode == 1) {
      in_0 = in[0][i] + channel0LastSample * fdbk_factor_0;
      in_1 = in[0][i] + channel1LastSample * fdbk_factor_1;
    } else if (switchMode == 2) {
      in_0 = in[0][i] + in[1][i] + channel0LastSample * fdbk_factor_0;
      in_1 = in[0][i] + in[1][i] + channel1LastSample * fdbk_factor_1;
    }


    float out_0 = 0.0f;
    float out_1 = 0.0f;
    float additionalFactor_0 = 1.0f;
    float additionalFactor_1 = 1.0f;


    for (int i = 0; i < 12; i++) {
      filters[i]->Process(in_0);
      secondFilters[i]->Process(in_1);

      if (switchMode == 0) {
        additionalFactor_0 = filterSwitchStatus[i];
        additionalFactor_1 = filterSwitchStatus[i];

      } else if (switchMode == 1) {
        additionalFactor_0 = 1.0f;
        additionalFactor_1 = filterSwitchStatus[i];
      } else if (switchMode == 2) {
        additionalFactor_0 = 1.0f;
        additionalFactor_1 = filterSwitchStatus[i];
      }
      additionalFactor_0 = touchValueFactors[i];
      additionalFactor_1 = touchValueFactors[i];

      if (i == 0) {
        out_0 += filters[i]->Low() * touchValueFactors[i];
        out_1 += secondFilters[i]->Low() * touchValueFactors[i];

      } else if (i == 11) {
        out_0 += filters[i]->High() * touchValueFactors[i];
        out_1 += secondFilters[i]->High() * touchValueFactors[i];

      } else {
        out_0 += filters[i]->Band() * touchValueFactors[i];
        out_1 += secondFilters[i]->Band() * touchValueFactors[i];
      }
    }
    channel0LastSample = out_0;
    channel1LastSample = out_1;

    float revOut_0, revOut_1;
    verb.Process(out_0, out_1, &revOut_0, &revOut_1);

    out[0][i] = revOut_0;
    out[1][i] = revOut_1;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1)
      ;
  }
  Serial.println("MPR121 found!");

  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();
  /*
  for (int i = 0; i < 12; i++) {
    pinMode(switchSliderPins[i], INPUT_PULLUP);
  }
*/

  /*
  pinMode(SW_MODE_0, INPUT_PULLUP);
  pinMode(SW_MODE_1, INPUT_PULLUP);

  pinMode(SW_FDBK0_0, INPUT_PULLUP);
  pinMode(SW_FDBK0_1, INPUT_PULLUP);

  pinMode(SW_FDBK1_0, INPUT_PULLUP);
  pinMode(SW_FDBK1_1, INPUT_PULLUP);
*/
  for (int i = 0; i < 12; i++) {
    filters[i] = new Svf();
    filters[i]->Init(sample_rate);
    filters[i]->SetRes(resonances[i]);
    filters[i]->SetDrive(drives[i]);
    filters[i]->SetFreq(filterFrequencies[i]);

    secondFilters[i] = new Svf();
    secondFilters[i]->Init(sample_rate);
    secondFilters[i]->SetRes(resonances[i]);
    secondFilters[i]->SetDrive(drives[i]);
    secondFilters[i]->SetFreq(filterFrequencies[i]);
  }

  verb.Init(sample_rate);
  verb.SetFeedback(0.85f);
  verb.SetLpFreq(18000.0f);

  DAISY.begin(ProcessAudio);

  getInitialValues();
}

void loop() {
  // put your main code here, to run repeatedly:
  //readAnalogues();
  //readDigitals();
  readSliders();
  capacitiveTouchRead();
  delay(10);
}

void getInitialValues() {

  for (int i = 0; i < 12; i++) {
    for (int iter = 0; iter < 16 + 4; iter++) {
      if (iter > 3)
        initialValues[i] += cap.filteredData(i);
    }
  }
  for (int i = 0; i < 12; i++) {
    initialValues[i] = initialValues[i] / 16;
  }
}

void readSliders() {
  leftSliderValue = simpleAnalogRead(SLIDER_RIGHT);
  rightSliderValue = simpleAnalogRead(SLIDER_LEFT);
}

void readAnalogues() {

  for (int i = 0; i < 12; i++) {
    filterFactors[i] = simpleAnalogReadBank(filterSliderPins[i]);
  }

#ifdef DEBUG_ANALOG_PIN

  for (int i = 0; i < 12; i++) {
    Serial.print(filterFactors[i]);
    Serial.print(", ");
  }
  Serial.println();
#endif
}

void readDigitals() {
  for (int i = 0; i < 12; i++) {
    filterSwitchStatus[i] = digitalRead(switchSliderPins[i]);
  }

  uint8_t swMode0 = digitalRead(SW_MODE_0);
  uint8_t swMode1 = digitalRead(SW_MODE_1);

  if (swMode0 == 1 && swMode1 == 1)
    switchMode = 1;
  else if (swMode0 == 0 && swMode1 == 1)
    switchMode = 0;
  else if (swMode0 == 1 && swMode1 == 0)
    switchMode = 2;

  uint8_t swfdbk0_0 = digitalRead(SW_FDBK0_0);
  uint8_t swfdbk0_1 = digitalRead(SW_FDBK0_1);

  uint8_t swfdbk1_0 = digitalRead(SW_FDBK1_0);
  uint8_t swfdbk1_1 = digitalRead(SW_FDBK1_1);

  if (swfdbk0_0 == 1 && swfdbk0_1 == 1)
    switchfdbk0 = 1;
  else if (swfdbk0_0 == 0 && swfdbk0_1 == 1)
    switchfdbk0 = 0;
  else if (swfdbk0_0 == 1 && swfdbk0_1 == 0)
    switchfdbk0 = 2;

  if (swfdbk1_0 == 1 && swfdbk1_1 == 1)
    switchfdbk1 = 1;
  else if (swfdbk1_0 == 0 && swfdbk1_1 == 1)
    switchfdbk1 = 0;
  else if (swfdbk1_0 == 1 && swfdbk1_1 == 0)
    switchfdbk1 = 2;


#ifdef DEBUG_DIGITAL_PIN

  for (int i = 0; i < 12; i++) {
    Serial.print(filterSwitchStatus[i]);
    Serial.print(", ");
  }
  Serial.print("mode: ");
  Serial.print(switchMode);
  Serial.print(",feedback channel0: ");
  Serial.print(switchfdbk0);
  Serial.print(",feedback channel1: ");
  Serial.print(switchfdbk1);
  Serial.println();
#endif
}

void capacitiveTouchRead() {
  int32_t value;
  float fValue;
  for (int i = 0; i < 12; i++) {
    value = initialValues[i] - cap.filteredData(i);
    if (value < 0)
      value = 0;
    fValue = value / 100.0;
    if (fValue < 0) fValue = 0.0f;
    else if (fValue > 1.5) fValue = 1.5f;
    touchValueFactors[touchIndex[i]] = fValue;
  }
}

float simpleAnalogReadBank(uint32_t pin) {
  return 0.7 * ((float)analogRead(pin) / 1023);
}
float simpleAnalogRead(uint32_t pin) {
  return 1.0 * ((float)analogRead(pin) / 1023);
}
