# Embedded_Systems-accelerometer-uart
Real-time embedded firmware for accelerometer data acquisition, roll/pitch calculation, UART communication, SPI interfacing, and configurable sensor telemetry on the dsPIC33EP512MU810.

# Accelerometer Data Acquisition & UART Processing

<p align="center">
  <strong>Embedded Systems — On-going Assessment</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/MCU-dsPIC33EP512MU810-0066CC?style=for-the-badge" alt="Microcontroller">
  <img src="https://img.shields.io/badge/Control%20Loop-100%20Hz-2E8B57?style=for-the-badge" alt="Control Loop">
  <img src="https://img.shields.io/badge/Accelerometer-50%20Hz-FF8C00?style=for-the-badge" alt="Accelerometer">
  <img src="https://img.shields.io/badge/UART1-RS232-6A5ACD?style=for-the-badge" alt="UART">
  <img src="https://img.shields.io/badge/SPI-IMU-B22222?style=for-the-badge" alt="SPI">
  <img src="https://img.shields.io/badge/Circular%20Buffers-RX%20%2F%20TX-8B008B?style=for-the-badge" alt="Circular Buffers">
</p>

---

## 📌 Project Overview

This repository contains the firmware developed for the **Embedded Systems On-going Assessment**.

The project is implemented for a microcontroller-based embedded system and focuses on real-time scheduling, accelerometer acquisition, UART communication, interrupt handling, circular buffers, command parsing, and computation of roll and pitch angles.

The firmware is designed around a **100 Hz main control loop**, corresponding to a **10 ms control period**.

Inside this real-time framework, the firmware performs a simulated 7 ms algorithm, controls an LED at 1 Hz, acquires the three accelerometer axes at 50 Hz, processes UART commands, dynamically changes the accelerometer filter bandwidth, and transmits accelerometer and orientation data according to configurable frequencies.

The project also demonstrates interrupt-driven UART reception and transmission using separate RX and TX circular buffers.

---

# 🎯 Assignment Objectives

The project requirements are divided into seven main tasks.

### Task 1 — Simulated Real-Time Algorithm

Simulate an algorithm requiring:

- 7 ms execution time
- 100 Hz execution frequency

This emulates a real-world computational task that must execute periodically while respecting its deadline.

### Task 2 — LD2 Blinking

Make LD2 blink at:

- 1 Hz
- 500 ms ON
- 500 ms OFF

### Task 3 — UART Command Processing

Read characters from UART1.

The firmware must recognize:

`$BW,xx*`

where `xx` specifies the accelerometer filter bandwidth.

Valid values:

- 8
- 9
- 10
- 11
- 12
- 13
- 14
- 15

Invalid values must be discarded and the following error message must be transmitted:

`$ERR,1*`

The initial bandwidth is:

**1000 Hz**

corresponding to:

**value 15**

The firmware must also recognize:

`$HZ,yy*`

where `yy` determines the frequency at which accelerometer data is transmitted through UART.

Valid values:

- 0
- 1
- 2
- 5
- 10

A value of `0` disables accelerometer transmission.

Invalid values must be discarded and:

`$ERR,1*`

must be transmitted.

The initial transmission frequency is:

**10 Hz**

### Task 4 — Accelerometer Acquisition

Acquire:

- X-axis
- Y-axis
- Z-axis

at:

**50 Hz**

### Task 5 — Roll and Pitch

Compute:

- Roll angle
- Pitch angle

from the acquired accelerometer data.

### Task 6 — Accelerometer UART Transmission

Transmit the accelerometer data at the configured `yy` frequency using:

`$ACC,x,y,z*`

### Task 7 — Angle UART Transmission

Transmit the calculated angles at:

**5 Hz**

using:

`$ANG,x,y*`

where:

- `x` = roll angle in degrees
- `y` = pitch angle in degrees

---

# 🧠 System Architecture

The firmware is organized around a periodic real-time loop.

The overall data flow is:

PC

→ UART1 RX

→ RX Circular Buffer

→ Command Processing

→ Update Bandwidth / Transmission Frequency

→ Main 100 Hz Loop

→ Accelerometer Acquisition

→ Roll / Pitch Calculation

→ UART TX Circular Buffer

→ UART1 TX

At the same time, the SPI subsystem communicates directly with the accelerometer.

The main components are:

- Timer subsystem
- UART1 subsystem
- SPI subsystem
- IMU subsystem
- Accelerometer acquisition
- Command parser
- RX circular buffer
- TX circular buffer
- Real-time scheduler

---

# ⏱️ Real-Time Scheduling

The main application operates at:

**100 Hz**

Therefore:

**Period = 10 ms**

The assignment requires a simulated algorithm that takes:

**7 ms**

to execute.

This leaves approximately:

**3 ms**

within each 10 ms control period for the remaining application tasks.

This timing constraint is one of the key aspects of the assignment.

The project therefore needs to carefully manage:

- UART communication
- Sensor acquisition
- Data processing
- Transmission
- Interrupt execution
- Shared data

without missing deadlines.

---

# 📊 Timing Requirements

| Task | Frequency | Period |
|---|---:|---:|
| Main control loop | 100 Hz | 10 ms |
| Simulated algorithm | 100 Hz | 10 ms |
| Algorithm execution time | — | 7 ms |
| LD2 state timing | 2 Hz toggle | 500 ms |
| Accelerometer acquisition | 50 Hz | 20 ms |
| Accelerometer UART transmission | Configurable | 0 / 1 / 2 / 5 / 10 Hz |
| Roll/Pitch UART transmission | 5 Hz | 200 ms |

The accelerometer is acquired twice during every 100 Hz main-loop cycle.

---

# 🧮 Task 1 — Simulated Algorithm

The simulated real-time algorithm is implemented through the `algorithm()` function.

The function intentionally waits for:

**7 ms**

to emulate a computational workload.

The main loop executes this algorithm once per 100 Hz cycle.

The timer framework is therefore responsible for ensuring that the simulated algorithm consumes the expected amount of execution time while the main periodic loop remains synchronized.

---

# 💡 Task 2 — LD2

LD2 is controlled through:

**RG9**

The firmware configures RG9 as a digital output.

The LED changes state every:

**500 ms**

Therefore:

- 500 ms ON
- 500 ms OFF

producing:

**1 Hz blinking**

The LED counter is derived from the 100 Hz main loop.

Since:

100 Hz × 0.5 s = 50 cycles

the LED state is toggled every:

**50 control-loop iterations**

This produces the required 1 Hz blinking behavior.

---

# 📡 UART1 Communication

UART1 provides the communication interface between the embedded system and the external host.

The UART pins are mapped using Peripheral Pin Select.

The project uses:

| UART Signal | Pin |
|---|---|
| UART1 TX | RD0 / RP64 |
| UART1 RX | RD11 / RPI75 |

The UART configuration uses:

- 8-bit data
- No parity
- 1 stop bit
- Low-speed baud-rate mode
- Auto-baud disabled

The UART baud-rate configuration is determined by the `U1BRG` setting used in the project.

---

# 🔄 UART Circular Buffers

The firmware uses two independent circular buffers:

- RX circular buffer
- TX circular buffer

The RX buffer stores incoming characters.

The TX buffer stores outgoing characters.

This architecture allows UART communication to continue asynchronously without requiring the main application to manually wait for every character.

The current implementation uses:

**RX buffer size: 64 bytes**

**TX buffer size: 256 bytes**

The TX buffer is larger because the application can generate multiple telemetry messages while the UART is still transmitting previous data.

---

# ⚡ UART Interrupt Architecture

UART reception and transmission are handled using interrupts.

The RX interrupt:

1. Detects received UART data.
2. Reads available characters from `U1RXREG`.
3. Stores the characters in the RX circular buffer.
4. Updates the RX buffer indexes.
5. Clears the UART overrun condition if necessary.

The TX interrupt:

1. Detects when UART transmission can continue.
2. Retrieves data from the TX circular buffer.
3. Writes the next character to `U1TXREG`.
4. Updates the TX buffer indexes.
5. Disables the TX interrupt when the buffer becomes empty.

This approach prevents UART communication from unnecessarily blocking the main control loop.

---

# 📥 UART RX Data Flow

Incoming UART data follows this sequence:

UART1 RX

↓

RX Interrupt

↓

Read Character

↓

RX Circular Buffer

↓

Main Application

↓

Command Parser

↓

Command Execution

The interrupt therefore handles the low-level byte reception while the main application performs the higher-level command processing.

---

# 📤 UART TX Data Flow

Outgoing data follows:

Application

↓

TX Circular Buffer

↓

TX Interrupt

↓

UART1 TX Register

↓

External Host

