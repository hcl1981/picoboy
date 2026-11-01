"""Startmenue des PicoBoy.

Listet alle .py-Dateien im Speicher auf und startet die ausgewaehlte.
Steuerung: Joystick hoch/runter waehlt aus, druecken startet.

Dieses Modul steckt fest in der Firmware. Aufrufen laesst es sich
jederzeit von Hand:

    import startmenu
    startmenu.run()

Beim allerersten Start nach dem Flashen legt der PicoBoy dafuer eine
ganz normale main.py im Speicher an (siehe _boot.py). Die darfst du
aendern oder loeschen -- das Menue bleibt hier trotzdem erhalten.
"""

import os
import time
from picoboy import PicoBoy

# Wie viele Eintraege gleichzeitig auf den Bildschirm passen. Die
# Schrift ist 8 Pixel hoch, der Bildschirm 64 -- mit Titel und
# Seitenanzeige bleiben drei Zeilen uebrig.
MAX_VISIBLE = 3

# 128 Pixel / 8 Pixel pro Zeichen = 16 Zeichen pro Zeile. Die Liste
# beginnt bei x=15, also bleiben 14 Zeichen fuer den Dateinamen.
_NAME_CHARS = 14


def _kurz(name, n=_NAME_CHARS):
    """Schneidet zu lange Dateinamen ab, damit sie nicht ueber den
    Bildschirmrand hinauslaufen."""
    return name if len(name) <= n else name[: n - 1] + "~"


def list_py_files(current_script_name="main.py"):
    """Alle .py-Dateien im Hauptverzeichnis, ohne das Menue selbst.

    picoboy.py taucht hier nicht auf: die steckt in der Firmware und
    liegt gar nicht erst im Dateisystem.
    """
    py_files = [
        f
        for f in os.listdir()
        if f.endswith(".py") and f not in ("picoboy.py", current_script_name)
    ]
    py_files.sort()
    return py_files


def draw_menu(pb, py_files, selector):
    pb.fill(0)

    total_files = len(py_files)
    # Selector klemmen: sonst zeigt eine leere Seite "Seite 2/1" an.
    if selector >= total_files:
        selector = total_files - 1
    if selector < 0:
        selector = 0
    total_pages = (total_files + MAX_VISIBLE - 1) // MAX_VISIBLE
    page = selector // MAX_VISIBLE

    pb.text("Auswahl:", 5, 5)
    pb.text("Seite {}/{}".format(page + 1, total_pages), 5, 15)

    start_idx = page * MAX_VISIBLE
    end_idx = min(start_idx + MAX_VISIBLE, total_files)

    y_pos = 30
    for i in range(start_idx, end_idx):
        if i == selector:
            pb.text(">", 5, y_pos)
        pb.text(_kurz(py_files[i]), 15, y_pos)
        y_pos += 10

    pb.show()


def _warte_auf_loslassen(pb):
    """Wartet, bis der Joystick losgelassen ist.

    Ohne das sieht das gerade gestartete Programm den Tastendruck, mit
    dem es ausgewaehlt wurde, sofort noch einmal -- und beim Ruecksprung
    ins Menue wuerde es sich selbst gleich wieder starten.
    """
    while (
        pb.pressedUp()
        or pb.pressedDown()
        or pb.pressedLeft()
        or pb.pressedRight()
        or pb.pressedCenter()
    ):
        time.sleep_ms(20)


def _aufraeumen(pb):
    """Bringt die Hardware nach einem Programm in einen ruhigen Zustand
    zurueck: LEDs aus, Lautsprecher still."""
    try:
        pb.poweron()        # sonst bleibt eine Fehlermeldung unsichtbar
        pb.invert(False)
    except Exception:
        pass
    try:
        pb.LED_RED.off()
        pb.LED_YELLOW.off()
        pb.LED_GREEN.off()
    except Exception:
        pass
    try:
        from machine import Pin, PWM

        PWM(Pin(15)).duty_u16(0)
    except Exception:
        pass


def _zeige_fehler(pb, err):
    """Zeigt einen abgestuerzten Programmlauf an, statt das Menue mit in
    den Absturz zu reissen."""
    pb.fill(0)
    pb.text("Fehler im", 5, 5)
    pb.text("Programm:", 5, 15)
    name = type(err).__name__
    pb.text(_kurz(name, 16), 5, 30)
    pb.text("Joystick", 5, 47)
    pb.text("druecken", 5, 56)
    pb.show()
    print("Programm abgebrochen:", repr(err))
    time.sleep_ms(400)
    while not pb.pressedCenter():
        time.sleep_ms(20)


