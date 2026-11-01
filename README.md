# Lernen, Programmieren, Spielen – Der PicoBoy
Der Picoboy ist ein leistungsstarkes Mini-Handheld mit einer Größe von nur 3 x 5 cm. Er eignet sich, um das Programmieren zu lernen, eigene Spiele zu entwickeln oder einfach nur, um damit zu spielen. Eine Einführung in die Programmierung mit der Arduino-Umgebung und MicroPython ist auf dieses Seite zu finden,

Du benötigst dazu nichts als einen PC, den PicoBoy und ein USB-C-Kabel.

Da der PicoBoy kompatibel zum Raspberry Pi Pico und zur Arduino-Umgebung ist, finden sich im Netz unzählige weitere Tutorials, Beispiele und Bibliotheken, die das Programmieren erleichtern.

# Technische Daten
* 1,3“ s/w-OLED-Display mit 128 x 64 Pixeln
* durch RP2040-Mikrocontroller kompatibel zum Raspberry Pi Pico
* 2 x 133 MHz ARM M0+
* 2 MB Flash
* 264 KB RAM
* USB-C-Schnittstelle zur Programmierung und Datenübertragung
* Spielesammlung PicoPac vorinstalliert
* 5-Wege-Joystick
* Beschleunigungssensor
* Spannungsversorgung über USB-C oder eine CR2032-Knopfzelle

# Firmware und Werkzeuge
Im Ordner [Firmware](Firmware) liegt MicroPython 1.29 für den PicoBoy, mit eingebauter `picoboy.py` und Startmenü. Zum Aufspielen BOOT gedrückt halten, RES kurz drücken und die `.uf2`-Datei auf das Laufwerk `RPI-RP2` kopieren.

Programmieren geht direkt im Browser mit dem [PicoBoy Studio](https://picoboy.de/studio/), am besten in Google Chrome.

Weitere Informationen: [picoboy.de](https://www.picoboy.de/)

# Lizenz
MIT, siehe [LICENSE](LICENSE). `picoboy.py` enthält Code von Radomir Dopieralski und anderen unter MIT, der Vermerk steht in der Datei. Die Firmware baut auf MicroPython auf, ebenfalls MIT: [Firmware/LICENSE-micropython.txt](Firmware/LICENSE-micropython.txt).