The application can therefore enqueue a complete message without waiting for every character to physically leave the UART peripheral.

---

# 🧩 Circular Buffer Design

Both buffers use head and tail indexes.

The next index is calculated using wrap-around logic.

When the index reaches the end of the buffer, it returns to zero.

This creates the circular structure required for continuous UART operation.

The firmware prevents the write index from overtaking the read index.

If the RX buffer is full, incoming characters are discarded rather than overwriting unread data.

If the TX buffer is full, additional outgoing characters are not inserted.

This prevents memory corruption and preserves the integrity of the circular-buffer data structure.

---

# 📥 Task 3 — Bandwidth Command

The accelerometer filter bandwidth is controlled through:

`$BW,xx*`

The valid values are:

| Value | Status |
|---:|---|
| 8 | Valid |
| 9 | Valid |
| 10 | Valid |
| 11 | Valid |
| 12 | Valid |
| 13 | Valid |
| 14 | Valid |
| 15 | Valid |

Any value outside this range is invalid.

Examples of invalid values include:

- -1
- 3.2
- 18

Invalid commands are discarded and:

`$ERR,1*`

is transmitted.

---

# 🎚️ Initial Accelerometer Bandwidth

The initial accelerometer bandwidth is:

**1000 Hz**

This corresponds to:

**Register value 15**

The firmware initializes:

`acc_bandwidth = 15`

When a valid `$BW,xx*` command is received, the requested value is stored.

The main application detects a change in the bandwidth setting and writes the new value to the accelerometer configuration register.

The project uses:

**Register 0x10**

for the accelerometer bandwidth configuration.

---

# 📡 Bandwidth Update Flow

The complete process is:

PC

↓

`$BW,xx*`

↓

UART RX Interrupt

↓

RX Circular Buffer

↓

Command Parser

↓

Validate `xx`

↓

Valid?

YES → Update `acc_bandwidth`

NO → Send `$ERR,1*`

↓

Main Loop Detects Change

↓

Write New Bandwidth to Accelerometer

---

# 📥 Task 3 — Frequency Command

The accelerometer transmission frequency is controlled through:

`$HZ,yy*`

Valid values are:

| Value | Meaning |
|---:|---|
| 0 | Disable accelerometer transmission |
| 1 | 1 Hz |
| 2 | 2 Hz |
| 5 | 5 Hz |
| 10 | 10 Hz |

The initial frequency is:

**10 Hz**

Therefore the firmware starts with:

`uart_frequency = 10`

---

# ❌ Invalid UART Commands

The command parser validates the received numerical value.

The parser accepts only unsigned integer values.

It rejects:

- Empty values
- Negative values
- Decimal values
- Alphabetic characters
- Values outside the allowed range

For example:

`$BW,-1*`

is invalid.

`$BW,3.2*`

is invalid.

`$BW,18*`

is invalid.

Similarly:

`$HZ,3*`

is invalid.

When an invalid command is detected, the firmware sends:

`$ERR,1*`

---

# 🧮 Numeric Parsing

The firmware uses a dedicated numeric parser.

The parser:

1. Checks that the string is not empty.
2. Checks every character.
3. Accepts only characters from `0` to `9`.
4. Builds the integer value.
5. Rejects values that exceed the supported range.
6. Returns the parsed value when valid.

This prevents unintended interpretation of malformed commands.

---

# 🧭 Command Parser

The parser recognizes two main commands.

## Bandwidth Command

`$BW,xx*`

The parser checks:

- Command prefix
- Comma separator
- Numeric value
- Valid range

If valid:

`acc_bandwidth = xx`

Otherwise:

`$ERR,1*`

---

## Frequency Command

`$HZ,yy*`

The parser checks:

- Command prefix
- Comma separator
- Numeric value
- Allowed frequency set

Valid values:

0, 1, 2, 5, 10

If valid:

`uart_frequency = yy`

Otherwise:

`$ERR,1*`

---

# 📈 Task 4 — Accelerometer Acquisition

The three accelerometer axes are acquired at:

**50 Hz**

The main control loop operates at:

**100 Hz**

Therefore, the accelerometer is acquired once every:

**2 main-loop iterations**

The implementation uses an acquisition counter.

When the counter reaches the required number of control-loop iterations, the firmware executes:

`accel_read_xyz()`

The function returns:

- X
- Y
- Z

accelerometer measurements.

---

# 🧭 Accelerometer Data Structure

The accelerometer data is stored in:

`AxesRaw_t`

The structure contains:

- `x`
- `y`
- `z`

This structure is shared between the accelerometer acquisition and angle-calculation / transmission tasks.

---

# 🔌 SPI Communication

The accelerometer is accessed through SPI1.

The SPI peripheral is configured in master mode.

The project configures:

- Master mode
- 8-bit SPI transfers
- Clock polarity
- Clock phase
- Input sampling
- SPI enable
- Peripheral Pin Select

The accelerometer chip-select signal is controlled separately.

The accelerometer chip select is connected to:

**RB3**

---

# 📐 Accelerometer Data Acquisition

The accelerometer read sequence begins by selecting the accelerometer using its chip-select signal.

The firmware sends the accelerometer register address with the read bit enabled.

The starting register used for accelerometer data acquisition is:

**0x02**

The firmware then reads:

- X-axis
- Y-axis
- Z-axis

Each axis is reconstructed from its corresponding bytes.

The resulting values are returned using the `AxesRaw_t` structure.

---

# 🧮 Task 5 — Roll and Pitch Calculation

The accelerometer measurements are converted to floating-point values before calculating the orientation angles.

The firmware calculates:

- Roll
- Pitch

using the accelerometer axes.

The current implementation uses the following equations:

Roll:

`atan2(-ay, ax)`

Pitch:

`atan2(sqrt(ax² + ay²), az)`

The resulting values are converted from radians to degrees using:

`180 / π`

---

# 📐 Roll Angle

The roll angle is calculated from:

`atan2(-ay, ax)`

The result is converted to degrees.

The resulting value represents the roll orientation derived from the accelerometer measurements.

---

# 📐 Pitch Angle

The pitch angle is calculated from:

`atan2(sqrt(ax² + ay²), az)`

The result is converted from radians to degrees.

This provides the pitch orientation based on the accelerometer vector.

---

# 📤 Task 6 — Accelerometer UART Transmission

The accelerometer data is transmitted using:

`$ACC,x,y,z*`

where:

- `x` = accelerometer X-axis
- `y` = accelerometer Y-axis
- `z` = accelerometer Z-axis

The transmission frequency is controlled dynamically through the `$HZ,yy*` command.

---

# ⏱️ Dynamic ACC Transmission

If:

`uart_frequency > 0`

the firmware calculates the required number of 100 Hz control-loop cycles between transmissions.

The relationship is:

`ticks = 100 / uart_frequency`

Therefore:

| UART Frequency | Control-Loop Cycles |
|---:|---:|
| 1 Hz | 100 |
| 2 Hz | 50 |
| 5 Hz | 20 |
| 10 Hz | 10 |

If:

`uart_frequency = 0`

accelerometer transmission is disabled.

---

# 📤 ACC Message Example

For example, if the accelerometer values are:

X = 120

Y = -35

Z = 980

the transmitted message is:

`$ACC,120,-35,980*`

The message is placed into the TX circular buffer and transmitted through UART1.

---

# 📤 Task 7 — Angle Transmission

Roll and pitch are transmitted at:

**5 Hz**

The required message format is:

`$ANG,x,y*`

where:

- `x` = roll angle in degrees
- `y` = pitch angle in degrees

Because the main control loop operates at 100 Hz:

100 / 5 = 20

Therefore the angle message is transmitted every:

**20 control-loop iterations**

corresponding to:

**200 ms**

---

# 📤 ANG Message Example

If:

Roll = 12.50°

Pitch = -4.75°

the transmitted message is:

`$ANG,12.50,-4.75*`

The firmware formats the angles with two decimal places.

---

# 📊 UART Protocol Summary

| Direction | Message | Frequency / Trigger |
|---|---|---|
| PC → MCU | `$BW,xx*` | On command |
| PC → MCU | `$HZ,yy*` | On command |
| MCU → PC | `$ERR,1*` | Invalid command |
| MCU → PC | `$ACC,x,y,z*` | 0 / 1 / 2 / 5 / 10 Hz |
| MCU → PC | `$ANG,x,y*` | 5 Hz |

The protocol uses:

`$` as the start character.

`*` as the end character.

---

# 🏗️ Main Program Architecture

The main application performs the following operations every control cycle:

1. Execute the simulated 7 ms algorithm.
2. Process received UART commands.
3. Check for accelerometer bandwidth changes.
4. Update LD2 timing.
5. Acquire accelerometer data at 50 Hz.
6. Transmit accelerometer data at the configured frequency.
7. Calculate roll and pitch at the required timing.
8. Transmit roll and pitch at 5 Hz.
9. Wait for the next 100 Hz timer period.

This creates a deterministic periodic structure around Timer1.

---

# ⏲️ Timer Architecture

The project uses:

**Timer1**

for the 100 Hz main control loop.

The period is configured to:

**10 ms**

Timer1 therefore provides the main scheduling reference.

The project also uses:

**Timer2**

for the simulated 7 ms algorithm.

Timer2 provides the delay required to emulate the execution time of the real-world algorithm.

---

# 🔁 Main Runtime Sequence

The runtime behavior can be summarized as:

Timer1 Period = 10 ms

↓

Start Main Loop

↓

Execute 7 ms Algorithm

↓

Process UART Commands

↓

Update Accelerometer Bandwidth

↓

Update LD2

↓

Read Accelerometer at 50 Hz

↓

Transmit `$ACC` at Configured Frequency

↓

Calculate Roll / Pitch

↓

Transmit `$ANG` at 5 Hz

↓

Wait for Timer1 Period

↓

Repeat

---

# ⚠️ Real-Time Constraints

The assignment emphasizes that only approximately:

**3 ms**

remain available after the 7 ms algorithm within each 10 ms period.

Therefore, the firmware must avoid unnecessary blocking operations.

The most important considerations are:

- UART handling
- Interrupt execution time
- Circular-buffer management
- Shared variables
- Sensor acquisition
- Data transmission
- Timer deadlines

---

# ⚡ Interrupt Design

The project uses interrupts where they are useful for asynchronous hardware interaction.

UART RX and TX are handled through interrupts because UART communication can occur independently from the main 100 Hz scheduler.

The UART RX interrupt stores received bytes in the RX circular buffer.

The UART TX interrupt transmits bytes from the TX circular buffer.

The main program then performs command parsing outside the interrupt context.

This keeps message-level processing separate from the low-level interrupt routines.

---

# 🔐 Shared Data

Several variables are shared between the UART subsystem and the main application.

Important shared variables include:

- `acc_bandwidth`
- `uart_frequency`
- RX buffer indexes
- TX buffer indexes

The firmware therefore needs to consider the possibility that an interrupt may occur while the main program is accessing shared data.

Correct shared-data handling is important because inconsistent updates can produce incorrect system behavior.

---

# 🧠 Why Circular Buffers Are Used

The assignment specifically suggests the use of separate circular buffers for UART reception and transmission.

The reason is that UART communication is asynchronous and may occur while the main application is executing the 7 ms simulated algorithm.

Without buffering, the application could lose characters or become blocked waiting for UART hardware.

The circular-buffer architecture provides temporary storage between:

- UART hardware
- Interrupt routines
- Main application

This is particularly important because the application has only a limited amount of free execution time in each 10 ms period.

---

# 🚫 Busy-Waiting Considerations

The assignment warns about unnecessary busy-waiting.

Busy-waiting can prevent the processor from performing other required tasks and can cause missed deadlines.

The firmware therefore uses interrupts for UART communication.

The SPI communication is comparatively short, and the assignment notes that SPI interactions can be considered negligible given the available SPI clock rate.

The main timing concern remains the 7 ms algorithm and UART handling.

---

# 📏 SPI Timing Consideration

The accelerometer communicates through SPI.

The assignment states that the IMU allows an SPI clock up to:

**7.5 MHz**

and that two bytes require approximately:

**2 microseconds**

at that clock speed.

Therefore, the SPI communication required for sensor acquisition is considered small compared with the 10 ms control-loop period.

---

# 🧪 Testing Strategy

The firmware should be tested progressively.

## Test 1 — Main Loop Frequency

Verify that Timer1 produces:

**100 Hz**

or:

**10 ms period**

The main loop should maintain its periodic timing without missed deadlines.

---

## Test 2 — Simulated Algorithm

Verify that the simulated algorithm consumes approximately:

**7 ms**

per execution.

The algorithm should execute once per 100 Hz cycle.

---

## Test 3 — LD2

Verify:

- 500 ms ON
- 500 ms OFF
- 1 Hz complete blink cycle

The LED should toggle every 50 main-loop cycles.

---

## Test 4 — Accelerometer Acquisition

Verify that the three axes are acquired at:

**50 Hz**

The acquisition should therefore occur every:

