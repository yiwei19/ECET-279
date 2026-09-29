# Lab 6 — Check Off 1

## 1. I/O Setup

### Pushbuttons
- Start pushbutton → PA4
- Stop pushbutton → PA5
- Test pushbutton → PA6

Initialize PA4, PA5, PA6 as inputs.

### Test LED
- Connect one LED as the test/mode LED.
- Initialize the LED pin as an output.


## 2. Create Timer Module

Create two files:

- `Timer.c`
- `Timer.h`

Timer0 initialization and the custom delay function go in this module.


## 3. Configure Timer0 for 1 ms

Write a Timer0 initialization function.

Need to determine:

- F_CPU = ___16 MHz_______
- Timer0 prescaler = ___64____
- Timer0 flag used = TOV0 or OCF0A/OCF0B => I choose OCF0A
- T_tick = 64/16,000,000 = 4 $\mu$ s
- so for 1ms, we need: 1000 $\mu$ s / 4 = 250 ticks
- TCNT0 starting value = _____0_____
- OR OCR0A/OCR0B value = _____249_____

Goal:

Timer0 must generate an event every **1 ms**.

Use the Debugger to verify the Timer0 register values after initialization.


## 4. Write the 1 ms Delay Function

Required behavior:

`delay_1ms(1)` → delay 1 ms

`delay_1ms(1000)` → delay 1000 ms

Prototype:

`void delay_1ms(uint16_t delay);`

Requirements:

- Must use Timer0
- Must use polling
- Must check the Timer0 flag
- Must NOT use `_delay_ms()`


## 5. Flash the LED

In `main()`:

1. Initialize I/O.
2. Initialize Timer0.
3. Enter `while(1)`.
4. Turn LED ON.
5. Call `delay_1ms(1000)`.
6. Turn LED OFF.
7. Call `delay_1ms(1000)`.
8. Repeat.

Expected behavior:

LED ON  → 1000 ms  
LED OFF → 1000 ms  
LED ON  → 1000 ms  
LED OFF → 1000 ms  
...


## 6. Multimodule Requirement

Project should at least contain:

main.c
Timer.c
Timer.h

Do NOT put all Timer code directly inside `main.c`.


## 7. Manual Calculation Sheet

Complete these before check off:

- [ ] Timer flag used
- [ ] F_CPU
- [ ] Prescaler
- [ ] TCNT0 starting value calculation
- [ ] OCR0A/OCR0B calculation (if used)
- [ ] Line of code producing the 1 ms interval
- [ ] Test Plan for Check Off 3


# CHECK OFF 1 — Final Checklist

Before calling the instructor:

- [ ] PA4 / PA5 / PA6 configured as inputs
- [ ] Test LED configured as output
- [ ] Timer0 initialization works
- [ ] Timer0 register values verified in Debugger
- [ ] `delay_1ms(1)` produces 1 ms
- [ ] `delay_1ms(1000)` produces 1000 ms
- [ ] Delay uses polling
- [ ] `_delay_ms()` is NOT used
- [ ] LED flashes ON for 1000 ms
- [ ] LED flashes OFF for 1000 ms
- [ ] `main.c + Timer.c + Timer.h` multimodule structure works
- [ ] Manual Timer0 calculations completed
- [ ] Check Off 3 Test Plan completed
- [ ] Can explain how the 1 ms polling delay works

→ CALL INSTRUCTOR FOR CHECK OFF 1
      
# Lab 6 — Check Off 2

## Goal

Use Timer1 to generate:

- PWM output: OC1A
- Frequency: 245 Hz
- Mode: 9-bit Phase-Correct PWM
- Duty cycle: adjustable from 0%–100%
- Initial duty cycle: 10%


## 1. Complete Manual Work

Fill in the worksheet:

- [ ] Equation for Phase-Correct PWM frequency:

      f_PWM = F_CPU / (2 × N × TOP)
      F_CPU = CPU frequency
      N = prescaler
      TOP = Timer1 maximum counting value

- [ ] F_CPU = __16MHz________

- [ ] PWM mode = 9-bit Phase-Correct PWM

- [ ] TOP = 0x01FF = 511

- [ ] Prescaler = ____64______
      f_PWM = 245, F_CPU = 16,000,000, TOP = 511
      245 = 16000000/2*N*511
      N = 63.92 = 64

