#include "DaisyDuino.h"

using namespace daisysp;

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

//#define DEBUG_ANALOG_PIN
//#define DEBUG_DIGITAL_PIN

float sample_rate;

float filterFactors[12];
uint8_t filterSwitchStatus[12];

uint8_t switchMode = 0;

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
float filterFrequencies[12] = { 100, 158, 249, 392, 618, 975, 1538, 2425, 3825, 6032, 9512, 15000 };


void ProcessAudio(float** in, float** out, size_t size) {

  for (size_t i = 0; i < size; i++) {

    float in_0;
    float in_1;

    if (switchMode == 0) {
      in_0 = in[0][i];
      in_1 = in[1][i];
    } else if (switchMode == 1) {
      in_0 = in[0][i];
      in_1 = in[0][i];
    } else if (switchMode == 2) {
      in_0 = in[0][i] + in[1][i];
      in_1 = in_0;
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

      if (i == 0) {
        out_0 += filters[i]->Low() * filterFactors[i] * additionalFactor_0;
        out_1 += secondFilters[i]->Low() * filterFactors[i] * additionalFactor_1;

      } else if (i == 11) {
        out_0 += filters[i]->High() * filterFactors[i] * additionalFactor_0;
        out_1 += secondFilters[i]->High() * filterFactors[i] * additionalFactor_1;

      } else {
        out_0 += filters[i]->Band() * filterFactors[i] * additionalFactor_0;
        out_1 += secondFilters[i]->Band() * filterFactors[i] * additionalFactor_1;
      }
    }
    out[0][i] = out_0;
    out[1][i] = out_1;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();

  for (int i = 0; i < 12; i++) {
    pinMode(switchSliderPins[i], INPUT_PULLUP);
  }

  pinMode(SW_MODE_0, INPUT_PULLUP);
  pinMode(SW_MODE_1, INPUT_PULLUP);

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

  DAISY.begin(ProcessAudio);
}

void loop() {
  // put your main code here, to run repeatedly:
  readAnalogues();
  readDigitals();
  delay(10);
}

void readAnalogues() {

  for (int i = 0; i < 12; i++) {
    filterFactors[i] = simpleAnalogRead(filterSliderPins[i]);
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
    switchMode = 2;
  else if (swMode0 == 1 && swMode1 == 0)
    switchMode = 0;

#ifdef DEBUG_DIGITAL_PIN

  for (int i = 0; i < 12; i++) {
    Serial.print(filterSwitchStatus[i]);
    Serial.print(", ");
  }
  Serial.print("mode: ");
  Serial.print(switchMode);
  Serial.println();
#endif
}

float simpleAnalogRead(uint32_t pin) {
  return 0.7 * ((float)analogRead(pin) / 1023);
}