**20 ms**

or every two 100 Hz control cycles.

---

## Test 5 — Bandwidth Command

Send:

`$BW,8*`

Expected:

Bandwidth changes to 8.

Send:

`$BW,15*`

Expected:

Bandwidth changes to 15.

Send:

`$BW,18*`

Expected:

Command is rejected.

Expected response:

`$ERR,1*`

---

## Test 6 — Invalid Bandwidth

Test:

`$BW,-1*`

Expected:

`$ERR,1*`

Test:

`$BW,3.2*`

Expected:

`$ERR,1*`

Test:

`$BW,18*`

Expected:

`$ERR,1*`

---

## Test 7 — Frequency Command

Send:

`$HZ,10*`

Expected:

ACC messages are transmitted at 10 Hz.

Send:

`$HZ,5*`

Expected:

ACC messages are transmitted at 5 Hz.

Send:

`$HZ,2*`

Expected:

ACC messages are transmitted at 2 Hz.

Send:

`$HZ,1*`

Expected:

ACC messages are transmitted at 1 Hz.

Send:

`$HZ,0*`

Expected:

ACC transmission is disabled.

---

## Test 8 — Invalid Frequency

Send:

`$HZ,3*`

Expected:

`$ERR,1*`

Send:

`$HZ,7*`

Expected:

`$ERR,1*`

Send:

`$HZ,11*`

Expected:

`$ERR,1*`

---

## Test 9 — ACC Message

Verify that the accelerometer message follows:

`$ACC,x,y,z*`

The message frequency must correspond to the currently selected `$HZ` value.

---

## Test 10 — ANG Message

Verify that:

`$ANG,x,y*`

is transmitted at:

**5 Hz**

The values must correspond to:

- Roll
- Pitch

in degrees.

---

# 📋 Requirements Checklist

| Requirement | Implementation |
|---|---|
| 7 ms simulated algorithm | Implemented |
| 100 Hz main loop | Implemented |
| LD2 at 1 Hz | Implemented |
| 500 ms ON / 500 ms OFF | Implemented |
| UART1 reception | Implemented |
| `$BW,xx*` command | Implemented |
| Valid bandwidth 8–15 | Implemented |
| Invalid bandwidth rejection | Implemented |
| `$ERR,1*` error message | Implemented |
| Initial bandwidth = 15 | Implemented |
| `$HZ,yy*` command | Implemented |
| Valid frequencies 0,1,2,5,10 | Implemented |
| Invalid frequency rejection | Implemented |
| Initial frequency = 10 Hz | Implemented |
| 50 Hz accelerometer acquisition | Implemented |
| X/Y/Z acquisition | Implemented |
| Roll calculation | Implemented |
| Pitch calculation | Implemented |
| `$ACC,x,y,z*` message | Implemented |
| Configurable ACC frequency | Implemented |
| `$ANG,x,y*` message | Implemented |
| 5 Hz angle transmission | Implemented |
| RX circular buffer | Implemented |
| TX circular buffer | Implemented |
| UART RX interrupt | Implemented |
| UART TX interrupt | Implemented |
| SPI accelerometer communication | Implemented |

---

# 📁 Project Structure

The project is organized as an MPLAB X embedded project.

The main source files are:

`main.c`

Contains:

- Main application
- 100 Hz scheduler
- Task coordination
- Accelerometer acquisition scheduling
- LED scheduling
- ACC transmission scheduling
- Angle calculation
- Angle transmission

`timer.c`

Contains:

- Timer configuration
- Timer period handling
- Timer waiting functions

`timer.h`

Contains timer-related declarations and definitions.

`uart.c`

Contains:

- UART1 initialization
- UART RX interrupt
- UART TX interrupt
- RX circular buffer
- TX circular buffer
- Command parser
- Numeric validation
- Error-message generation
- UART transmission functions

`uart.h`

Contains the public UART interface.

`spi.c`

Contains:

- SPI initialization
- SPI data transmission
- SPI receive operation

`spi.h`

Contains SPI declarations.

`imu.c`

Contains:

- IMU initialization
- Accelerometer chip-select configuration
- Accelerometer register writes
- Accelerometer X/Y/Z acquisition

`imu.h`

Contains:

- Accelerometer data structure
- IMU function declarations

---

# 📂 Repository Structure

A simplified repository structure is:

Embedded_Systems_Group_3.X/

├── main.c

├── imu.c
├── imu.h

