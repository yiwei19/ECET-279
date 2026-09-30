# Lab 7 — ADC Control

This lab builds a reusable, polling-based Analog-to-Digital Converter (ADC)
module for the ATmega2560. The finished program reads three potentiometers on
ADC channels 1, 2, and 3, displays the selected 10-bit ADC result on eight LEDs,
and uses one pushbutton to select the active channel.

Keep the breadboard and the finished code after this lab. The ADC module and the
Timer/PWM code from Lab 6 will be reused in the next lab to control DC-motor
speed and other outputs.

## Performance Check

- **Check Off 1 — 30 points:** one ADC channel displayed on eight LEDs
- **Check Off 2 — 30 points:** pushbutton selection among three ADC channels
- **Check Off 3 — 30 points:** troubleshooting exercise

---

# Before Starting

## Required Lab 6 Code

Have the completed Lab 6 Check Off 1 and Check Off 2 code available:

- `Timer.c`
- `Timer.h`
- the custom Timer0 polling delay
- the Timer1 PWM initialization and duty-cycle function

Lab 7 itself does not require PWM for the first two check offs, but the final ADC
module should be reusable with the Lab 6 PWM module in the next lab.

## Required Hardware

- ATmega2560 training board
- three potentiometers
- one pushbutton for channel selection
- eight LEDs with current-limiting resistors
- breadboard and jumper wires
- debugger/programmer
- ATmega2560 datasheet

## Recommended Project Structure

Create a new Microchip Studio project and keep the ADC code outside `main.c`:

```text
Lab7/
├── checkoff1.c       # Use this as main.c for Check Off 1
├── checkoff2.c       # Use this as main.c for Check Off 2
├── ADC.c             # ADC initialization and conversion functions
└── ADC.h             # ADC public interface
```

Only add one file containing `main()` to the Microchip Studio build at a time.

---

# I/O Connections

Complete the physical pin column before wiring. The lab sheet fixes the ADC
inputs, but it does not assign a port to the eight LEDs or an MCU pin to the
channel-select pushbutton.

| Hardware | MCU signal | Direction | Notes |
|---|---|---|---|
| Potentiometer 1 wiper | PF1 / ADC1 | Input | ADC channel 1 |
| Potentiometer 2 wiper | PF2 / ADC2 | Input | ADC channel 2 |
| Potentiometer 3 wiper | PF3 / ADC3 | Input | ADC channel 3 |
| Channel-select pushbutton | `________` | Input | One button, not three switches |
| Test LED bit 0 | `________` | Output | Displays ADC bit 2 after shifting |
| Test LED bit 1 | `________` | Output | Displays ADC bit 3 after shifting |
| Test LED bit 2 | `________` | Output | Displays ADC bit 4 after shifting |
| Test LED bit 3 | `________` | Output | Displays ADC bit 5 after shifting |
| Test LED bit 4 | `________` | Output | Displays ADC bit 6 after shifting |
| Test LED bit 5 | `________` | Output | Displays ADC bit 7 after shifting |
| Test LED bit 6 | `________` | Output | Displays ADC bit 8 after shifting |
| Test LED bit 7 | `________` | Output | Displays ADC bit 9 after shifting |

Each potentiometer must be wired as a voltage divider:

```text
5 V ---- outer terminal
          potentiometer
GND ---- outer terminal
          wiper ---- PF1, PF2, or PF3
```

Do not connect an ADC pin directly to a voltage above 5 V or below ground.

## I/O Initialization Checklist

- [ ] Configure PF1, PF2, and PF3 as inputs.
- [ ] Disable the digital input buffer on ADC1, ADC2, and ADC3 with `DIDR0`.
- [ ] Do not enable pull-up resistors on PF1, PF2, or PF3.
- [ ] Configure all eight LED pins as outputs.
- [ ] Configure the channel-select pushbutton as an input.
- [ ] If the button uses the internal pull-up, document that it is active-low.
- [ ] Verify the button and LED connections in the Debugger before testing ADC.

---

# Test Plan

Complete the observed-result and pass/fail columns during the lab.

| Test | Specification | How to Test | Expected Result | Observed Result | Pass/Fail |
|---:|---|---|---|---|---|
| 1 | LED output wiring works | Write `0x00`, `0x55`, `0xAA`, and `0xFF` to the LED port | LEDs show the four corresponding patterns | | |
| 2 | Potentiometer 1 is read on ADC1 | Select channel 1 and rotate potentiometer 1 from minimum to maximum | LED value moves smoothly from about `0x00` to about `0xFF` | | |
| 3 | ADC result is 10-bit | Watch the returned value in the Debugger while rotating a potentiometer | Value remains in the range 0–1023 | | |
| 4 | Channel changes only on release | Press and hold the select button, then release it once | Channel advances exactly once on release | | |
| 5 | All three channels are independent | Select channels 1, 2, and 3 and move only the matching potentiometer | LEDs follow only the currently selected potentiometer | | |

---

# Check Off 1 — One ADC Channel on LEDs

## Goal

Use ADC1 / PF1 to perform a 10-bit, single-ended conversion over the 0–5 V
range. Poll the `ADIF` flag and display the upper eight bits of the result on the
eight test LEDs.

## 1. ADC Requirements

- ADC mode: single-ended input
- Initial channel: ADC1 / PF1
- Resolution: 10 bits
- Input range: 0–5 V
- Voltage reference: AVCC, matching the lab's 0–5 V requirement
- ADC prescaler: 128
- Completion method: polling the `ADIF` flag
- Required prototype:

```c
uint16_t ADC_convert(uint8_t channel);
```

## 2. ADC Clock Calculation

For the course board:

```text
F_CPU = 16,000,000 Hz
prescaler = 128

F_ADC = F_CPU / prescaler
      = 16,000,000 / 128
      = 125,000 Hz
```

Set `ADPS2:ADPS0 = 111` in `ADCSRA` to select the divide-by-128 prescaler.

## 3. Important ADC Registers

| Register | Purpose in this lab |
|---|---|
| `DDRF` | Configure PF1, PF2, and PF3 as inputs |
| `PORTF` | Keep ADC pin pull-ups disabled |
| `DIDR0` | Disable digital input buffers on ADC1–ADC3 |
| `ADMUX` | Select AVCC reference and ADC channel |
| `ADCSRB` | Keep the extra channel-select bit and auto-trigger configuration correct |
| `ADCSRA` | Enable ADC, start conversion, select prescaler, and poll/clear `ADIF` |
| `ADC` | Read the combined 10-bit ADC result |

Use right-adjusted ADC data (`ADLAR = 0`) so reading the 16-bit `ADC` register
returns a value from 0 through 1023.

## 4. ADC Initialization Sequence

The initialization function should:

1. Configure PF1, PF2, and PF3 as inputs.
2. Disable their pull-up resistors.
3. Disable their digital input buffers in `DIDR0`.
4. Select AVCC as the voltage reference with `REFS0 = 1`.
5. Keep the ADC result right-adjusted with `ADLAR = 0`.
6. Disable auto-triggering because the program starts conversions manually.
7. Select the divide-by-128 prescaler.
8. Enable the ADC with `ADEN = 1`.

Recommended public function:

```c
void ADC_init(void);
```

After calling `ADC_init()`, stop in the Debugger and verify `ADMUX`, `ADCSRA`,
`ADCSRB`, `DDRF`, `PORTF`, and `DIDR0` before continuing.

## 5. Conversion Function Sequence

`ADC_convert(channel)` should:

1. Validate that the requested channel is supported.
2. Change only the channel-selection bits in `ADMUX`.
3. Clear any old `ADIF` flag by writing a one to `ADIF`.
4. Start a conversion by writing a one to `ADSC`.
5. Poll until `ADIF` becomes one.
6. Read and save the 10-bit `ADC` result.
7. Clear `ADIF` by writing a one to it.
8. Return the saved `uint16_t` result.

