# ECET 279 Labs 4–5: Washing Machine Controller

# ECET 279 实验 4–5：洗衣机控制器

This is a bilingual working checklist for the washing-machine lab. Keep the C
code, function names, variable names, and comments in English; use the Chinese
explanations to understand what each step is doing.

这是一份洗衣机实验的中英文工作清单。C 代码、函数名、变量名和代码注释全部保留英文；
中文部分用于帮助理解每一步的目的。

---

## A. Before You Start / 开始之前

- [ ] Create a folder named `ECET279_Lab4_Wash_Embed_Sys_Dev`.
      创建名为 `ECET279_Lab4_Wash_Embed_Sys_Dev` 的文件夹。
- [ ] Create an ATmega2560 project named `Wash_MC_Sys_Dev`.
      创建名为 `Wash_MC_Sys_Dev` 的 ATmega2560 工程。
- [ ] Copy `Debugger.c` and `Debugger.h` from Lab 3 into the project.
      把 Lab 3 的 `Debugger.c` 和 `Debugger.h` 复制进工程。
- [ ] Use only one file containing `main()` at a time.
      每次编译只能保留一个包含 `main()` 的文件。
- [ ] Label every switch, pushbutton, LED, valve, and motor connection.
      给每个开关、按钮、LED、阀门和电机连接贴上标签。

Why / 原因：The lab is developed in stages. Each stage has its own `main()`;
building two stage files together causes a multiple-definition error.

这个实验分阶段完成，每个阶段都有自己的 `main()`。如果同时编译两个阶段文件，
会出现 multiple definition of `main` 错误。

---

## B. Hardware Assignment / 硬件分配

| Function / 功能 | Pin / 引脚 | Direction / 方向 | Active state / 有效状态 |
|---|---:|---|---|
| Hot selection / 热水选择 | PA0 | Input / 输入 | Low / 低电平 |
| Warm selection / 温水选择 | PA1 | Input / 输入 | Low / 低电平 |
| Cold selection / 冷水选择 | PA2 | Input / 输入 | Low / 低电平 |
| Door open / 门打开 | PA3 | Input / 输入 | Low / 低电平 |
| Start button / 启动按钮 | PA4 | Input / 输入 | Low / 低电平 |
| Stepper IN1–IN4 / 步进电机输入 | PD0–PD3 | Output / 输出 | Pattern / 时序控制 |
| Drain valve / 排水阀 | PC0 | Output / 输出 | High / 高电平 |
| Hot valve / 热水阀 | PC1 | Output / 输出 | High / 高电平 |
| Cold valve / 冷水阀 | PC2 | Output / 输出 | High / 高电平 |
| Done LED / 完成指示灯 | PC3 | Output / 输出 | High / 高电平 |
| Agitate LED / 搅动指示灯 | PC4 | Output / 输出 | High / 高电平 |
| Spin LED / 脱水指示灯 | PC5 | Output / 输出 | High / 高电平 |

The solution assumes active-low switches with internal pull-up resistors.

本方案假设开关为低电平有效，并使用单片机内部上拉电阻：未按下时读取 1，按下或打开时读取 0。

---

## C. Procedure 1: I/O Setup and Testing / 步骤 1：输入输出设置与测试

Use / 使用：`Control_stg_1.c`

- [ ] Define the 16 MHz clock and include the AVR libraries.
      定义 16 MHz 时钟并包含 AVR 库。
- [ ] Configure PA0–PA4 as inputs.
      将 PA0–PA4 设置为输入。
- [ ] Enable pull-up resistors on PA0–PA4.
      打开 PA0–PA4 的内部上拉电阻。
- [ ] Configure PC0–PC5 as valve and LED outputs.
      将 PC0–PC5 设置为阀门和 LED 输出。
- [ ] Configure PD0–PD3 as stepper-motor outputs.
      将 PD0–PD3 设置为步进电机输出。
- [ ] Turn every output off during initialization.
      初始化时关闭全部输出。
- [ ] Run the output test and verify one output at a time.
      运行输出测试，逐个确认每个输出。
- [ ] Activate each input and verify the corresponding `PINA` bit.
      逐个操作输入，并确认对应的 `PINA` 位。

Key idea / 核心概念：A DDR bit of 0 means input; a DDR bit of 1 means output.

DDR 位为 0 表示输入，DDR 位为 1 表示输出。

```c
DDRA &= (uint8_t)~INPUT_MASK;
PORTA |= INPUT_MASK;
DDRC |= CONTROL_OUTPUT_MASK;
PORTC &= (uint8_t)~CONTROL_OUTPUT_MASK;
DDRD |= STEPPER_MASK;
PORTD &= (uint8_t)~STEPPER_MASK;
```

---

## D. Instructor Check-Off 1 / 教师检查 1

- [ ] Show the completed hardware table.
      展示完成的硬件分配表。