├── spi.c
├── spi.h

├── timer.c
├── timer.h

├── uart.c
├── uart.h

├── Makefile

├── nbproject/

└── README.md

The repository may also contain generated MPLAB X build and configuration files.

---

# 🛠️ Development Environment

The project is developed as an MPLAB X embedded project.

Target microcontroller:

**dsPIC33EP512MU810**

Main technologies used:

- C
- XC16
- MPLAB X
- UART1
- SPI1
- Timer1
- Timer2
- Accelerometer
- Interrupt Service Routines
- Circular Buffers

---

# 🔌 Hardware Interfaces

## UART1

Used for:

- Receiving `$BW` commands
- Receiving `$HZ` commands
- Sending `$ERR`
- Sending accelerometer data
- Sending roll/pitch data

TX:

**RD0 / RP64**

RX:

**RD11 / RPI75**

---

## SPI1

Used for:

- Accelerometer communication

The accelerometer is accessed through SPI and a dedicated chip-select line.

---

## Timer1

Used for:

**100 Hz main scheduling**

Period:

**10 ms**

---

## Timer2

Used for:

**7 ms simulated algorithm**

---

## LD2

Connected to:

**RG9**

Used for:

**1 Hz status indication**

---

# 📡 Communication Examples

## Change Bandwidth

PC sends:

`$BW,12*`

Firmware accepts the command and changes the accelerometer bandwidth configuration to:

**12**

---

## Change ACC Frequency

PC sends:

`$HZ,5*`

Firmware changes the accelerometer transmission rate to:

**5 Hz**

---

## Disable ACC Transmission

PC sends:

`$HZ,0*`

Firmware stops sending `$ACC` messages.

Accelerometer acquisition continues at 50 Hz.

---

## Invalid Command

PC sends:

`$HZ,7*`

Firmware responds:

`$ERR,1*`

---

# 📈 Example Data Flow

A typical execution cycle can be summarized as:

100 Hz Timer

↓

Execute 7 ms Algorithm

↓

Process UART RX Buffer

↓

Check `$BW` / `$HZ`

↓

Update Accelerometer Configuration

↓

Update LD2

↓

Every 20 ms:

Acquire X/Y/Z

↓

Calculate Roll/Pitch when required

↓

Transmit `$ACC` according to configured frequency

↓

Transmit `$ANG` every 200 ms

↓

Wait for next Timer1 period

↓

Repeat

---

# 🧩 Software Design Principles

The firmware follows several important embedded-system design principles.

## Separation of Responsibilities

The UART driver is responsible for:

- UART configuration
- Byte reception
- Byte transmission
- Circular buffers
- Command parsing

The IMU driver is responsible for:

- Accelerometer configuration
- SPI communication
- Accelerometer acquisition

The timer module is responsible for:

- Timer configuration
- Period management
- Timing delays

The main application is responsible for:

- Scheduling
- Task coordination
- Sensor processing
- Telemetry scheduling

---

# 🔐 Data Integrity

Because UART interrupts can occur at any point during main-program execution, shared variables and circular-buffer indexes must be handled carefully.

The important shared structures include:

- RX buffer
- TX buffer
- RX head
- RX tail
- TX head
- TX tail
- `acc_bandwidth`
- `uart_frequency`

Incorrect handling could result in:

- Lost characters
- Corrupted commands
- Incorrect buffer indexes
- Incorrect configuration
- Transmission errors

Therefore, interrupt timing and shared-data access are important parts of the design.

---

# ⚠️ Compilation Warnings

Compilation warnings should not be ignored.

In embedded applications, a warning can indicate a real functional problem such as:

- Integer truncation
- Overflow
- Incorrect type conversion
- Incorrect signed/unsigned operation
- Invalid pointer usage
- Incorrect peripheral register assignment

The assignment explicitly emphasizes solving compiler warnings rather than ignoring them.

---

# 🧪 Evaluation Criteria

The project is evaluated according to several important embedded-system criteria.

## Timing

Are tasks executed at the correct frequency?

Are deadlines missed because of incorrect peripheral handling?

Is busy-waiting used appropriately?

## Shared Data

Is shared data handled correctly?

Could an interrupt occur at an unsafe point and cause inconsistent data?

## Circular Buffers

Are the buffers correctly sized?

Has the data-production and data-consumption rate been considered?

Is memory being used efficiently?

## Interrupts

Are interrupt routines short enough?