Important: AVR status flags such as `ADIF` are cleared by writing a **one** to
the flag. Do not use a read-modify-write expression that can accidentally clear
other pending flags.

## 6. Display the 10-Bit Value on Eight LEDs

The ADC result ranges from 0 to 1023, but the LED bank displays only eight bits.
Discard the two least-significant bits:

```c
uint16_t adc_value = ADC_convert(1);
uint8_t led_value = (uint8_t)(adc_value >> 2);
LED_PORT = led_value;
```

Expected approximate values:

| Potentiometer voltage | 10-bit ADC result | LED value after `>> 2` |
|---:|---:|---:|
| 0 V | 0 | `0x00` |
| 1.25 V | 256 | `0x40` |
| 2.50 V | 512 | `0x80` |
| 3.75 V | 767 | `0xBF` |
| 5 V | 1023 | `0xFF` |

The exact readings may vary slightly because of supply voltage, potentiometer
tolerance, electrical noise, and ADC quantization.

## Check Off 1 — Final Checklist

- [ ] I/O table is complete and included in the code comments.
- [ ] Test plan is complete.
- [ ] PF1 / ADC1 is wired to potentiometer 1.
- [ ] ADC uses 10-bit, right-adjusted results.
- [ ] ADC prescaler is 128.
- [ ] `F_ADC` is calculated as 125 kHz for a 16 MHz CPU clock.
- [ ] ADC conversion uses `ADIF` polling.
- [ ] `ADIF` is cleared correctly by writing a one.
- [ ] `ADC_convert(uint8_t channel)` returns a `uint16_t`.
- [ ] ADC initialization and conversion code are in `ADC.c` and `ADC.h`.
- [ ] The eight LEDs display `(ADC_convert(1) >> 2)`.
- [ ] Rotating potentiometer 1 changes the LED value smoothly.
- [ ] ADC registers were verified in the Debugger.
- [ ] Can explain every ADC register used.

→ **CALL INSTRUCTOR FOR CHECK OFF 1**

---

# Check Off 2 — Three-Channel Selection

## Goal

Use one pushbutton to cycle through ADC1, ADC2, and ADC3. The selected channel
must advance when the pushbutton is **released**, not when it is pressed.

## 1. Channel State

Store the channel as an integer from 1 through 3:

```c
uint8_t channel = 1;
```

Each valid release advances to the next channel:

```text
ADC1 -> ADC2 -> ADC3 -> ADC1 -> ...
```

A simple wraparound update is:

```c
channel++;
if (channel > 3)
{
    channel = 1;
}
```

## 2. Release-Edge Behavior

Do not increment continuously while the button is held. Track the previous
button state and change the channel only on the pressed-to-released transition.

For an active-low button:

```text
released = logic 1
pressed  = logic 0

previous  current  action
   1         1     none
   1         0     remember that the button is down
   0         0     none; continue waiting
   0         1     advance exactly one channel
```

The button may bounce mechanically. A short debounce delay after detecting the
release, or a stable-state debounce routine, can prevent one release from being
counted more than once.

## 3. Verify Selection Before ADC Testing

Before connecting channel selection to `ADC_convert()`, temporarily display the
selected channel on the LEDs:

```text
channel 1 -> LED value 0x01
channel 2 -> LED value 0x02
channel 3 -> LED value 0x04
```

Press and release the button repeatedly. Confirm that the display advances once
per release and wraps from channel 3 back to channel 1.

## 4. Read the Selected Potentiometer

After the button logic works, replace the temporary channel pattern with:

```c
uint16_t adc_value = ADC_convert(channel);
LED_PORT = (uint8_t)(adc_value >> 2);
```

Test independence carefully:

1. Select ADC1 and rotate only potentiometer 1.
2. Select ADC2 and rotate only potentiometer 2.
3. Select ADC3 and rotate only potentiometer 3.
4. Return to ADC1 and confirm the cycle repeats.

## 5. Multiplexer Precaution

