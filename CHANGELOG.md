# Changelog

## 0.2.0
- Rename `decoder_id2data_size` to `decoder_id2block_size`
- Add `decoder_id2bootloader_size` function
- Bugfix adjust CV write timeout to 5ms

## 0.1.6
- Add `pulse_interval` member to `Timeouts`

## 0.1.5
- Bugfix increase timeouts ([#8](https://github.com/ZIMO-Elektronik/DECUP/issues/8))

## 0.1.4
- Bugfix add missing stop bit ([#3](https://github.com/ZIMO-Elektronik/DECUP/issues/3))

## 0.1.3
- Bugfix add missing `command.hpp` header

## 0.1.2
- Bugfix add both preamble bytes to `Command`

## 0.1.1
- Bugfix place `rmt_decup_encoder_reset` in IRAM_ATTR

## 0.1.0
- Initial release