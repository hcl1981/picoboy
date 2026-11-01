# Entspricht ports/rp2/boards/manifest.py, aber OHNE dessen _boot.py:
# wir frieren weiter unten unser eigenes ein, das beim allerersten Start
# die Datei main.py im Dateisystem anlegt.
freeze("$(PORT_DIR)/modules", ("rp2.py", "_boot_fat.py"))

include("$(MPY_DIR)/extmod/asyncio")
require("onewire")
require("ds18x20")
require("dht")
require("neopixel")

# picoboy.py, startmenu.py und unser _boot.py.
freeze("$(BOARD_DIR)/modules")
