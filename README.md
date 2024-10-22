
# simple_filter_bank

The simple_filter_bank or lx-bank-001 is a digital filter bank (with unique feedback resonance implementation *and vocoder mode!*) based on the [daisy seed](https://electro-smith.com/products/daisy-seed) by electrosmith and the [Simple Designer](https://www.synthux.academy/#Designer) by Synthux Academy.

Alternatively, the software can be run on the [Simple Touch](https://www.synthux.academy/#Touch) also by Synthux Academy.

![lx-bank](/Pictures/lx-bank.png)


- [simple\_filter\_bank](#simple_filter_bank)
  - [About](#about)
  - [Software](#software)
    - [Requirement](#requirement)
    - [Simple Designer version](#simple-designer-version)
    - [Simple Touch version](#simple-touch-version)
    - [Mode Description](#mode-description)
  - [Hardware](#hardware)
    - [Simple Designer Assembly](#simple-designer-assembly)
    - [Simple Touch Assembly](#simple-touch-assembly)
  - [License](#license)

## About

Making a digital filter bank adds the possibility to tune the filter perfectly. Thanks to this ability, the lx-bank-001 is tuned following the circle of fifth with 12 banks tuned accordingly to G3, D3, A3, E4, B4, F#5, C#6, G#6, D#7, A#7, F8, C9.

This make the 12 banks cover a large spectrum up to 8.4kHz. The whole filter bank is made resonant with a feedback loop inspired by the Make Noise Music QPAS filter. The feedback can either be disabled, set to low or high.

When the filter bank feedback is set to high, the filter bank become a unique drone like instrument where two adjacent faders are distant of a fifth and therefore will sound beautiful to your ears.

## Software

### Requirement

The synth is based on a [Daisy seed](https://www.electro-smith.com/daisy/daisy) by electro-smith. It's Arduino based and uses electrosmith Arduino library [DaisyDuino](https://github.com/electro-smith/DaisyDuino).
For the [Simple Touch Version](#simple-touch-version), you will also need the [Adafruit_MPR121](https://github.com/adafruit/Adafruit_MPR121) library.

### Simple Designer version

The Simple Designer version has two monos inputs and two monos outputs. Each filter bank has a fader controlling the strength of the filter and a switch that can enable or disable the filter bank.
Additionally, a switch controls the [Mode](#mode-description), routing the input and output differently, and the feedback level: disabled, low or high.

### Simple Touch version

The Simple Touch version has a stereo input and a stereo output. Each filter bank has a touchpad controlling the strength of the filter. The resonant is by default set to high making it mainly a touch based drone instrument. The [Mode](#mode-description) is set to 0 and can't be changed.
Additionally, the two faders of the simple touch controls:

- left slider: input gain
- right slider: additional reverb strength

### Mode Description

- 0 : input 0 goes to out 0, input 1 goes to out 1. Switch enable or disable filters on both output
  - In mode 0, if all the filters are disabled, you will enter in vocoder mode. Input 0 for modulator(voice), Input 1 for carrier (oscillator).
- 1 : input 0 goes to out 0 and 1 isn't used Switch enable or disable filters only on output 1, output 0 has all filters enabled
- 2 : input 0 and 1 are mixed to out 0 and 1 Switch enable or disable filters only on output 1, output 0 has all filters enabled

## Hardware

### Simple Designer Assembly

The Simple Designer version has a custom 32HP eurorack compatible PCB front panel. The Simple Designer PCB is 42HP so take your best saw or dremel and cut the excess 10HP.
Under the [Hardware/panel-pcb](/Hardware/panel-pcb/) folder, you will find the KiCad project that will allow you to print the PCB.

![front-pannel-kicad-render](/Pictures/front-pannel.png)

In my case, I recommend using [PCBWay](pcbway.com/) services. I chose to go with a white solder mask, black silkscreen and HASL surface finish.

**CAUTION, the daisy is soldered on the bottom to have enough room. The pinning is then the opposite of standard mounting of the daisy on Simple Designer.**
Follow the following table if you want to create a version of the lx-bank-001 compatible with the front panel and firmware.

| Description | Components | Gpios | Simple Socket | Simple Pin |
|:---------------------------------------:|:----------------:|:---------:|:-------------:|:----------:|
| in 1 | jack mono | AudioIn1 | 5 | 33 |
| in 2 | jack mono | AudioIn2 | 10 | 32 |
| out 1 | jack mono | AudioOut1 | 55 | 31 |
| out 2 | jack mono | AudioOut2 | 60 | 30 |
| mode selection | Switch 3 pos 1 | 12 | 20 | 36 |
| mode selection | Switch 3 pos 2 | 13 | 25 | 35 |
| feedback selection 1 | Switch 3 pos 1 | 29 | 40 | 5 |
| feedback selection 1 | Switch 3 pos 2 | 30 | 35 | 4 |
| feedback selection 2 | Switch 3 pos 1 | 26 | 50 | 8 |
| feedback selection 2 | Switch 3 pos 2 | 27 | 45 | 7 |
| power on led, 3V3 and GND with resistor | LED | None | 15 | None |
| filter bank 0 on/mute | Switch 2 pos | 0 | 1 | 48 |
| filter bank 1 on/mute | Switch 2 pos | 1 | 6 | 47 |
| filter bank 2 on/mute | Switch 2 pos | 2 | 11 | 46 |
| filter bank 3 on/mute | Switch 2 pos | 3 | 16 | 45 |
| filter bank 4 on/mute | Switch 2 pos | 4 | 21 | 44 |
| filter bank 5 on/mute | Switch 2 pos | 5 | 26 | 43 |
| filter bank 6 on/mute | Switch 2 pos | 6 | 31 | 42 |
| filter bank 7 on/mute | Switch 2 pos | 7 | 36 | 41 |
| filter bank 8 on/mute | Switch 2 pos | 8 | 41 | 40 |
| filter bank 9 on/mute | Switch 2 pos | 9 | 46 | 39 |
| filter bank 10 on/mute | Switch 2 pos | 10 | 51 | 38 |
| filter bank 11 on/mute | Switch 2 pos | 11 | 56 | 37 |
| filter bank 0 strength | Fader | A11 | 4 | 6 |
| filter bank 1 strength | Fader | A10 | 9 | 9 |
| filter bank 2 strength | Fader | A9 | 14 | 10 |
| filter bank 3 strength | Fader | A8 | 19 | 11 |
| filter bank 4 strength | Fader | A7 | 24 | 12 |
| filter bank 5 strength | Fader | A6 | 29 | 13 |
| filter bank 6 strength | Fader | A5 | 34 | 14 |
| filter bank 7 strength | Fader | A4 | 39 | 15 |
| filter bank 8 strength | Fader | A3 | 44 | 16 |
| filter bank 9 strength | Fader | A2 | 49 | 17 |
| filter bank 10 strength | Fader | A1 | 54 | 18 |
| filter bank 11 strength | Fader | A0 | 59 | 19 |

![pcb-placement-layout](/Pictures/pcb-placement-layout.png)
![PCB-bottom](/Pictures/PCB-bottom.jpeg)
![PCB-top](/Pictures/PCB-top.jpeg)

### Simple Touch Assembly

You'll need to assemble:

- The left fader
- The right fader
- The input stereo jack
- The output stereo jack
- The MPR121 board

## License

MIT License

Copyright (c) 2024 Lucas Bonvin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
