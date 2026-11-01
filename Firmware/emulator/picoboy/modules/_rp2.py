# Was auf dem Geraet in C steckt (Modul _rp2). Der Flash-Speicher liegt
# im Worker und ueberlebt dort jeden Neustart. PIO und DMA nehmen Befehle
# an, bewegen aber nichts: am Emulator haengt keine Hardware an den Pins.
import _pb
import uctypes as __uc

__d = _pb.d


class Flash:
    def __init__(self, *a, **k):
        pass

    def readblocks(self, block, buf, off=0):
        __d('bd_lies', block, off, __uc.addressof(buf), len(buf))

    def writeblocks(self, block, buf, off=None):
        if off is None:
            __d('bd_loesche', block)
            off = 0
        __d('bd_schreib', block, off, __uc.addressof(buf), len(buf))

    def ioctl(self, op, arg):
        if op == 4:
            return 352
        if op == 5:
            return 4096
        if op == 6:
            __d('bd_loesche', arg)
            return 0
        return 0


class StateMachine:
    def __init__(self, id, *a, **k):
        self._id = id
        self._an = False

    def __repr__(self):
        return 'StateMachine(%d)' % self._id

    def init(self, *a, **k):
        pass

    def active(self, v=None):
        if v is None:
            return self._an
        self._an = bool(v)

    def restart(self):
        pass

    def exec(self, i):
        pass

    def get(self, buf=None, shift=0):
        return 0

    def put(self, v, shift=0):
        pass

    def rx_fifo(self):
        return 0

    def tx_fifo(self):
        return 0

    def irq(self, *a, **k):
        pass


class PIO:
    IN_LOW = 0
    IN_HIGH = 1
    OUT_LOW = 0
    OUT_HIGH = 1
    SHIFT_LEFT = 0
    SHIFT_RIGHT = 1
    JOIN_NONE = 0
    JOIN_TX = 1
    JOIN_RX = 2
    IRQ_SM0 = 256
    IRQ_SM1 = 512
    IRQ_SM2 = 1024
    IRQ_SM3 = 2048

    def __init__(self, id):
        if id not in (0, 1):
            raise ValueError('invalid PIO')
        self._id = id

    def __repr__(self):
        return 'PIO(%d)' % self._id

    def add_program(self, p):
        pass

    def remove_program(self, p=None):
        pass

    def state_machine(self, id, *a, **k):
        return StateMachine(self._id * 4 + id)

    def irq(self, *a, **k):
        pass


class DMA:
    def __init__(self):
        self._an = 0

    def config(self, **k):
        pass

    def active(self, v=None):
        if v is None:
            return False

    def irq(self, *a, **k):
        pass

    def close(self):
        pass

    def pack_ctrl(self, **k):
        return 0

    def unpack_ctrl(self, c):
        return {}


class DMATimer:
    def __init__(self, *a, **k):
        pass

    def close(self):
        pass


def bootsel_button():
    return 0
