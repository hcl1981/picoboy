#Speichere diese Datei als main.py, damit sie automatisch ausgeführt wird
import os
import time
from picoboy import PicoBoy

pb = PicoBoy()

MAX_VISIBLE = 3  # wie viele Einträge maximal pro „Seite“ angezeigt werden

def list_py_files(current_script_name):
    """
    Liest alle Dateien ein, filtert nach '.py', entfernt 'picoboy.py' und die Menüdatei selbst.
    """
    all_files = os.listdir()
    
    py_files = []
    for f in all_files:
        # Nur .py-Dateien verwenden, aber picoboy.py und das Menüskript selbst auslassen
        if f.endswith(".py") and f not in ["picoboy.py", current_script_name]:
            py_files.append(f)
    
    # Sortieren ist optional, aber meist hilfreich
    py_files.sort()
    return py_files

def draw_menu(py_files, selector, page):
    """
    Zeichnet das Menü auf dem Display.
    - py_files: Liste aller .py-Dateien
    - selector: Der Index des aktuell ausgewählten Eintrags (in der *Gesamtliste*, nicht nur in der Seite)
    - page: Seite in der Seitenaufteilung
    """
    pb.fill(0)  # schwarzer Hintergrund
    
    total_files = len(py_files)
    # Beispiel: Titel
    pb.text("Auswahl:", 5, 5)
    
    # Zeige Seiten-Info (z. B. "Seite 1/3")
    total_pages = (total_files + MAX_VISIBLE - 1) // MAX_VISIBLE  # aufrunden
    pb.text(f"Seite {page+1}/{total_pages}", 5, 15)
    
    # Start-Index und End-Index für die aktuelle Seite
    start_idx = page * MAX_VISIBLE
    end_idx   = min(start_idx + MAX_VISIBLE, total_files)
    
    y_pos = 30
    for i in range(start_idx, end_idx):
        filename = py_files[i]
        
        # Markierung für ausgewähltes Element
        if i == selector:
            # z. B. kleiner gefüllter Pfeil links
            pb.text(">", 5, y_pos)
            pb.text(filename, 15, y_pos)
        else:
            pb.text(filename, 15, y_pos)
        
        y_pos += 10
    
    pb.show()

def main():
    # Eigener Dateiname (dieses Skript) – musst du ggf. anpassen, falls du anders heißt.
    # Zum Beispiel "main.py"
    current_script_name = "main.py"  
    
    # Liste der Skripte laden
    py_files = list_py_files(current_script_name)
    
    # Falls keine Dateien gefunden:
    if not py_files:
        pb.fill(0)
        pb.text("Keine .py-Dateien gefunden", 5, 5)
        pb.show()
        while True:
            time.sleep_ms(100)
    
    selector = 0  # Ausgewählter Index in der Gesamtliste
    page = 0      # Aktuelle Seite
    total_files = len(py_files)
    
    # Hauptschleife
    while True:
        # Seite für das aktuelle selector-Element bestimmen
        page = selector // MAX_VISIBLE
        
        # Menü zeichnen
        draw_menu(py_files, selector, page)
        
        # Eingaben abfragen
        if pb.pressedUp():
            selector = (selector - 1) % total_files
            time.sleep_ms(150)  # "Entprellen"
        
        if pb.pressedDown():
            selector = (selector + 1) % total_files
            time.sleep_ms(150)
        
        if pb.pressedCenter():
            # Aktuell ausgewähltes Skript starten
            chosen_file = py_files[selector]
            launch_program(chosen_file)
        
        time.sleep_ms(50)  # CPU schonen

def launch_program(filename):
    """
    Lädt und führt die ausgewählte Datei aus. Danach kehrt man ins Menü zurück.
    """
    pb.fill(0)
    pb.text(f"Starte {filename}...", 5, 5)
    pb.show()
    time.sleep_ms(500)

    pb.fill(0)
    pb.show()
    
    exec_context = {} # this is necessary so that globals and locals use the same context
    exec(open(filename).read(),exec_context, exec_context)
    time.sleep_ms(2000)
        
# Programmstart
main()