def launch_program(pb, filename):
    """Fuehrt die ausgewaehlte Datei aus und kehrt danach ins Menue
    zurueck -- auch dann, wenn das Programm abstuerzt."""
    pb.fill(0)
    pb.text("Starte", 5, 20)
    pb.text(_kurz(filename, 16), 5, 32)
    pb.show()
    time.sleep_ms(500)
    _warte_auf_loslassen(pb)

    pb.fill(0)
    pb.show()

    # Ein gemeinsames dict fuer globals und locals, damit Funktionen im
    # Programm die Variablen auf Modulebene auch wirklich sehen.
    ctx = {"__name__": "__main__"}
    try:
        with open(filename) as f:
            quelltext = f.read()
        exec(quelltext, ctx, ctx)
    except KeyboardInterrupt:
        raise
    except SystemExit:
        pass                # sys.exit() beendet das Programm, nicht das Menue
    except Exception as err:
        _aufraeumen(pb)
        _zeige_fehler(pb, err)
    finally:
        _aufraeumen(pb)
    _warte_auf_loslassen(pb)


def run(current_script_name="main.py"):
    """Startet das Menue. Kehrt nie zurueck."""
    pb = PicoBoy()
    selector = 0
    py_files = list_py_files(current_script_name)
    letzte_liste = time.ticks_ms()
    neu_zeichnen = True

    while True:
        # Die Dateiliste liegt auf dem Flash -- sie bei jedem
        # Schleifendurchlauf neu einzulesen waere reine Verschwendung.
        # Alle zwei Sekunden reicht voellig, damit ein frisch
        # hochgeladenes Programm von selbst auftaucht.
        if time.ticks_diff(time.ticks_ms(), letzte_liste) > 2000:
            letzte_liste = time.ticks_ms()
            aktuell = list_py_files(current_script_name)
            if aktuell != py_files:
                py_files = aktuell
                neu_zeichnen = True

        if not py_files:
            if neu_zeichnen:
                pb.fill(0)
                pb.text("Keine", 5, 12)
                pb.text("Programme", 5, 24)
                pb.text("im Speicher.", 5, 36)
                pb.show()
                neu_zeichnen = False
            time.sleep_ms(200)
            continue

        if selector >= len(py_files):
            selector = len(py_files) - 1
            neu_zeichnen = True

        # Nur zeichnen, wenn sich etwas geaendert hat: das spart
        # Rechenzeit und haelt das Bild ruhig.
        if neu_zeichnen:
            draw_menu(pb, py_files, selector)
            neu_zeichnen = False

        if pb.pressedUp():
            selector = (selector - 1) % len(py_files)
            neu_zeichnen = True
            time.sleep_ms(150)
        if pb.pressedDown():
            selector = (selector + 1) % len(py_files)
            neu_zeichnen = True
            time.sleep_ms(150)
        if pb.pressedCenter():
            launch_program(pb, py_files[selector])
            py_files = list_py_files(current_script_name)
            letzte_liste = time.ticks_ms()
            neu_zeichnen = True

        time.sleep_ms(50)


# Quelltext der main.py, die _boot.py beim ersten Start anlegt. Steht
# hier, damit man sie jederzeit wiederherstellen kann:
#     import startmenu; startmenu.restore()
MAIN_PY = """\
# Autostart des PicoBoy.
#
# Diese Datei laeuft automatisch, sobald das Geraet eingeschaltet wird.
# Sie startet das Menue, das alle Programme im Speicher auflistet.
#
# Das hier ist ein ganz normales Programm: du darfst es aendern,
# ueberschreiben oder loeschen. Ohne main.py startet der PicoBoy
# einfach in die Konsole.
#
# Menue von Hand starten:      import startmenu; startmenu.run()
# Diese Datei wiederherstellen: import startmenu; startmenu.restore()

import startmenu

startmenu.run()
"""


def restore(filename="main.py"):
    """Legt die Autostart-Datei neu an, falls sie geloescht wurde."""
    with open(filename, "w") as f:
        f.write(MAIN_PY)
    print(filename, "wurde angelegt.")
