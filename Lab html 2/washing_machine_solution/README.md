# ECET 279 Labs 4-5: Washing-Machine Controller

This guide follows the procedures in `Lab_4_5_Washing_Machine_Accessible.html`.
All code, identifiers, and comments are in English.

## 1. Important assumptions

- Microcontroller: ATmega2560 at 16 MHz.
- Input switches connect the selected pin to ground when active.
- Internal pull-up resistors are enabled, so inactive is logic 1 and active is
  logic 0.
- `DOOR_OPEN` is active when the door is physically open.
- Stepper hardware and patterns are reused from Lab 3.
- The lab's operational table says spin uses a 2 ms full-step delay, while
  Procedure 3 says 4 ms. `stepper.c` follows the table and provides one named
  constant that can be changed to 4 if the instructor prefers Procedure 3.

If the physical switch board produces logic 1 when active, do not change every
condition. Change `input_is_active()` so it returns `!= 0`, and remove the
inversion in `read_temperature_selection()`.

## 2. Hardware assignment

| Function | ATmega2560 pin | Direction | Active state |
|---|---:|---|---|
| Hot selection | PA0 | Input | Low |
| Warm selection | PA1 | Input | Low |
| Cold selection | PA2 | Input | Low |
| Door open | PA3 | Input | Low |
| Start pushbutton | PA4 | Input | Low |
| Stepper IN1 | PD0 | Output | Pattern controlled |
| Stepper IN2 | PD1 | Output | Pattern controlled |
| Stepper IN3 | PD2 | Output | Pattern controlled |
| Stepper IN4 | PD3 | Output | Pattern controlled |
| Drain valve | PC0 | Output | High |
| Hot valve | PC1 | Output | High |
| Cold valve | PC2 | Output | High |
| Done LED | PC3 | Output | High |
| Agitate LED | PC4 | Output | High |
| Spin LED | PC5 | Output | High |

## 3. Create the project folder

1. Create `ECET279_Lab4_Wash_Embed_Sys_Dev`.
2. In Microchip Studio, create an ATmega2560 GCC C executable project named
   `Wash_MC_Sys_Dev`.
3. Copy `Control_stg_1.c` into the project and make it the only file containing
   `main()`.
4. Copy the Lab 3 `Debugger.c` and `Debugger.h` files into the project.
5. Add `Debugger.c` to the project through **Add > Existing Item**.
6. Define `USE_DEBUGGER` in the project symbols, or remove the surrounding
   `#if defined(USE_DEBUGGER)` lines after the debugger files are available.

Why: a C project may contain many modules, but it can contain only one `main()`.
The stage files are separate versions, not files to build together.

## 4. Procedure 1: initialize and test I/O

Use `Control_stg_1.c`.

### Step 4.1: define the clock and include libraries

```c
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
```

`F_CPU` lets `_delay_ms()` calculate timing for the 16 MHz clock. `avr/io.h`
provides register names such as `DDRA`, `PORTA`, and `PINA`.

### Step 4.2: assign every pin a name

The `#define` statements at the top of the file connect meaningful names such
as `HOT_VALVE_BIT` to physical port bits such as `PC1`. This makes the program
readable and puts wiring changes in one location.

### Step 4.3: configure direction registers

```c
DDRA &= (uint8_t)~INPUT_MASK;
PORTA |= INPUT_MASK;
DDRC |= CONTROL_OUTPUT_MASK;
PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;
DDRD |= STEPPER_MASK;
PORTD &= (uint8_t)~STEPPER_MASK;
```

A zero in a DDR bit creates an input. A one creates an output. Writing ones to
`PORTA` after making the pins inputs enables their pull-up resistors. Every
output is explicitly turned off during initialization for safety.

### Step 4.4: verify outputs

`test_outputs()` activates PC0-PC5 one at a time and then PD0-PD3 one at a
time. In the debugger, verify the corresponding port bit becomes one. On the
hardware, verify the correct label/output responds.

### Step 4.5: verify inputs

After the startup output test, the infinite loop shows each input on a related
LED. Activate one input at a time and verify its `PINA` bit becomes zero in the
debugger.

## 5. Test plan

Record the observed result and Pass/Fail during the demonstration.

| Test | Specification | How to test | Expected result |
|---:|---|---|---|
| 1 | Hot fill | Select only Hot and start with the door closed | Hot valve on for 2 s; cold valve off |
| 2 | Warm fill | Select only Warm and start with the door closed | Both water valves on for 2 s |
| 3 | Cold fill | Select only Cold and start with the door closed | Cold valve on for 2 s; hot valve off |
| 4 | Invalid temperature | Select none or select two temperature switches | Controller waits; no water valve opens |
| 5 | Door interlock | Press Start while the door is open, then close it | Cycle waits while open and begins after closing |
| 6 | Complete sequence | Start a valid cycle and time every state | Fill 2 s, agitate 8 s, drain 1 s, fill 2 s, agitate 4 s, drain 1 s, spin 4 s |
| 7 | Restart interlock | Finish a cycle without opening the door | Done stays on and a second cycle cannot start |
| 8 | Motor action | Observe both motor modes | Agitate reverses each second; spin is continuous clockwise full-step |

## 6. Procedure 2: build the program skeleton