- [ ] Show labels on all physical hardware.
      展示所有硬件标签。
- [ ] Show the test plan.
      展示测试计划。
- [ ] Demonstrate all pushbuttons and switches in the debugger.
      在 debugger 中演示所有按钮和开关。
- [ ] Demonstrate that output configuration matches the hardware table.
      演示输出配置与硬件表一致。

---

## E. Procedure 2: Program Skeleton / 步骤 2：程序骨架

Use / 使用：`Control_stg_2_skeleton.c`

### E1. Wait for Start / 等待启动

- [ ] Stay in a loop until Start is pressed.
      保持循环，直到按下 Start。
- [ ] Add a short debounce delay.
      加入短暂的按键消抖延时。
- [ ] Wait until the button is released.
      等待按钮松开，避免同一次按键被重复读取。

### E2. Check the Door / 检查门状态

- [ ] If the door is open, do not operate any valve or motor.
      如果门是打开的，不允许阀门或电机运行。
- [ ] Continue only after the door is closed.
      只有门关闭后才能继续。

### E3. Validate Temperature / 验证温度选择

Only one temperature switch is valid at a time.

每次只能有一个温度开关有效。

| Selection / 选择 | Valid pattern / 有效模式 | Valve result / 阀门结果 |
|---|---|---|
| Hot / 热水 | Hot only / 仅 Hot | Hot ON, Cold OFF |
| Warm / 温水 | Warm only / 仅 Warm | Hot ON, Cold ON |
| Cold / 冷水 | Cold only / 仅 Cold | Hot OFF, Cold ON |
| None or multiple / 无选择或多选 | Invalid / 无效 | Wait; both valves OFF / 等待；两阀关闭 |

- [ ] Use exact equality comparisons for Hot, Warm, and Cold.
      对 Hot、Warm、Cold 使用精确相等比较。
- [ ] Use logical OR `||` between the three valid conditions.
      三个有效条件之间使用逻辑或 `||`。
- [ ] Trap all other combinations in the loop.
      让其他所有组合停留在循环中等待。

### E4. Test with LEDs Before the Motor / 电机接入前先用 LED 测试

- [ ] Fill for 2 seconds.
      进水 2 秒。
- [ ] Turn on the Agitate LED for 8 seconds.
      搅动 LED 亮 8 秒。
- [ ] Drain for 1 second.
      排水 1 秒。
- [ ] Read the temperature selection again and fill for 2 seconds.
      再次读取温度选择并进水 2 秒。
- [ ] Turn on the Agitate LED for 4 seconds.
      搅动 LED 亮 4 秒。
- [ ] Drain for 1 second.
      排水 1 秒。
- [ ] Turn on the Spin LED for 4 seconds.
      脱水 LED 亮 4 秒。
- [ ] Turn on Done and wait for the door to open.
      点亮 Done，并等待门被打开。

Required sequence / 必须的运行顺序：

```text
Start -> Door Closed -> Fill 2 s -> Agitate 8 s -> Drain 1 s
      -> Fill 2 s -> Agitate 4 s -> Drain 1 s -> Spin 4 s
      -> Done -> Wait for Door Open
```

---

## F. Instructor Check-Off 2 / 教师检查 2

- [ ] Demonstrate the complete LED-based sequence.
      演示完整的 LED 模拟运行顺序。
- [ ] Demonstrate Hot, Warm, and Cold separately.
      分别演示 Hot、Warm 和 Cold。
- [ ] Demonstrate that invalid temperature combinations do nothing.
      演示无效温度组合不会启动阀门。
- [ ] Demonstrate that the machine waits for the door to close.
      演示机器会等待门关闭。
- [ ] After completion, demonstrate that opening the door turns Done off.
      完成后演示打开门会关闭 Done 指示灯。
- [ ] Demonstrate that a new cycle cannot start until the door has opened.
      演示门未打开之前不能重新开始下一周期。

---

## G. Procedure 3: Motor Operations / 步骤 3：电机运行

Use / 使用：`stepper.c` and `stepper.h`

### G1. Motor Function Interface / 电机函数接口

```c
void motor_run(char mode, uint8_t seconds);
```

- `'A'` means Agitate / `'A'` 表示搅动。
- `'S'` means Spin / `'S'` 表示脱水。
- `seconds` is the requested operation time / `seconds` 是要求的运行秒数。
- Any invalid mode turns the motor off / 任何无效模式都关闭电机。

### G2. Agitate Mode / 搅动模式

- [ ] Use the eight-pattern half-step sequence.
      使用 8 个模式的 half-step 序列。
- [ ] Use a 4 ms delay between half-steps.
      每个 half-step 之间延时 4 ms。
- [ ] Run 250 half-steps per second: `1000 / 4 = 250`.
      每秒运行 250 个 half-step：`1000 / 4 = 250`。
