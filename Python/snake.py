from picoboy import PicoBoy
import time
import random

# Spielfeldgrößen
xSize = 29
ySize = 15

# Erzeugt ein PicoBoy-Objekt (Grafik, Tasten etc.)
pb = PicoBoy()

# Erzeugt ein 2D-Array (Liste von Listen), das den Zustand des Spielfeldes speichert
# board[x][y] == 0 bedeutet Feld ist leer
# board[x][y] == -1 bedeutet Feld hat einen Apfel
# board[x][y] > 0 bedeutet, dass sich dort Teil des Schlangenkörpers befindet.
board = [[0 for x in range(ySize)] for y in range(xSize)]

##### Quellcode über dieser Linie bitte nicht ändern! #####

# Schlangen-Startlänge
laenge = 3

# Startposition der Schlange (mittig)
xPos = 14
yPos = 7

# Zufällige Start-Richtung: 1=oben, 2=unten, 3=links, 4=rechts
richtung = random.randint(1,4)

def putApple():
    """
    Platziert einen Apfel an einer zufälligen, freien Stelle im Spielfeld.
    (board[x][y] == -1)
    """
    global board
    x = random.randint(0, xSize - 1)
    y = random.randint(0, ySize - 1)
    # Solange das zufällig gewählte Feld nicht leer ist, suche ein neues Feld
    while board[x][y] != 0:
        x = random.randint(0, xSize - 1)
        y = random.randint(0, ySize - 1)
    board[x][y] = -1

def erlaubt(x, y):
    """
    Überprüft, ob die Position (x, y) gültig ist:
    - noch innerhalb des Spielfelds
    - und die Schlange nicht auf sich selbst stößt
    """
    if x < 0 or x >= xSize or y < 0 or y >= ySize or board[x][y] > 0:
        return False
    else:
        return True

def spielfeldLeeren():
    """
    Setzt alle Felder im Spielfeld (board) auf 0, 
    damit die Schlange und Äpfel komplett entfernt werden.
    """
    global board
    for i in range(xSize):
        for j in range(ySize):
            board[i][j] = 0

def score():
    """
    Zeigt den aktuellen Punktestand (basierend auf der Schlangenlänge) 
    an einer festen Stelle auf dem Display an.
    """
    global laenge
    punkte = laenge - 3  # Punkte = (Schlangenlänge - 3)
    # Wir zeigen die letzten drei Stellen des Punktezählers an (Hunderter, Zehner, Einer)
    pb.text(str(punkte % 10), 121, 38, 1)
    punkte = punkte // 10
    pb.text(str(punkte % 10), 121, 28, 1)
    punkte = punkte // 10
    pb.text(str(punkte % 10), 121, 18, 1)

def spielfeldZeichnen():
    """
    Zeichnet den Spielfeld-Rahmen, die Schlange und die Äpfel.
    Aktualisiert außerdem den 'Alterungsprozess' der Schlange, 
    indem ihre Körperteile decrementiert werden (Wert > 0 um 1 verringern).
    """
    global board
    global laenge
    
    score()
    # Äußerer Rahmen
    pb.rect(0, 0, 4 * (xSize + 1), 4 * (ySize + 1), 1)
    
    for i in range(xSize):
        for j in range(ySize):
            if board[i][j] > 0:
                # Kopf der Schlange
                if board[i][j] == laenge:
                    pb.fill_rect(i * 4 + 2, j * 4 + 2, 4, 4, 1)
                # Körper der Schlange
                else:
                    pb.rect(i * 4 + 2, j * 4 + 2, 4, 4, 1)
                # 'Alter' jedes Schlangenglieds wird um 1 verringert
                board[i][j] -= 1
            
            # Apfel
            elif board[i][j] == -1:
                pb.fill_rect(i * 4 + 3, j * 4 + 3, 2, 2, 1)

def schritt():
    """
    Führt einen Spielschritt aus:
    - Tasteneingaben auswerten (Richtung ändern)
    - Neue Kopfposition berechnen
    - Falls ungültige Position -> gameOverScreen
    - Falls Apfel -> Schlange wächst
    - Neue Kopfposition im Spielfeld eintragen
    """
    global xPos, yPos, board, richtung, laenge
    
    # Richtungsänderungen nur zulassen, wenn die neue Richtung
    # nicht exakt die entgegengesetzte Richtung ist.
    if pb.pressedUp() and richtung != 2:
        richtung = 1
    if pb.pressedDown() and richtung != 1:
        richtung = 2
    if pb.pressedLeft() and richtung != 4:
        richtung = 3
    if pb.pressedRight() and richtung != 3:
        richtung = 4
        
    # Kopfposition entsprechend der Richtung aktualisieren
    if richtung == 1:
        yPos -= 1
    elif richtung == 2:
        yPos += 1
    elif richtung == 3:
        xPos -= 1
    elif richtung == 4:
        xPos += 1
        
    # Überprüfen, ob die neue Position erlaubt ist
    if not erlaubt(xPos, yPos):
        gameOverScreen()
        return  # Dieser Spielschritt ist dann beendet

    # Wenn dort ein Apfel war, Schlange verlängern und neuen Apfel setzen
    if board[xPos][yPos] == -1:
        laenge += 1
        putApple()
        
    # Neue Kopfposition als höchsten Schlangenwert eintragen (Kopf)
    board[xPos][yPos] = laenge

def gameOverScreen():
    """
    Zeigt einen "Game Over"-Bildschirm mit dem erreichten Score
    und startet danach automatisch ein neues Spiel.
    """
    global laenge

    # Schwarzes Rechteck über dem mittleren Bereich
    pb.fill_rect(10, 10, 4 * (xSize + 1) - 20, 4 * (ySize + 1) - 20, 0)
    # Rahmen drumherum
    pb.rect(10, 10, 4 * (xSize + 1) - 20, 4 * (ySize + 1) - 20, 1)

    pb.text("Game Over!", 18, 18)
    pb.text("Score:", 18, 28)
    pb.text(str(laenge - 3), 70, 28)
    pb.show()

    # Kurz warten, damit der Spieler den Score sehen kann
    time.sleep_ms(3000)
    
    # Dann ein neues Spiel starten
    newGame()

def newGame():
    """
    Setzt alle Spiel-Variablen auf den Anfangszustand zurück,
    leert das Spielfeld und platziert einen Apfel.
    """
    global laenge, xPos, yPos, richtung
    
    laenge = 3
    xPos = 14
    yPos = 7
    richtung = random.randint(1,4)
    
    spielfeldLeeren()
    putApple()

# Initiales Leeren des Spielfelds und Apfel setzen
spielfeldLeeren()
putApple()

# Startzeit erfassen (für die Geschwindigkeitssteuerung)
last = time.ticks_us()

while True:
    # Einen Spielschritt ausführen
    schritt()
    
    # Bildschirm leeren und neues Spielfeld zeichnen
    pb.fill(0)
    spielfeldZeichnen()
    pb.show()
    
    # Warte so lange, bis 75ms vergangen sind (Spielgeschwindigkeit)
    while time.ticks_diff(time.ticks_us(), last) < 75000:
        pass
    last = time.ticks_us()

