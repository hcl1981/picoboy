# PicoBoy -- RP2040-Handheld mit 128x64-OLED (SH1106), 5-Wege-Joystick,
# drei LEDs, Lautsprecher und Beschleunigungssensor.
#
# Hardwareseitig ist der PicoBoy ein Raspberry Pi Pico, deshalb erben wir
# dessen Board-Definition aus dem pico-sdk unveraendert. Der einzige
# Unterschied zum Standard-Build ist das Manifest: picoboy.py und das
# Startmenue sind fest in die Firmware eingebacken.
set(PICO_BOARD "pico")
set(PICO_PLATFORM "rp2040")

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 1441792)  # 1408 * 1024
endif()

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
