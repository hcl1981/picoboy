# Laeuft nach jedem Neustart, wie _boot.py auf dem Geraet.
import sys, builtins, gc
import uctypes as __uc
import _pb

__d = _pb.d
sys.path.append('/lib')
import _boot

del sys.modules['_boot']


def __eingabe(aufforderung=''):
    b = str(aufforderung).encode()
    n = __d('eingabe', __uc.addressof(b) if b else 0, len(b))
    if n < 0:
        raise KeyboardInterrupt
    z = bytearray(n)
    if n:
        __d('eingabe_kopie', __uc.addressof(z), n)
    return z.decode()


builtins.input = __eingabe
gc.collect()