Replace the first stage file with `Control_stg_2_skeleton.c`. Do not add
`Control_final.c` to this project yet because both files contain `main()`.

### Step 6.1: wait for Start

`wait_for_start()` stays in a loop while the active-low pushbutton is not
pressed. It then waits for release so a held button cannot be interpreted as a
future press.

### Step 6.2: enforce the door interlock

`wait_for_door_closed()` loops while `DOOR_OPEN` is active. This implements the
provided flowchart: a Start press with an open door does not operate anything;
the program waits for the door to close.

### Step 6.3: validate temperature selection

`read_temperature_selection()` accepts only these exact bit patterns:

```text
HOT  = 001
WARM = 010
COLD = 100
```

Zero selected switches and every multiple-switch combination remain trapped in
the loop. Equality comparisons and logical OR (`||`) are used to decide whether
one complete condition is valid. Bitwise operations are used only to extract
the three input bits.

### Step 6.4: operate the water valves

`fill_for_two_seconds()` calls the validated selection function, turns on hot,
cold, or both valves, waits two seconds, and turns both valves off. It is called
again before rinse, so the operator may change the temperature between fills.

### Step 6.5: substitute LEDs for the unfinished motor

`temporary_motor_operation()` lights the Agitate or Spin LED for the required
time. This allows the complete state sequence to be checked before motor logic
is integrated.

### Step 6.6: complete the non-motor sequence

The skeleton performs this exact order:

```text
Start -> Door closed -> Fill 2 s -> Agitate indicator 8 s
      -> Drain 1 s -> Fill 2 s -> Agitate indicator 4 s
      -> Drain 1 s -> Spin indicator 4 s -> Done
```

After Done turns on, the program waits until the door opens. Opening the door
turns Done off and returns to the Start wait.

### Step 6.7: use the simulator

Temporarily comment out the `_delay_ms(1);` line inside `delay_seconds()` to
make the sequence run quickly. Place breakpoints at the beginning of each state
or single-step while watching `PINA`, `PORTC`, and `PORTD`. Restore the delay
before programming the hardware.

## 7. Procedure 3: implement the motor module

Create the second project or save a copy before changing the working skeleton.
Add `stepper.c` and `stepper.h`.

### Step 7.1: expose a small module interface

```c
void stepper_init(void);
void motor_run(char mode, uint8_t seconds);
void motor_off(void);
```

`motor_run()` meets the required two-parameter design: the mode is `'A'` or
`'S'`, and the second parameter is the operation time in seconds. Any other
mode turns the motor off.

### Step 7.2: implement agitation

Agitation uses the eight-pattern half-step array and a 4 ms inter-step delay.
One second contains `1000 / 4 = 250` half-steps. Even-numbered one-second
sections traverse the array forward; odd-numbered sections traverse it in
reverse. The direction therefore changes every second.

### Step 7.3: implement spin

Spin uses the four-pattern full-step array and always traverses it forward.
With the table's 2 ms inter-step delay, each second contains `1000 / 2 = 500`
full steps. If the observed direction is counter-clockwise because of the coil
wiring order, reverse the array traversal or reverse the four driver leads.

### Step 7.4: preserve unrelated port bits

```c
STEPPER_PORT = (STEPPER_PORT & 0xF0) | (pattern & STEPPER_MASK);
```

The mask preserves PD4-PD7 and replaces only PD0-PD3. This is the required
bit-masking technique.

## 8. Procedure 4: integrate the complete controller

Use these three files in the final project:

- `Control_final.c`
- `stepper.c`
- `stepper.h`

Remove the skeleton file from the build. Add `Debugger.c` and `Debugger.h` if
the instructor requires the debugger during the final check-off.

The final main program replaces each temporary indicator call with
`run_motor_with_indicator()`. That function lights the correct status LED,
calls the motor module for the required number of seconds, and then turns the
indicator off.

## 9. Final check-off sequence

1. Label all five inputs and all outputs.
2. Show the completed hardware table.
3. Show the test plan with observed results and Pass/Fail entries.
4. Demonstrate Hot, Warm, and Cold separately.
5. Demonstrate that zero or multiple temperature selections do nothing.
6. Demonstrate that an open door blocks operation.
7. Demonstrate 8-second agitation, including a direction change every second.
8. Demonstrate both one-second drain operations.
9. Demonstrate 4-second rinse agitation.
10. Demonstrate 4-second clockwise full-step spin.
11. Demonstrate Done remaining on until the door opens.
12. Show that all code is divided into functions and fully commented.

## 10. Common troubleshooting checks

- **Every input always reads zero:** the board may be wired active-high or may
  be shorted to ground. Check `PINA` before changing the program.
- **The cycle never leaves temperature selection:** exactly one selector must
  be active. Inspect only the low three bits of `PINA`.
- **Warm opens only one valve:** verify both PC1 and PC2 are connected and that
  the Warm branch uses bitwise OR to set both output bits.
- **The motor vibrates but does not rotate:** verify IN1-IN4 order and common
  ground between the ATmega2560 and ULN2003 board.
- **The motor turns the wrong direction:** reverse array traversal or correct
  the IN1-IN4 wiring order.
- **The program restarts immediately:** verify that PA3 truly becomes active
  when the door opens and that Done waits for this condition.
- **Build reports multiple definitions of `main`:** include only one control
  stage file in a project at a time.