- [ ] Datasheet page number = __________
<img width="1760" height="1142" alt="image" src="https://github.com/user-attachments/assets/dafe5ae5-e35c-42a2-aa4a-fca89e6885fb" />

- [ ] Waveform Generation Mode table number = __________
      should be **Mode 2**
      WGM13 = 0, WGM12 = 0, WGM11 = 1, WGM10 = 0
- [ ] Duty-cycle register = OCR1A


## 2. Configure OC1A Pin

- [ ] Find the physical pin corresponding to OC1A
- [ ] Configure OC1A as OUTPUT

Example structure:

    DDRx |= (1 << OC1A_PIN);


## 3. Initialize Timer1

Write a Timer1 initialization function.

The function needs to:

- [ ] Select 9-bit Phase-Correct PWM
- [ ] Configure OC1A as PWM output
- [ ] Select the correct prescaler
- [ ] Start with 10% duty cycle
- [ ] Do NOT change Timer0 configuration

Expected result after initialization:

    OC1A → 245 Hz PWM
    Duty Cycle → 10%


## 4. Write Duty-Cycle Function

Required prototype:

    void Timer1_PWM_245(uint8_t Duty_Cycle);

Input range:

    0 <= Duty_Cycle <= 100

Examples:

    Timer1_PWM_245(10);   // 10%
    Timer1_PWM_245(25);   // 25%
    Timer1_PWM_245(50);   // 50%
    Timer1_PWM_245(75);   // 75%

The function needs to:

1. Receive duty cycle as a percentage.
2. Convert percentage to the corresponding OCR1A value.
3. Store the result in OCR1A.

Conceptually:

    OCR1A = TOP × Duty_Cycle / 100;


## 5. Put Timer1 Code in Timer Module

Project structure:

    main.c
    Timer.c
    Timer.h

Timer.c should contain:

    Timer0 initialization
    delay_1ms()
    Timer1 initialization
    Timer1_PWM_245()

Timer.h should contain the function prototypes.


## 6. Test in main()

Initialize Timer1.

Then test different duty cycles:

    Timer1_PWM_245(10);

Connect OC1A to the oscilloscope.

Verify:

- [ ] Frequency ≈ 245 Hz
- [ ] Duty cycle ≈ 10%

Then try:

    Timer1_PWM_245(25);
    Timer1_PWM_245(50);
    Timer1_PWM_245(75);

Verify that the frequency stays approximately 245 Hz while
the duty cycle changes.


## 7. Be Ready for Instructor's Random Duty Cycle

During check off, instructor may give you a duty cycle.

For example, if instructor says:

    37%

you should only need to call:

    Timer1_PWM_245(37);

Oscilloscope should show approximately:

    Frequency = 245 Hz
    Duty Cycle = 37%


# CHECK OFF 2 — Final Checklist

Before calling the instructor:

- [ ] Manual calculations completed
- [ ] Datasheet page/table identified
- [ ] 9-bit Phase-Correct PWM selected
- [ ] TOP = 511
- [ ] Correct prescaler selected
- [ ] OC1A configured as output
- [ ] Timer1 initialization function works
- [ ] Initial duty cycle = 10%
- [ ] `Timer1_PWM_245(uint8_t Duty_Cycle)` works
- [ ] Duty cycle can be changed from 0–100%
- [ ] Oscilloscope shows approximately 245 Hz
- [ ] Oscilloscope shows requested duty cycle
- [ ] Program is multimodule
- [ ] Can explain how OCR1A determines duty cycle

→ CALL INSTRUCTOR FOR CHECK OFF 2

# Lab 6 — Check Off 3: Speed Profile

## Goal

Create an adjustable PWM speed profile.

The function must allow the instructor to change:

- Starting duty cycle
- Ending duty cycle
- Total ramp time
- Number of steps

Required function:

    void ramp_up_delay_n_steps(
        uint8_t start,
        uint8_t end,
        uint16_t ms_time,
        uint8_t num_steps
    );


## 1. Start / Stop Pushbuttons

My connections:

- Start pushbutton → PA4
- Stop pushbutton → PA5
- Test pushbutton → PA6

Required behavior:

- [ ] Press PA4 → start the speed profile
- [ ] Press PA5 → stop PWM output
- [ ] After stopping/completing, PA4 can start the profile again


## 2. Calculate Duty Cycle Change

Inside `ramp_up_delay_n_steps()`:

Calculate:

    duty_cycle_change = (end - start) / num_steps;

Example:

    start = 15%
    end = 95%
    num_steps = 8

Then:

    duty_cycle_change = (95 - 15) / 8
                      = 10%

So the profile becomes:

    15%
    25%
    35%
    45%
    55%
    65%
    75%
    85%
    95%


## 3. Calculate Time Per Step

Calculate:

    step_time = ms_time / num_steps;

Example:

    ms_time = 5000 ms
    num_steps = 8

Therefore:

    step_time = 5000 / 8
              = 625 ms


## 4. Generate the Speed Profile

The function should:

1. Start PWM at `start`
2. Wait `step_time`
3. Increase duty cycle by `duty_cycle_change`
4. Update OCR1A / PWM duty cycle
5. Wait `step_time`
6. Repeat for `num_steps`
7. Finish at `end`

Use the working PWM function from Check Off 2:

    Timer1_PWM_245(duty_cycle);

Use the working delay function from Check Off 1:

    delay_1ms(step_time);


## 5. Example Profile

Calling:

    ramp_up_delay_n_steps(15, 95, 5000, 8);

should produce:

| Time | Duty Cycle |
|---|---:|
| 0 ms | 15% |
| 625 ms | 25% |
| 1250 ms | 35% |
| 1875 ms | 45% |
| 2500 ms | 55% |
| 3125 ms | 65% |
| 3750 ms | 75% |
| 4375 ms | 85% |
| 5000 ms | 95% |


## 6. Stop PWM

When Stop (PA5) is pressed:

- [ ] Stop PWM output
- [ ] Change OC1A compare-output mode back to normal port operation
- [ ] Do NOT accidentally erase unrelated Timer1 configuration bits

After stopping:

    OC1A → normal I/O pin
    PWM output → OFF


## 7. main() Behavior

Main program should behave approximately like:

    initialize I/O
    initialize Timer0
    initialize Timer1

    while (1)
    {
        if Start button pressed:
            run speed profile

        if Stop button pressed:
            stop PWM
    }

After one profile finishes, pressing Start again must be able to run it again.


## 8. Be Ready for Instructor Values

DO NOT hard-code only:

    15%, 95%, 5000 ms, 8 steps

Instructor will give different values.

For example, instructor should be able to say:

    start = 20
    end = 80
    time = 4000
    steps = 6

and I should only need:

    ramp_up_delay_n_steps(20, 80, 4000, 6);

The same code must still work.


# CHECK OFF 3 — Final Checklist

- [ ] PA4 starts the speed profile
- [ ] PA5 stops PWM
- [ ] `ramp_up_delay_n_steps()` implemented
- [ ] `start` is adjustable
- [ ] `end` is adjustable
- [ ] `ms_time` is adjustable
- [ ] `num_steps` is adjustable
- [ ] Ending duty cycle can be > 70%
- [ ] Number of steps can be < 10
- [ ] Correct duty-cycle change calculated
- [ ] Correct step time calculated
- [ ] OCR1A / PWM changes at each step
- [ ] `delay_1ms()` controls timing
- [ ] Profile can run again after finishing
- [ ] Oscilloscope shows the staircase PWM profile
- [ ] Can explain the code

→ CALL INSTRUCTOR FOR CHECK OFF 3


# Lab 6 — Check Off 4: Troubleshooting

This part cannot be completed ahead of time because the instructor
provides the troubleshooting code.

## What to Do

- [ ] Ask instructor for troubleshooting code
- [ ] Put the provided code into the project
- [ ] Run the code
- [ ] Observe what the hardware actually does
- [ ] Compare actual behavior with expected behavior
- [ ] Find the incorrect section of code
- [ ] Explain why that section causes the problem


# CHECK OFF 4 — Final Checklist

Be ready to tell the instructor:

1. What the hardware is doing
2. Where the problem is in the code
3. Why that code causes the incorrect behavior

After passing:

- [ ] DELETE the instructor's troubleshooting code

→ LAB COMPLETE