When the ADC multiplexer changes from one potentiometer to another, the first
sample may still be influenced by the previous channel, especially when the
source impedance is high. If readings are unstable after switching channels,
perform one conversion and discard it, then use the second conversion. Start
with the simpler single-conversion version and add this only if measurement
behavior shows it is needed.

## Check Off 2 — Final Checklist

- [ ] Potentiometer 1 wiper is connected to PF1 / ADC1.
- [ ] Potentiometer 2 wiper is connected to PF2 / ADC2.
- [ ] Potentiometer 3 wiper is connected to PF3 / ADC3.
- [ ] One pushbutton selects all three channels.
- [ ] Selection order is ADC1 → ADC2 → ADC3 → ADC1.
- [ ] Channel changes on button release, not button press.
- [ ] Holding the button does not repeat the channel change.
- [ ] Button bounce does not cause obvious skipped channels.
- [ ] LED channel patterns were tested before ADC selection was enabled.
- [ ] Each potentiometer independently controls the LEDs when selected.
- [ ] Program remains multimodule.
- [ ] Can explain the channel-selection state logic.
- [ ] Can explain how `ADMUX` selects the ADC channel.

→ **CALL INSTRUCTOR FOR CHECK OFF 2**

---

# Check Off 3 — Troubleshooting

The troubleshooting scenario is supplied by the instructor and cannot be
completed fully in advance.

## Troubleshooting Procedure

1. Record the expected hardware behavior before changing any code.
2. Build the instructor-provided code and record warnings or errors.
3. Observe the actual LEDs, selected channel, and ADC values.
4. Inspect the relevant ADC and GPIO registers in the Debugger.
5. Reduce the problem to initialization, conversion, display, or button logic.
6. Change one item at a time and retest.
7. Explain both the original problem and why the fix works.

## Common ADC Problems to Check

- ADC pin accidentally configured as an output
- pull-up resistor left enabled on an analog input
- wrong voltage-reference bits
- wrong prescaler bits
- `ADEN` not set
- conversion never started with `ADSC`
- polling the wrong flag
- trying to clear `ADIF` by writing zero
- overwriting reference bits while changing `ADMUX`
- reading only `ADCL` or only `ADCH` instead of the full 10-bit result
- forgetting `>> 2` before writing the result to eight LEDs
- invalid or off-by-one channel number
- button edge detected on press instead of release
- no button-state memory, causing repeated channel changes while held
- LED data-direction register not configured as output

## Check Off 3 — Final Checklist

- [ ] Can describe the expected behavior.
- [ ] Can describe the observed incorrect behavior.
- [ ] Located the faulty section of code.
- [ ] Corrected the bug without breaking other features.
- [ ] Retested the repaired code on hardware.
- [ ] Can explain why the original code failed.
- [ ] Can explain why the correction works.

→ **CALL INSTRUCTOR FOR CHECK OFF 3 — LAB COMPLETE**

---

# Final Register Reference

The exact hexadecimal values depend on how the LED port and button are wired,
but the ADC configuration should express these choices:

```text
ADMUX
  REFS1:0 = 01   AVCC reference
  ADLAR   = 0    right-adjusted result
  MUX4:0  = channel number 1, 2, or 3

ADCSRA
  ADEN    = 1    ADC enabled
  ADSC    = 1    start each conversion
  ADATE   = 0    auto trigger disabled
  ADIF    = 1    conversion complete; write 1 to clear
  ADIE    = 0    interrupt not used; polling is required
  ADPS2:0 = 111  prescaler 128

ADCSRB
  MUX5    = 0    channels ADC0 through ADC7
  ADTS2:0 = 000  free-running source selection is irrelevant when ADATE = 0
```

# Final Lab Checklist

- [ ] Breadboard connections are labeled and secure.
- [ ] I/O table and test plan are complete.
- [ ] ADC module is reusable and does not contain application-specific LED code.
- [ ] No `_delay_ms()` is used for ADC conversion completion.
- [ ] Check Off 1 passed.
- [ ] Check Off 2 passed.
- [ ] Check Off 3 passed.
- [ ] Lab 6 Timer/PWM code and Lab 7 ADC code are saved for the next lab.