Is unnecessary processing performed inside interrupts?

Is unnecessary busy-waiting avoided?

## Code Quality

Is the code:

- Well written?
- Clearly formatted?
- Properly commented?
- Maintainable?
- Modular?

These aspects are particularly important in embedded applications.

---

# 🧠 Embedded Systems Concepts Demonstrated

This project demonstrates several fundamental embedded-systems concepts.

### Real-Time Scheduling

A periodic 100 Hz control loop is used to coordinate multiple tasks with different frequencies.

### Timer-Based Scheduling

Timer1 provides the 10 ms application period.

Timer2 provides the simulated 7 ms execution time.

### Interrupt-Driven Communication

UART RX and TX use interrupts to handle asynchronous communication.

### Circular Buffers

Separate RX and TX buffers decouple UART hardware from the application.

### SPI Communication

The accelerometer is accessed through SPI.

### Sensor Acquisition

The three accelerometer axes are acquired at 50 Hz.

### Orientation Estimation

Roll and pitch are calculated from accelerometer measurements.

### Command Parsing

The firmware interprets structured ASCII commands from the PC.

### Runtime Configuration

The accelerometer bandwidth and UART transmission frequency can be changed without recompiling the firmware.

### Error Handling

Invalid commands generate the standardized:

`$ERR,1*`

response.

---

# 📊 Complete System Summary

The project combines:

**100 Hz Real-Time Scheduling**

+

**7 ms Simulated Algorithm**

+

**1 Hz LED Indication**

+

**50 Hz Accelerometer Acquisition**

+

**Roll / Pitch Calculation**

+

**Configurable UART Telemetry**

+

**UART RX/TX Interrupts**

+

**RX/TX Circular Buffers**

+

**SPI Accelerometer Communication**

+

**Runtime Filter Bandwidth Configuration**

+

**Runtime Transmission Frequency Configuration**

into one embedded-system application.

---

# 🚀 Final Workflow

The final system behavior is:

System Reset

↓

Initialize GPIO

↓

Initialize SPI

↓

Initialize IMU

↓

Initialize UART1

↓

Enable Interrupts

↓

Configure Timer1 for 100 Hz

↓

Set Initial Bandwidth = 15

↓

Set Initial UART Frequency = 10 Hz

↓

Start Main Loop

↓

Execute 7 ms Algorithm

↓

Process UART Commands

↓

Update Bandwidth if Required

↓

Toggle LD2 According to 1 Hz Schedule

↓

Acquire Accelerometer at 50 Hz

↓

Transmit `$ACC` According to `$HZ`

↓

Calculate Roll and Pitch

↓

Transmit `$ANG` at 5 Hz

↓

Wait for Next 100 Hz Period

↓

Repeat

---

# 🏁 Conclusion

This project implements a real-time embedded application focused on accelerometer data acquisition, orientation calculation, UART communication, and periodic task scheduling.

The system is structured around a 100 Hz main control loop and must execute a simulated 7 ms workload during every control period.

The accelerometer is acquired at 50 Hz, while roll and pitch are calculated from the measured X, Y, and Z axes.

The UART interface provides a configurable command system through which the user can change the accelerometer filter bandwidth and the accelerometer-data transmission frequency at runtime.

The firmware supports:

- `$BW,xx*`
- `$HZ,yy*`
- `$ACC,x,y,z*`
- `$ANG,x,y*`
- `$ERR,1*`

UART communication is implemented using interrupt-driven RX and TX handling together with separate circular buffers.

The project therefore demonstrates the integration of:

**Real-Time Scheduling**

**Timers**

**SPI Communication**

**Accelerometer Acquisition**

**Orientation Calculation**

**UART Communication**

**Interrupts**

**Circular Buffers**

**Command Parsing**

**Runtime Configuration**

**Error Handling**

into a single embedded firmware application.

---

## 👤 Project Information

**Course:** Embedded Systems

**Project:** On-going Assessment — Accelerometer

**Microcontroller:** dsPIC33EP512MU810

**Main Loop:** 100 Hz

**Simulated Algorithm:** 7 ms

**Accelerometer Acquisition:** 50 Hz

**Angle Transmission:** 5 Hz

**UART:** UART1

**Sensor Interface:** SPI

**Communication:** UART / RS232

---

<p align="center">
  <strong>Embedded Systems • Real-Time Programming • Sensor Acquisition • UART Communication</strong>
</p>