- [ ] Reverse the pattern direction every 1 second.
      每 1 秒反转一次模式方向。

### G3. Spin Mode / 脱水模式

- [ ] Use the four-pattern full-step sequence.
      使用 4 个模式的 full-step 序列。
- [ ] Run continuously clockwise.
      持续顺时针运行。
- [ ] Use the instructor-approved inter-step delay.
      使用教师确认的步间延时。

Important / 注意：The requirements table says 2 ms for Spin, but Procedure 3
says 4 ms. The provided code uses 2 ms. Ask the instructor which value will be
graded; change only `SPIN_STEP_DELAY_MS` if 4 ms is required.

要求表写 Spin 为 2 ms，但 Procedure 3 写 4 ms。当前代码采用 2 ms。请向老师确认；
如果要求 4 ms，只需要修改 `SPIN_STEP_DELAY_MS`。

### G4. Bit Masking / 位掩码

```c
STEPPER_PORT = (STEPPER_PORT & 0xF0) | (pattern & STEPPER_MASK);
```

This preserves PD4–PD7 and updates only PD0–PD3.

这会保留 PD4–PD7，只更新步进电机使用的 PD0–PD3。

---

## H. Procedure 4: Final Integration / 步骤 4：最终整合

Build only these control files / 最终工程只编译以下控制文件：

- `Control_final.c`
- `stepper.c`
- `stepper.h`
- `Debugger.c` and `Debugger.h`, if required / 如果老师要求 debugger

- [ ] Remove `Control_stg_1.c` and `Control_stg_2_skeleton.c` from the build.
      从编译中移除前两个阶段文件。
- [ ] Replace temporary motor LEDs with actual `motor_run()` calls.
      用真正的 `motor_run()` 调用替换临时电机 LED 模拟。
- [ ] Keep Agitate and Spin indicators active during their motor states.
      电机运行期间保持对应的 Agitate 和 Spin 指示灯点亮。
- [ ] Verify that all valves and motor coils are off before Done turns on.
      确认 Done 点亮前所有阀门和电机线圈都已关闭。

---

## I. Test Plan / 测试计划

Fill in Observed Result and Pass/Fail during the lab.

实验时填写实际结果和 Pass/Fail。

| Test | What to test / 测试内容 | Expected result / 预期结果 | Observed / 实际结果 | Pass/Fail |
|---:|---|---|---|---|
| 1 | Hot fill / 热水进水 | Hot valve ON for 2 s; Cold OFF | | |
| 2 | Warm fill / 温水进水 | Both valves ON for 2 s | | |
| 3 | Cold fill / 冷水进水 | Cold valve ON for 2 s; Hot OFF | | |
| 4 | Invalid temperature / 无效温度选择 | Controller waits; both valves OFF | | |
| 5 | Door interlock / 门安全联锁 | No operation while door is open | | |
| 6 | Full timing / 完整时间顺序 | 2, 8, 1, 2, 4, 1, and 4 seconds | | |
| 7 | Restart interlock / 重启联锁 | No restart until door opens | | |
| 8 | Motor action / 电机动作 | Agitate reverses each second; Spin stays CW | | |

---

## J. Final Instructor Check-Off / 最终教师检查

- [ ] Hardware is completely labeled / 硬件标签完整。
- [ ] All code comments are in English / 所有代码注释为英文。
- [ ] Hot, Warm, and Cold work correctly / 三种温度选择正确。
- [ ] Invalid combinations are trapped / 无效组合被正确拦截。
- [ ] Door interlock works / 门安全联锁正确。
- [ ] Agitate reverses every second / 搅动每秒反向。
- [ ] Spin is clockwise full-step / 脱水为顺时针 full-step。
- [ ] All durations match the lab table / 所有运行时间符合实验表格。
- [ ] Done remains on until the door opens / Done 保持点亮直到门打开。
- [ ] A second cycle requires a new Start press / 下一周期需要重新按 Start。

---

## K. Common Problems / 常见问题

- **Inputs always read zero / 输入一直为 0:** Check wiring and whether the
  board is active-high instead of active-low. 检查接线和开关有效电平。
- **Program waits at temperature selection / 程序卡在温度选择:** Make sure
  exactly one selector is active. 确认只有一个温度开关有效。
- **Warm opens one valve / Warm 只打开一个阀:** Verify PC1 and PC2 wiring and
  the bitwise OR operation. 检查 PC1、PC2 和位或操作。
- **Motor vibrates / 电机振动但不转:** Check IN1–IN4 order and common ground.
  检查 IN1–IN4 顺序以及共地连接。
- **Motor direction is wrong / 电机方向错误:** Reverse the sequence or correct
  the motor-wire order. 反转数组读取顺序或修正电机接线。
- **Multiple `main` error / 多个 main 错误:** Build only one control-stage file.
  每次只编译一个控制阶段文件。

