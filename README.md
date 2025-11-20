# DECUP

[![build](https://github.com/ZIMO-Elektronik/DECUP/actions/workflows/build.yml/badge.svg)](https://github.com/ZIMO-Elektronik/DECUP/actions/workflows/build.yml) [![tests](https://github.com/ZIMO-Elektronik/DECUP/actions/workflows/tests.yml/badge.svg)](https://github.com/ZIMO-Elektronik/DECUP/actions/workflows/tests.yml) [![release](https://github.com/ZIMO-Elektronik/DECUP/actions/workflows/release.yml/badge.svg)](https://github.com/ZIMO-Elektronik/DECUP/actions/workflows/release.yml) [![license](https://img.shields.io/github/license/ZIMO-Elektronik/DECUP)](https://github.com/ZIMO-Elektronik/DECUP/raw/master/LICENSE)

> [!IMPORTANT]  
> Not recommended for new designs.

<img src="https://github.com/ZIMO-Elektronik/DECUP/raw/master/data/images/MXDECUP-VL.gif" width="20%" align="right">

DECUP is an acronym for **Dec**oder **Up**date, a protocol for [ZPP](https://github.com/ZIMO-Elektronik/ZPP) and [ZSU](https://github.com/ZIMO-Elektronik/ZSU) updates over the track. The protocol is currently supported by the following products:
- Command stations
  - [ZIMO MXULF](https://www.zimo.at/web2010/products/InfMXULF_EN.htm)
  - [ZIMO MXDECUP](https://www.zimo.at/web2010/documents/MXDECUPE.pdf)
- Decoders
  - [ZIMO small-](http://www.zimo.at/web2010/products/lokdecoder_EN.htm) and [large-scale MX decoders](http://www.zimo.at/web2010/products/lokdecodergrosse_EN.htm)

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#protocol">Protocol</a></li>
      <ul>
        <li><a href="#transmission">Transmission</a></li>
        <li><a href="#feedback">Feedback</a></li>
        <li><a href="#zsu">ZSU</a></li>
        <li><a href="#zpp">ZPP</a></li>
      </ul>
    <li><a href="#getting-started">Getting Started</a></li>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
        <li><a href="#build">Build</a></li>
      </ul>
    <li><a href="#usage">Usage</a></li>
  </ol>
</details>

## Protocol
### Transmission
The protocol was developed for the MXDECUP, an update device without a microprocessor. The device placed a UART signal (at that time ±12V) more or less directly on the tracks. The connection itself uses **LSB first 38400 baud 8N2**, i.e. 8 data bits, no parity and 2 stop bits.

#### RTS
A special feature is that **the RTS line is used to switch the track voltage on and off**.
> [!WARNING]  
> Using RTS is not optional. ZSP already sends packets for calibration while RTS is off. The data received during this time must under no circumstances be transmitted on the track.

### Feedback
In principle, the receiver (decoder) has the option of responding with one or two short-circuit pulses after a transmission in the time window provided for this purpose. These pulses are **<8µs long** and, if two pulses are made, come at an **interval of 150-160µs**. The duration of the **entire time window can unfortunately vary**, which is why in the following it is explicitly stated for each transmission how long, if at all, to wait for a response.

### ZSU
In its original version, the protocol was intended exclusively for [ZSU](https://github.com/ZIMO-Elektronik/ZSU) updates. For this reason, the decoder update procedure does not have any meaningfully segmented packets, but simply sends a bunch of raw data whose meaning is determined solely by the sequence.

#### Entry
The following two bytes represent the entry sequence for a ZSU firmware update. The two bytes must each be sent **at least 20 times**, with a pause of at least **300µs** between each byte.

| Length | Value | Description         |
| ------ | ----- | ------------------- |
| 1 byte | 0xBF  | First preamble byte |

| Length | Value | Description          |
| ------ | ----- | -------------------- |
| 1 byte | 0xEF  | Second preamble byte |

#### Decoder ID
After entry, the decoder IDs (e.g. 221 for MX645) are sent one by one to determine which decoder is connected. A connected decoder responds with a double pulse.

| Length | Value | Description                                |
| ------ | ----- | ------------------------------------------ |
| 1 byte | >0x80 | Single byte decoder ID (found in ZSU file) |
| 1 ms   | \|\|  | Double pulse on success                    |

#### Block count
Next, the block count is transmitted.

| Length | Value | Description             |
| ------ | ----- | ----------------------- |
| 1 byte |       | Block count             |
| 1 ms   | \|    | Single pulse on success |

The block count is directly proportional to the number of update blocks actually transmitted and is calculated as follows.

```cpp
uint8_t block_count = (firmware_size + bootloader_size) / 256u - 1u; 
```

#### Security bytes
The block count followed by the transmission of two security bytes, first 0x55, then 0xAA.

| Length | Value | Description             |
| ------ | ----- | ----------------------- |
| 1 byte | 0x55  | First security byte     |
| 5 ms   | \|    | Single pulse on success |

| Length | Value | Description             |
| ------ | ----- | ----------------------- |
| 1 byte | 0xAA  | Second security byte    |
| 5 ms   | \|    | Single pulse on success |

#### Blocks
Then the transfer of the actual firmware blocks can finally begin.

| Length        | Value      | Description                                        |
| ------------- | ---------- | -------------------------------------------------- |
| 1 byte        |            | Current block count                                |
| 32 or 64 byte |            | Firmware data                                      |
| 1 byte        |            | XOR checksum                                       |
| 100 ms        | \| or \|\| | Single pulse on failure<br>Double pulse on success |

For feedback, a double pulse counts as ack and a single pulse as nak. If a decoder only responds with a single pulse, the block can be repeated.

> [!WARNING]  
> Please note that the blocks must be of different lengths depending on the decoder ID. Decoders with ID 200, 202, 203, 204 and 205 use 32 byte long blocks, all other decoders use 64 bytes.

### ZPP
When the first generation of ZIMO sound decoders (MX) was developed, the DECUP protocol was extended to include ZPP updates. Fortunately, the meaning of the transmitted data no longer depends only on the order, but command codes have been defined.

#### Entry
In contrast to the ZSU update, the entry sequence has been extended by another 0xEF byte. The **300µs** pause between each individual byte must also be observed here.

| Length | Value | Description         |
| ------ | ----- | ------------------- |
| 1 byte | 0xBF  | First preamble byte |

| Length | Value | Description          |
| ------ | ----- | -------------------- |
| 1 byte | 0xEF  | Second preamble byte |

| Length | Value | Description         |
| ------ | ----- | ------------------- |
| 1 byte | 0xEF  | Third preamble byte |

#### CV Read
**CV Read** reads a single CV value from a decoder. Since a single CV value naturally has 8 bits, the receiver is offered 8 feedback windows within the packet, in each of which one bit must be reported back as set or cleared.

| Length | Value      | Description                                              |
| ------ | ---------- | -------------------------------------------------------- |
| 1 byte | 0x01       | Command                                                  |
| 1 byte |            | CV address high byte                                     |
| 1 byte |            | CV address low byte                                      |
| 1 ms   | \| or \|\| | Single pulse if bit0 cleared<br>Double pulse if bit0 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit1 cleared<br>Double pulse if bit1 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit2 cleared<br>Double pulse if bit2 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit3 cleared<br>Double pulse if bit3 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit4 cleared<br>Double pulse if bit4 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit5 cleared<br>Double pulse if bit5 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit6 cleared<br>Double pulse if bit6 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit7 cleared<br>Double pulse if bit7 set |

#### CV Write (Deprecated)
**CV Write** writes a single CV value to a decoder.

| Length | Value | Description             |
| ------ | ----- | ----------------------- |
| 1 byte | 0x02  | Command                 |
| 1 byte | 0xAA  |                         |
| 1 byte |       | CV address high byte    |
| 1 byte |       | CV address low byte     |
| 1 byte |       | CV value                |
| 5 ms   | \|    | Single pulse on success |

> [!WARNING]  
> This version of the command is deprecated and only provided here for documentation purposes. If your decoder expects this command, please update the firmware.

#### CV Write
**CV Write** writes a single CV value to a decoder.

| Length | Value | Description             |
| ------ | ----- | ----------------------- |
| 1 byte | 0x06  | Command                 |
| 1 byte | 0xAA  |                         |
| 1 byte |       | CV address high byte    |
| 1 byte |       | CV address low byte     |
| 1 byte |       | CRC8 checksum           |
| 1 byte |       | CV value                |
| 5 ms   | \|\|  | Double pulse on success |

#### Flash Erase
**Flash Erase** erases the entire flash memory. Please note that deleting a NOR flash can take up to 60s depending on the manufacturer and type.

| Length | Value | Description             |
| ------ | ----- | ----------------------- |
| 1 byte | 0x03  | Command                 |
| 1 byte | 0x55  |                         |
| 1 byte | 0xFF  |                         |
| 1 byte | 0xFF  |                         |
| 200 s  | \|    | Single pulse on success |

#### Flash Write (Deprecated)
**Flash Write** writes to flash memory.

| Length   | Value      | Description                                        |
| -------- | ---------- | -------------------------------------------------- |
| 1 byte   | 0x03       | Command                                            |
| 1 byte   | 0x55       |                                                    |
| 1 byte   |            | Block count high byte                              |
| 1 byte   |            | Block count low byte                               |
| 256 byte |            |                                                    |
| 1 byte   |            | XOR checksum (starting at the block count)         |
| 1 ms     | \| or \|\| | Single pulse on failure<br>Double pulse on success |

> [!WARNING]  
> This version of the command is deprecated and only provided here for documentation purposes. If your decoder expects this command, please update the firmware.

#### Flash Write
**Flash Write** writes to flash memory.

| Length   | Value      | Description                                                         |
| -------- | ---------- | ------------------------------------------------------------------- |
| 1 byte   | 0x05       | Command                                                             |
| 1 byte   | 0x55       |                                                                     |
| 1 byte   |            | Block count high byte                                               |
| 1 byte   |            | Block count low byte                                                |
| 256 byte |            |                                                                     |
| 1 byte   |            | CRC8 checksum (starting at the block count, **initial value 0x55**) |
| 100 ms   | \| or \|\| | Single pulse on failure<br>Double pulse on success                  |

#### Decoder ID
**Decoder ID** reads a single byte decoder ID. Just like the **CV Read** command, this one contains 8 feedback windows.

| Length | Value      | Description                                              |
| ------ | ---------- | -------------------------------------------------------- |
| 1 byte | 0x04       | Command                                                  |
| 1 ms   | \| or \|\| | Single pulse if bit0 cleared<br>Double pulse if bit0 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit1 cleared<br>Double pulse if bit1 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit2 cleared<br>Double pulse if bit2 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit3 cleared<br>Double pulse if bit3 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit4 cleared<br>Double pulse if bit4 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit5 cleared<br>Double pulse if bit5 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit6 cleared<br>Double pulse if bit6 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit7 cleared<br>Double pulse if bit7 set |

#### CRC or XOR Query (Deprecated)
Queries a decoder wether it support a CRC8 checksum. Just like the **CV Read** command, this one contains 8 feedback windows. A returned value of 1 indicates that CRC8 checksums are supported.

| Length | Value      | Description                                              |
| ------ | ---------- | -------------------------------------------------------- |
| 1 byte | 0x07       | Command                                                  |
| 1 ms   | \| or \|\| | Single pulse if bit0 cleared<br>Double pulse if bit0 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit1 cleared<br>Double pulse if bit1 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit2 cleared<br>Double pulse if bit2 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit3 cleared<br>Double pulse if bit3 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit4 cleared<br>Double pulse if bit4 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit5 cleared<br>Double pulse if bit5 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit6 cleared<br>Double pulse if bit6 set |
| 1 byte | 0xFF       |                                                          |
| 1 ms   | \| or \|\| | Single pulse if bit7 cleared<br>Double pulse if bit7 set |

> [!WARNING]  
> This version of the command is deprecated and only provided here for documentation purposes. If your decoder expects this command, please update the firmware.

### Typical process
#### ZSU Update
1. [Entry](#entry) to enter ZSU firmware updates
2. [Decoder ID](#decoder-id) to find connected decoder type
3. [Block Count](#block-count)
4. [Security Bytes](#security-bytes)
5. [Blocks](#blocks) to transmit ZSU firmware data

#### ZPP Update
1. [Entry](#entry-1) to enter ZPP sound updates
2. [CV Read](#cv-read) to read CV8 (2x)
3. [Decoder ID](#decoder-id-1) to read decoder ID (2x)
4. [CV Read](#cv-read) to read CV7 (2x)
5. [Flash Erase](#flash-erase) to erase ZPP sound flash
6. [Flash Write](#flash-write) to write ZPP sound flash
7. [CV Write](#cv-write) to write CVs

## Getting Started
### Prerequisites
- C++23 compatible compiler
- [CMake](https://cmake.org/) ( >= 3.25 )
- Optional
  - for building [ESP32](https://www.espressif.com/en/products/socs/esp32) [RMT](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html) encoder example
    - [ESP-IDF](https://github.com/espressif/esp-idf) ( >= 5.0.3 )

### Installation
This library is meant to be consumed with CMake,

```cmake
# Either by including it with CPM
cpmaddpackage("gh:ZIMO-Elektronik/DECUP@0.1.6")

# or the FetchContent module
FetchContent_Declare(
  DECUP
  GIT_REPOSITORY "https://github.com/ZIMO-Elektronik/DECUP"
  GIT_TAG v0.1.6)

target_link_libraries(YourTarget PRIVATE DECUP::DECUP)
```

or, on [ESP32 platforms](https://www.espressif.com/en/products/socs/esp32), with the [IDF Component Manager](https://docs.espressif.com/projects/idf-component-manager/en/latest/) by adding it to a `idf_component.yml` file.
```yaml
dependencies:
  zimo-elektronik/decup:
    version: "0.1.6"
```

### Build
:construction:

#### ESP32
On [ESP32 platforms](https://www.espressif.com/en/products/socs/esp32) examples from the [examples](https://github.com/ZIMO-Elektronik/DECUP/raw/master/examples) subfolder can be built directly using the [IDF Frontend](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/tools/idf-py.html).

```sh
idf.py create-project-from-example "zimo-elektronik/decup^0.1.6:esp32"
```

## Usage
:construction: