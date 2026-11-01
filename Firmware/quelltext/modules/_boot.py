# Wie das _boot.py des rp2-Ports, mit einer Ergaenzung: wenn das
# Dateisystem frisch angelegt werden musste -- also beim allerersten
# Start nach dem Flashen -- schreiben wir eine main.py hinein, die das
# Startmenue aufruft.
#
# Die main.py wird bewusst als echte Datei ins Dateisystem geschrieben
# und NICHT in die Firmware eingefroren. Nur so taucht sie in der
# Dateiliste auf, laesst sich mit jedem Editor oeffnen und genauso
# wieder loeschen wie jedes andere Programm. Wer sie loescht, landet
# beim Einschalten in der Konsole -- und wer sie zurueck will, ruft
# startmenu.restore() auf.
#
# Angelegt wird sie nur im except-Zweig unten, also wenn das
# Dateisystem gerade frisch formatiert wurde. Geloescht bleibt
# geloescht.
import vfs
import machine, rp2

bdev = rp2.Flash()
neu = False
try:
    fs = vfs.VfsLfs2(bdev, progsize=256)
except:
    vfs.VfsLfs2.mkfs(bdev, progsize=256)
    fs = vfs.VfsLfs2(bdev, progsize=256)
    neu = True
vfs.mount(fs, "/")

if neu:
    try:
        import startmenu

        with open("/main.py", "w") as f:
            f.write(startmenu.MAIN_PY)
        del startmenu
    except Exception as e:
        # Ein Fehler hier darf den Start nicht verhindern: lieber ohne
        # Autostart hochfahren als gar nicht.
        print("main.py konnte nicht angelegt werden:", e)

del vfs, bdev, fs, neu
