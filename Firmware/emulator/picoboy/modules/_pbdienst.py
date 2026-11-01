# Antwort an das Studio (Dateiliste, Dateiinhalt).
import _pb, uctypes


def ergebnis(text):
    b = text.encode()
    _pb.d('ergebnis', uctypes.addressof(b) if b else 0, len(b))
