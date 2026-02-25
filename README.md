# RISC-V Traffic Light SoC

**Tang Nano 9K – picorv32 Based Design**

---

This project implements a minimal 32-bit RISC-V SoC built around the picorv32 core from **YosysHQ**, targeting the **Tang Nano 9K** featuring the **GW1NR-9LV** FPGA.

The design demonstrates:

* Bare-metal RISC-V execution
* Memory-mapped peripherals
* FPGA SRAM pre-initialization
* Hardware/software co-design
* A UART-configurable Traffic Light Controller

This SoC is intentionally simple:

* No AXI/AHB bus
* No bootloader
* No RTOS
* No external memory
* Direct integration in `top.v`

---

# 🧠 Hardware Architecture

The SoC includes:

* picorv32 RV32I CPU
* 8 KB internal SRAM (configurable)
* UART (115200 baud)
* LED driver (read/write)
* 32-bit countdown timer
* Reset controller
* PLL (27 MHz → 5.4 MHz system clock)

### High-Level Block Diagram

```
                 +----------------------+
                 |    Reset Control     |
                 +----------+-----------+
                            |
                            v
+---------------------------------------------------+
|                   picorv32 CPU                    |
+----+-----------+------------+-----------+---------+
     |           |            |           |
     v           v            v           v
  +------+    +------+    +--------+   +--------+
  | SRAM |    | UART |    |  LED   |   | Timer  |
  |      |    |      |    | Driver |   | 32-bit |
  +------+    +------+    +--------+   +--------+
```

All peripherals are directly memory-mapped using the picorv32 simple memory interface.

---

# 🗺 Memory Map

| Address Range           | Peripheral      |
| ----------------------- | --------------- |
| 0x00000000 – 0x00001FFF | 8 KB SRAM       |
| 0x80000000              | LED Register    |
| 0x80000008 – 0x8000000F | UART            |
| 0x80000010              | Countdown Timer |

The CPU performs standard load/store instructions to interact with peripherals.

---

# ⏱ Clocking

* Board clock: 27 MHz
* PLL output: **5.4 MHz system clock**
* UART divisor configured for:

  * 115200 baud
  * 8 data bits
  * No parity
  * No flow control

---

# 💡 Firmware: Traffic Light Controller

The firmware implements a simple finite state machine (FSM):

### States

```
STATE_GREEN
STATE_YELLOW
STATE_RED
```

### Behavior

1. On boot, all LEDs turn ON (visual indicator).
2. UART prompts user for:

   * Green duration (seconds)
   * Yellow duration (seconds)
   * Red duration (seconds)
3. The FSM runs forever:

   * Sets LED output
   * Prints current state
   * Uses countdown timer for delay
   * Transitions to next state

Example UART session:

```
=== Traffic Light Controller ===
Enter delay times in seconds for each light:
Green duration (sec): 5
Yellow duration (sec): 2
Red duration (sec): 5

Starting traffic light FSM...

State: GREEN
State: YELLOW
State: RED
```

---

# 🕒 Countdown Timer

The timer:

* Is 32-bit
* Counts down to zero
* Stops at zero
* Is polled by software (blocking delay)

Tick calculation:

```
TICKS_PER_SEC = 5,400,000
delay_ticks = seconds * TICKS_PER_SEC
```

---

# 🔧 Getting Started

---

## 1️⃣ Requirements

### Hardware

* Tang Nano 9K
* USB cable
* Serial terminal

### FPGA Tools

* Gowin IDE (Educational version tested)

### RISC-V Toolchain

Required tools:

```
riscv64-unknown-elf-gcc
riscv64-unknown-elf-as
riscv64-unknown-elf-objcopy
riscv64-unknown-elf-objdump
```

Windows (MSYS2):

```
pacman -S mingw-w64-x86_64-riscv64-unknown-elf-gcc
```

Linux:
Install via package manager or build from source.

---

# 📁 Project Structure

```
project/
│
├── src/              ← Verilog SoC
│   ├── top.v
│   ├── mem_init*.ini     (auto-generated)
│   ├── sram_addr_width.v (auto-generated)
│   └── force_rebuild.v
│
└── c_code/           ← Firmware
    ├── main.c
    ├── startup.s
    ├── link_cmd.ld
    ├── Makefile
    └── drivers
```

---

# 🧱 Step 1 — Build Firmware

Navigate to firmware directory:

```bash
cd c_code
make
```

This will:

1. Compile startup.s
2. Compile C sources
3. Link into `prog.elf`
4. Generate `prog.bin`
5. Convert binary into SRAM initialization files:

   * `mem_init0.ini`
   * `mem_init1.ini`
   * `mem_init2.ini`
   * `mem_init3.ini`
   * `sram_addr_width.v`
6. Touch `force_rebuild.v` to trigger FPGA rebuild

After this step, the FPGA project is ready to rebuild.

---

# 🧩 Step 2 — Build FPGA Bitstream

1. Open Gowin IDE
2. Open project
3. Run:

   * Synthesis
   * Place & Route
   * Bitstream generation
4. Program Tang Nano 9K

The firmware is now embedded in SRAM.

---

# ▶ Step 3 — Run the Design

1. Open serial terminal
2. Configure:

   * 115200 baud
   * 8N1
   * No flow control
3. Press Reset (S1)

The traffic light controller will start.

---

# ⚙️ Configuring SRAM Size

Inside `c_code/Makefile`:

```
SRAM_ADDR_WIDTH = 11
```

SRAM size formula:

```
Total bytes = 4 * (1 << SRAM_ADDR_WIDTH)
```

Valid values for Tang Nano 9K:

| SRAM_ADDR_WIDTH | SRAM Size | Block RAM Used |
| --------------- | --------- | -------------- |
| 11              | 8 KB      | 4 blocks       |
| 12              | 16 KB     | 8 blocks       |
| 13              | 32 KB     | 16 blocks      |

⚠ Do not use 14 — FPGA has insufficient block RAM.

Changing this value automatically:

* Adjusts stack pointer
* Regenerates SRAM initialization
* Updates Verilog parameters

---

# 🧹 Cleaning Build Files

```
make clean
```

Removes:

* .elf
* .bin
* .hex
* object files

---

# 🎓 Educational Highlights

This project demonstrates:

* Minimal RISC-V softcore integration
* Custom linker script usage
* Direct memory-mapped peripheral design
* FPGA SRAM preloading technique
* Hardware/software co-verification
* Simple ready/valid handshake protocol

---

# 🚀 Possible Extensions

* Interrupt-driven timer
* Pedestrian button input
* Multiple traffic lanes
* Add hardware multiplier
* SPI flash boot
* External memory controller

---

# 📜 License

This project uses the picorv32 core from YosysHQ.
Refer to the original repository for its licensing terms.

