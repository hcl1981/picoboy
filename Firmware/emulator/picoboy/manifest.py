# Dieselben Module wie in der Firmware (ports/rp2/boards/PICOBOY/manifest.py).
# Die eingefrorenen Module brauchen wie dort keinen Platz im Heap.
freeze("$(PORT_DIR)/../rp2/modules", ("rp2.py",))
include("$(MPY_DIR)/extmod/asyncio")
require("onewire")
require("ds18x20")
require("dht")
require("neopixel")

# picoboy.py, startmenu.py und _boot.py: unveraendert aus der Firmware.
freeze("$(PORT_DIR)/../rp2/boards/PICOBOY/modules")

# Was auf dem Geraet in C steckt, hier die nachgebaute Hardware.
freeze("$(PORT_DIR)/variants/picoboy/modules")
