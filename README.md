# STM32_Sensor_Fusion

Bare-metal sensor fusion firmware for the STM32F407VG Discovery board. I2C, SPI, and UART drivers are hand-written directly against the RM0090 reference manual (no HAL), feeding a complementary filter that combines accelerometer and gyroscope data into orientation estimates.

## Features

- Register-level I2C1, SPI1, and USART2 drivers, no STM32Cube HAL dependency
- MPU6050 accelerometer/gyroscope driver with `WHO_AM_I` verification
- Complementary filter for accelerometer/gyroscope sensor fusion
- All pin and peripheral mapping centralized in a single config header
- Renode-based simulation support, with documented, verified limitations

## Hardware & toolchain

|              |                                                                           |
| ------------ | ------------------------------------------------------------------------- |
| Target board | STM32F407VG Discovery                                                     |
| Toolchain    | GNU Arm Embedded (`arm-none-eabi-gcc`)                                    |
| CMSIS        | ARM `CMSIS_5` (`Core/Include`) + ST `STM32CubeF4` (`Device/ST/STM32F4xx`) |
| Simulation   | Renode 1.16.1                                                             |

## Getting started

### Prerequisites

- GNU Arm Embedded toolchain (`arm-none-eabi-gcc`) on your `PATH`
- ARM `CMSIS_5` and ST `STM32CubeF4` cloned or extracted under `CMSIS/`
- [Renode](https://renode.io/) 1.16.1+ for simulation (optional, real hardware not required to build)

### Build

```
make            # build the firmware
make clean      # remove build/
make size       # report firmware size
```

### Flash / run

```
make flash      # program real hardware (untested — no hardware access at time of writing)
make debug      # attach GDB to a running Renode instance
```

## Project structure

```
STM32_Sensor_Fusion/
├── Makefile
├── src/
│   ├── app/
│   │   └── main.c                      # Application entry point, LED bring-up
│   ├── config/
│   │   └── board_config.h              # All pin/peripheral mapping
│   ├── drivers/
│   │   ├── i2c_driver.c / .h           # I2C1 — MPU6050 communication
│   │   ├── spi_driver.c / .h           # SPI1 — flash/EEPROM communication
│   │   └── uart_driver.c / .h          # USART2 — debug output
│   ├── sensors/
│   │   └── accel_mpu6050.c / .h        # MPU6050 register-level driver
│   └── fusion/
│       └── complementary_filter.c / .h # Accelerometer/gyro fusion math
├── CMSIS/                               # ARM + ST headers/sources
├── STM32F407Discovery                  # Reference manual + datasheet
└── renode/
    └── mpu6050_stub.repl                # Non-working I2C slave stub, kept
                                          # for documentation (see Limitations)
```

## Design decisions

**Register-level, no HAL.** Every driver is written directly against RM0090, with a consistent "clear-then-set" discipline on every register write (`&= ~(mask)` before `|= (value)`), even where reset values are already known to be zero.

**`board_config.h` as the single source of pin truth.** No driver hardcodes a GPIO port/pin or alternate function number — everything is a named `#define`, so porting to a different board only touches one file.

**Polling, not interrupt-driven.** All three peripheral drivers use polling loops on status flags rather than interrupts. This was a deliberate scope decision; interrupt-driven I2C is tracked under [Roadmap](#roadmap).

**Bus-appropriate GPIO output type, not copied across peripherals:**

- **I2C** (PB6/PB7) — open-drain, required for a true multi-master/multi-slave shared bus where multiple devices may need to pull a line low without contention.
- **SPI** (PA5–PA7) — push-pull. SPI never has more than one exclusive driver on any given line at a time, so open-drain's collision protection isn't needed.
- **UART** (PA2/PA3) — push-pull, point-to-point, not a shared bus at all.

**SPI chip-select is a plain GPIO, out of band from the SPI peripheral.** SPI has no in-band addressing scheme like I2C; CS (PA4) is configured as a software-controlled GPIO output, idled high immediately in `spi1_init()`, so no slave misinterprets init-time bus activity as an addressed transaction.

**Single full-duplex transfer primitive for SPI.** `spi1_transfer_byte()` always writes and reads simultaneously, matching the hardware's shift-register behavior, rather than exposing separate asymmetric read/write functions the way the I2C driver does.

**Boolean-style return values** (`1` = success) are used for `mpu6050_init()` for call-site readability, rather than the Unix convention of `0` = success.

## Driver reference

### I2C1 (`i2c_driver.c`)

- PB6 (SCL) / PB7 (SDA), AF4, open-drain with pull-up
- `CR2.FREQ = 16`, `CCR = 0x50` (100 kHz standard mode), `TRISE = 17`
- Single-byte and multi-byte read/write, full START/ADDR/TXE/BTF/STOP sequencing
- Reads clear `ACK` and set `STOP` _before_ clearing `ADDR` — required ordering, since the peripheral latches the ACK/NACK decision almost immediately once `ADDR` clears

### SPI1 (`spi_driver.c`)

- PA5 (SCK) / PA6 (MISO) / PA7 (MOSI), AF5, push-pull; PA4 as software chip-select
- `CR1`: `MSTR=1`, `BR=011` (1 MHz), `CPOL=0`/`CPHA=0` (Mode 0), `DFF=0`, `LSBFIRST=0`, `SSM=1`/`SSI=1`, `SPE=1` enabled last
- `spi1_transfer_byte()`: wait `TXE` → write `DR` → wait `RXNE` → read `DR`

### USART2 (`uart_driver.c`)

- PA2 (TX) / PA3 (RX), AF7, push-pull, transmit-only (115200 baud)
- `BRR = (8 << 4) | 11`
- Per-byte writes poll `TXE` only; `uart2_write_string()` polls `TC` once, after the full string

## Limitations

Renode 1.16.1 has no usable slave-device peripheral model for either I2C or SPI, so full sensor/flash transaction validation is not currently possible in simulation. This was verified directly against the installed Renode binaries and platform files, not assumed from documentation.

- **I2C** — no I2C slave model exists at all in this Renode version (confirmed against installed binaries). Firmware boots and correctly reaches the expected I2C polling loop, proving control flow is structurally correct up to the point of needing a real ACK response.
- **SPI** — a generic `SPI.NORFlash` class exists, but no example usage was found anywhere in the install, so it's unconfirmed rather than a dead end.

Full functional validation for both buses is blocked pending real hardware or a custom Renode peripheral model. UART is fully verified in simulation, since it has no slave-device dependency.

## Roadmap

- Validate `complementary_filter.c` against real sensor data
- Interrupt-driven I2C (currently polling-only)
- Attempt `SPI.NORFlash` instantiation in Renode for a JEDEC-ID read sanity check
- Wrap the sensor fusion loop in a FreeRTOS task

## License

MIT — see [LICENSE](LICENSE) for details.
