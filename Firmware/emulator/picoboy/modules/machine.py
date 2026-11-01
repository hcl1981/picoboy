# machine fuer den PicoBoy-Emulator.
#
# Auf dem Geraet steckt dieses Modul in C und spricht echte Hardware an.
# Hier bildet es dieselbe Schnittstelle nach, gegen eine Platine mit dem
# Bestueckungsplan des PicoBoy: Display (SPI 0), Sensor (I2C 0, Adresse
# 0x18), Joystick (GPIO 0-4), LEDs (GPIO 5-7), Lautsprecher (GPIO 15).
# Repr-Texte, Fehlermeldungen und Rechenwege stammen vom Geraet, siehe
# dokumentation/werkzeuge/paritaet/. Die picoboy.py aus der Firmware
# laeuft unveraendert darauf.
#
# Hilfsnamen beginnen mit zwei Unterstrichen: dir(machine) soll zeigen,
# was auf dem Geraet auch dort steht.
from _pb import d as __d, set_tick as __setze_tick
import utime as __time
import uctypes as __uc
import sys as __sys

__freq = [125000000]

PWRON_RESET = 1
WDT_RESET = 3

# ---- GPIO --------------------------------------------------------
__ALT = {1: 'SPI', 2: 'UART', 3: 'I2C', 4: 'PWM', 5: 'SIO', 6: 'PIO0', 7: 'PIO1', 8: 'GPCK', 9: 'USB'}
# [mode, pull, alt, wert]; alt 31 = Grundzustand nach dem Einschalten
__gp = {}
__irq = {}


def __zustand(i):
    z = __gp.get(i)
    if z is None:
        z = [3, 2, 31, 0]
        __gp[i] = z
    return z


class __Irq:
    def __init__(self, ausloeser):
        self._t = ausloeser
        self._f = 0

    def trigger(self, t=None):
        return self._t

    def flags(self):
        f = self._f
        self._f = 0
        return f


class Pin:
    IN = 0
    OUT = 1
    OPEN_DRAIN = 2
    ALT = 3
    PULL_UP = 1
    PULL_DOWN = 2
    IRQ_FALLING = 4
    IRQ_RISING = 8
    ALT_SPI = 1
    ALT_UART = 2
    ALT_I2C = 3
    ALT_PWM = 4
    ALT_SIO = 5
    ALT_PIO0 = 6
    ALT_PIO1 = 7
    ALT_GPCK = 8
    ALT_USB = 9

    class board:
        pass

    class cpu:
        pass

    def __init__(self, id, mode=-1, pull=-1, value=None, alt=-1):
        if isinstance(id, Pin):
            id = id._i
        elif isinstance(id, str):
            if id.startswith('GPIO') and id[4:].isdigit() and int(id[4:]) < 30:
                id = int(id[4:])
            else:
                raise ValueError('unknown named pin "%s"' % id)
        elif not isinstance(id, int) or id < 0 or id > 29:
            raise ValueError('invalid pin')
        self._i = id
        if mode != -1 or pull != -1 or value is not None or alt != -1:
            self.init(mode, pull, value, alt)

    def init(self, mode=-1, pull=-1, value=None, alt=-1):
        z = __zustand(self._i)
        if mode != -1:
            if mode not in (0, 1, 2, 3):
                raise ValueError('invalid pin mode')
            z[0] = mode
            z[2] = alt if mode == 3 and alt != -1 else 5
        z[1] = 0 if pull == -1 and mode != -1 else (z[1] if pull == -1 else pull)
        if value is not None:
            self.value(value)

    def __lesen(self):
        z = __zustand(self._i)
        i = self._i
        if i < 5:
            return 0 if __d('taste', i) else (1 if z[1] == 1 else 0)
        if z[0] == 1 or z[0] == 2:
            return z[3]
        return 1 if z[1] == 1 else 0

    def value(self, v=None):
        if v is None:
            return self.__lesen()
        z = __zustand(self._i)
        z[3] = 1 if v else 0
        i = self._i
        if 5 <= i <= 7 and z[0] == 1:
            __d('led', i - 5, z[3])
        elif i == 9 and z[3] == 0:
            __d('panel_reset')

    def __call__(self, v=None):
        return self.value(v)

    def on(self):
        self.value(1)

    def off(self):
        self.value(0)

    def high(self):
        self.value(1)

    def low(self):
        self.value(0)

    def toggle(self):
        z = __zustand(self._i)
        self.value(0 if z[3] else 1)

    def irq(self, handler=None, trigger=12, hard=False):
        i = self._i
        o = __Irq(trigger)
        if handler is None:
            __irq.pop(i, None)
        else:
            __irq[i] = (handler, trigger, self.__lesen(), o, self)
        m = 0
        for k in __irq:
            m |= 1 << k
        __d('irq_maske', m)
        return o

    def __repr__(self):
        z = __zustand(self._i)
        s = 'Pin(GPIO%d, mode=%s' % (self._i, ('IN', 'OUT', 'OPEN_DRAIN', 'ALT')[z[0]])
        if z[1]:
            s += ', pull=' + ('', 'PULL_UP', 'PULL_DOWN')[z[1]]
        if z[0] == 3:
            s += ', alt=' + str(__ALT.get(z[2], z[2]))
        return s + ')'


for __n in range(30):
    setattr(Pin.cpu, 'GPIO%d' % __n, __n)


def __pin_nr(p):
    if isinstance(p, Pin):
        return p._i
    return Pin(p)._i


def __alt(nr, art):
    z = __zustand(nr)
    z[0] = 3
    z[2] = art


# ---- PWM ---------------------------------------------------------
# Rechenwege aus ports/rp2/machine_pwm.c, je Slice ein Teiler und ein TOP.
__slice = {}


def __hz(offset, div16):
    return (16 * __freq[0] + offset) // div16


def __hz_rund(div16):
    return __hz(div16 // 2, div16)


class PWM:
    def __init__(self, dest, freq=-1, duty_u16=-1, duty_ns=-1, invert=-1):
        self._pin = __pin_nr(dest)
        self._s = (self._pin // 2) % 8
        self._k = self._pin % 2
        self._inv = 0
        __alt(self._pin, 4)
        if self._s not in __slice:
            __slice[self._s] = [16, 65535, False, {}]   # div16, top, gesetzt, kanaele
        self.init(freq, duty_u16, duty_ns, invert)

    def init(self, freq=-1, duty_u16=-1, duty_ns=-1, invert=-1):
        if freq != -1:
            self.freq(freq)
        if duty_u16 != -1:
            self.duty_u16(duty_u16)
        if duty_ns != -1:
            self.duty_ns(duty_ns)
        if invert != -1:
            self._inv = 1 if invert else 0

    def __repr__(self):
        return '<PWM slice=%d channel=%d invert=%d>' % (self._s, self._k, self._inv)

    def __anteil(self, u16):
        # was am Pin ankommt: Lautsprecher und LEDs
        if self._pin == 15:
            __d('ton_duty', u16)
        elif 5 <= self._pin <= 7:
            __d('led', self._pin - 5, 1 if u16 else 0)

    def freq(self, f=None):
        s = __slice[self._s]
        if f is None:
            return __hz_rund(s[0] * (s[1] + 1)) if s[2] else 0
        q = __freq[0]
        if f <= 0:
            raise ValueError('freq too small')
        if (q + f // 2) // f < 65534:
            div16 = 16
            top = (q + f // 2) // f - 1
        else:
            div16 = __hz(65534 * f - 1, 65534 * f)
            top = __hz_rund(div16 * f) - 1
        if div16 < 16:
            raise ValueError('freq too large')
        if div16 >= 256 * 16:
            raise ValueError('freq too small')
        s[0] = div16
        s[1] = top
        s[2] = True
        if self._pin == 15:
            __d('ton_hz', __hz_rund(div16 * (top + 1)))
        for k in s[3]:
            w = s[3][k]
            if w[0] == 1:
                self.__u16(k, w[1])

    def __u16(self, k, d):
        s = __slice[self._s]
        s[3][k] = [1, d, d * (s[1] + 1) // 65535]

    def duty_u16(self, d=None):
        s = __slice[self._s]
        w = s[3].get(self._k)
        if d is None:
            if w is None or not s[2]:
                return 0
            return d if d else min(65535, w[2] * 65535 // s[1] if s[1] else 0) if w[0] == 2 else w[1]
        self.__u16(self._k, d)
        self.__anteil(d)

    def duty_ns(self, d=None):
        s = __slice[self._s]
        hz = __hz_rund(s[0])
        w = s[3].get(self._k)
        if d is None:
            if w is None or not s[2]:
                return 0
            return (w[2] * 1000000000 + hz // 2) // hz
        cc = (d * hz + 500000000) // 1000000000
        if cc > s[1] + 1:
            cc = s[1] + 1
        s[3][self._k] = [2, d, cc]
        self.__anteil(cc * 65535 // (s[1] + 1))

    def deinit(self):
        s = __slice[self._s]
        s[3].pop(self._k, None)
        if not s[3]:
            s[2] = False
        self.__anteil(0)


# ---- ADC: nichts angeschlossen, jeder Kanal liefert 0 V --------------
class ADC:
    CORE_TEMP = 4

    def __init__(self, id):
        if isinstance(id, Pin):
            nr = id._i
        else:
            nr = id
            if not isinstance(nr, int):
                nr = Pin(nr)._i
            elif 0 <= nr <= 4:
                self._k = nr
                return
        if 26 <= nr <= 29:
            self._k = nr - 26
        else:
            raise ValueError("Pin doesn't have ADC capabilities")

    def __repr__(self):
        return '<ADC channel=%d>' % self._k

    def read_u16(self):
        return 0


# ---- I2C ---------------------------------------------------------
__acc_reg = {0x00: 0x87}


def __acc_lesen(reg):
    # STK8BA58: 12 Bit links im Registerpaar, Hoechstwert +-2 g.
    if 2 <= reg <= 7:
        roh = (__d('acc', (reg - 2) // 2) * 1024 // 1000) & 0xfff
        return ((roh & 15) << 4) if reg % 2 == 0 else (roh >> 4) & 0xff
    return __acc_reg.get(reg, 0)


class __I2CBasis:
    def __da(self, addr):
        if self._sda == 20 and self._scl == 21 and addr == 0x18:
            return True
        raise OSError(5)

    def scan(self):
        return [0x18] if self._sda == 20 and self._scl == 21 else []

    def writeto(self, addr, buf, stop=True):
        self.__da(addr)
        if len(buf):
            self._ptr = buf[0]
            for b in buf[1:]:
                __acc_reg[self._ptr] = b
                self._ptr += 1
        return len(buf)

    def writevto(self, addr, vector, stop=True):
        return self.writeto(addr, b''.join(vector), stop)

    def readfrom(self, addr, n, stop=True):
        self.__da(addr)
        r = bytearray(n)
        for i in range(n):
            r[i] = __acc_lesen(self._ptr)
            self._ptr += 1
        return bytes(r)

    def readfrom_into(self, addr, buf, stop=True):
        buf[:] = self.readfrom(addr, len(buf), stop)

    def writeto_mem(self, addr, reg, buf, addrsize=8):
        self.writeto(addr, bytes([reg]) + bytes(buf))

    def readfrom_mem(self, addr, reg, n, addrsize=8):
        self.__da(addr)
        self._ptr = reg
        return self.readfrom(addr, n)

    def readfrom_mem_into(self, addr, reg, buf, addrsize=8):
        buf[:] = self.readfrom_mem(addr, reg, len(buf))

    def start(self):
        pass

    def stop(self):
        pass

    def readinto(self, buf, nack=True):
        for i in range(len(buf)):
            buf[i] = 0xff

    def write(self, buf):
        return len(buf)

    def init(self, *a, **k):
        pass

    def deinit(self):
        pass


__I2C_STD = {0: (9, 8), 1: (7, 6)}


class I2C(__I2CBasis):
    def __init__(self, id, scl=None, sda=None, freq=400000, timeout=50000):
        if id not in (0, 1):
            raise ValueError("I2C(%d) doesn't exist" % id)
        s = __I2C_STD[id]
        self._id = id
        self._scl = s[0] if scl is None else __pin_nr(scl)
        self._sda = s[1] if sda is None else __pin_nr(sda)
        if self._scl % 2 != 1 or (self._scl // 2 % 2) != id:
            raise ValueError('bad SCL pin')
        if self._sda % 2 != 0 or (self._sda // 2 % 2) != id:
            raise ValueError('bad SDA pin')
        self._f = freq
        self._t = timeout
        self._ptr = 0
        __alt(self._scl, 3)
        __alt(self._sda, 3)

    def __repr__(self):
        return 'I2C(%d, freq=%d, scl=%d, sda=%d, timeout=%d)' % (self._id, self._f, self._scl, self._sda, self._t)


class SoftI2C(__I2CBasis):
    def __init__(self, scl, sda, freq=400000, timeout=50000):
        self._scl = __pin_nr(scl)
        self._sda = __pin_nr(sda)
        self._f = freq
        self._t = timeout
        self._ptr = 0

    def __repr__(self):
        return 'SoftI2C(scl=%d, sda=%d, freq=%d)' % (self._scl, self._sda, self._f)


class I2CTarget:
    IRQ_ADDR_MATCH_READ = 1
    IRQ_ADDR_MATCH_WRITE = 2
    IRQ_READ_REQ = 4
    IRQ_WRITE_REQ = 8
    IRQ_END_READ = 16
    IRQ_END_WRITE = 32

    def __init__(self, *a, **k):
        pass

    def deinit(self):
        pass

    def irq(self, *a, **k):
        pass

    def readinto(self, buf):
        return 0

    def write(self, buf):
        return 0


# ---- SPI ---------------------------------------------------------
def __spi_baud(b):
    # spi_set_baudrate aus dem pico-sdk, clk_peri laeuft fest mit 48 MHz
    q = 48000000
    v = 2
    while v <= 254:
        if q < (v + 2) * 256 * b:
            break
        v += 2
    n = 256
    while n > 1:
        if q // (v * (n - 1)) > b:
            break
        n -= 1
    return q // (v * n)


__spi = {}
__SPI_STD = {0: (18, 19, 16), 1: (10, 11, 8)}


class SPI:
    LSB = 0
    MSB = 1

    def __init__(self, id, baudrate=-1, polarity=-1, phase=-1, bits=-1, firstbit=-1, sck=None, mosi=None, miso=None):
        if id not in (0, 1):
            raise ValueError("SPI(%d) doesn't exist" % id)
        self._id = id
        z = __spi.get(id)
        if z is None:
            s = __SPI_STD[id]
            z = [1000000, 0, 0, 8, s[0], s[1], s[2]]
            __spi[id] = z
        self._z = z
        self.init(baudrate, polarity, phase, bits, firstbit, sck, mosi, miso)

    def init(self, baudrate=-1, polarity=-1, phase=-1, bits=-1, firstbit=-1, sck=None, mosi=None, miso=None):
        z = self._z
        if baudrate != -1:
            z[0] = __spi_baud(baudrate)
        if polarity != -1:
            z[1] = polarity
        if phase != -1:
            z[2] = phase
        if bits != -1:
            z[3] = bits
        if sck is not None:
            z[4] = __pin_nr(sck)
        if mosi is not None:
            z[5] = __pin_nr(mosi)
        if miso is not None:
            z[6] = __pin_nr(miso)

    def __repr__(self):
        z = self._z
        return 'SPI(%d, baudrate=%d, polarity=%d, phase=%d, bits=%d, sck=%d, mosi=%d, miso=%d)' % (self._id, z[0], z[1], z[2], z[3], z[4], z[5], z[6])

    def deinit(self):
        pass

    def write(self, buf):
        # Display: SPI 0, CS auf GPIO 10, D/C auf GPIO 8
        if self._id == 0 and __zustand(10)[3] == 0:
            __d('spi', __zustand(8)[3], __uc.addressof(buf), len(buf))

    def read(self, n, write=0):
        return bytes(n)

    def readinto(self, buf, write=0):
        for i in range(len(buf)):
            buf[i] = 0

    def write_readinto(self, out, buf):
        self.write(out)
        self.readinto(buf)


class SoftSPI(SPI):
    def __init__(self, baudrate=500000, polarity=0, phase=0, bits=8, firstbit=1, sck=None, mosi=None, miso=None):
        self._id = -1
        self._z = [baudrate, polarity, phase, bits, __pin_nr(sck), __pin_nr(mosi), __pin_nr(miso)]
        self._fb = firstbit

    def init(self, *a, **k):
        pass

    def __repr__(self):
        z = self._z
        return 'SoftSPI(baudrate=%d, polarity=%d, phase=%d, firstbit=%d, sck=%d, mosi=%d, miso=%d)' % (z[0], z[1], z[2], self._fb, z[4], z[5], z[6])


# ---- UART: nichts angeschlossen ----------------------------------------
__UART_STD = {0: (0, 1), 1: (4, 5)}


class UART:
    CTS = 2
    RTS = 1
    INV_TX = 1
    INV_RX = 2
    IRQ_RXIDLE = 4096
    IRQ_TXIDLE = 32768
    IRQ_BREAK = 512

    def __init__(self, id, baudrate=115200, bits=8, parity=None, stop=1, tx=None, rx=None, timeout=0, timeout_char=None, txbuf=256, rxbuf=256, invert=None, flow=0):
        if id not in (0, 1):
            raise ValueError("UART(%d) doesn't exist" % id)
        s = __UART_STD[id]
        self._id = id
        self._b = baudrate
        self._w = (bits, parity, stop, s[0] if tx is None else __pin_nr(tx), s[1] if rx is None else __pin_nr(rx), txbuf, rxbuf, timeout)
        self._tc = (timeout_char if timeout_char is not None else max(1, (13000 + baudrate - 1) // baudrate))
        self._frei = 0
        __alt(self._w[3], 2)
        __alt(self._w[4], 2)

    def init(self, *a, **k):
        pass

    def deinit(self):
        pass

    def __repr__(self):
        w = self._w
        return 'UART(%d, baudrate=%d, bits=%d, parity=%s, stop=%d, tx=%d, rx=%d, txbuf=%d, rxbuf=%d, timeout=%d, timeout_char=%d, invert=None, irq=0)' % (self._id, self._b, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7], self._tc)

    def write(self, buf):
        self._frei = __time.ticks_add(__time.ticks_us(), len(buf) * 10000000 // self._b)
        return len(buf)

    def any(self):
        return 0

    def read(self, n=-1):
        return None

    def readinto(self, buf, n=-1):
        return None

    def readline(self):
        return None

    def txdone(self):
        return __time.ticks_diff(__time.ticks_us(), self._frei) >= 0

    def flush(self):
        pass

    def sendbreak(self):
        pass

    def irq(self, *a, **k):
        pass


# ---- Timer -------------------------------------------------------
# Timer laufen als Soft-IRQ wie auf dem Geraet: der Worker meldet, wann
# der naechste faellig ist, der Interpreter ruft dann __tick auf.
__timer = []


def __planen():
    z = None
    for t in __timer:
        if z is None or __time.ticks_diff(t._due, z) < 0:
            z = t._due
    if z is not None:
        __d('timer_ziel', max(0, __time.ticks_diff(z, __time.ticks_ms())))


def __ruf(f, a):
    try:
        f(a)
    except Exception as e:
        print('Unhandled exception in IRQ callback handler')
        __sys.print_exception(e)


def __tick(_):
    jetzt = __time.ticks_ms()
    for t in list(__timer):
        if __time.ticks_diff(jetzt, t._due) >= 0:
            if t._mode == 0:
                __timer.remove(t)
            else:
                t._due = __time.ticks_add(t._due, t._ms)
                if __time.ticks_diff(jetzt, t._due) > 0:
                    t._due = __time.ticks_add(jetzt, t._ms)
            if t._cb is not None:
                __ruf(t._cb, t)
    for i in __irq:
        e = __irq[i]
        w = e[4].value()
        if w != e[2]:
            f = 8 if w else 4
            __irq[i] = (e[0], e[1], w, e[3], e[4])
            if e[1] & f:
                e[3]._f = f
                __ruf(e[0], e[4])
    __planen()


__setze_tick(__tick)


class Timer:
    ONE_SHOT = 0
    PERIODIC = 1

    def __init__(self, id=-1, mode=1, freq=-1, period=-1, tick_hz=1000, callback=None, hard=False):
        if id != -1:
            raise ValueError("Timer doesn't exist")
        self._us = 0
        self._mode = 1
        self._cb = None
        if freq != -1 or period != -1:
            self.init(mode=mode, freq=freq, period=period, tick_hz=tick_hz, callback=callback, hard=hard)

    def init(self, mode=1, freq=-1, period=-1, tick_hz=1000, callback=None, hard=False):
        self.deinit()
        self._mode = mode
        self._cb = callback
        if freq != -1:
            self._us = 1000000 // freq if freq else 0
        else:
            self._us = period * 1000000 // tick_hz
        self._ms = max(1, (self._us + 999) // 1000)
        self._due = __time.ticks_add(__time.ticks_ms(), self._ms)
        __timer.append(self)
        __planen()

    def deinit(self):
        if self in __timer:
            __timer.remove(self)

    def __repr__(self):
        return 'Timer(mode=%s, tick_hz=1000000, period=%d)' % ('PERIODIC' if self._mode else 'ONE_SHOT', self._us)


# ---- Uhr, Wachhund, Kleinkram ----------------------------------------
class RTC:
    def datetime(self, dt=None):
        if dt is None:
            t = __time.localtime()
            return (t[0], t[1], t[2], t[6], t[3], t[4], t[5], 0)
        __d('rtc', __time.mktime((dt[0], dt[1], dt[2], dt[4], dt[5], dt[6], 0, 0)))


class WDT:
    def __init__(self, id=0, timeout=5000):
        if id != 0:
            raise ValueError("WDT(%d) doesn't exist" % id)
        self._ms = timeout
        self._t = Timer(mode=0, period=timeout, callback=WDT.__ablauf)

    @staticmethod
    def __ablauf(_):
        reset()

    def feed(self):
        self._t.init(mode=0, period=self._ms, callback=WDT.__ablauf)


class Signal:
    def __init__(self, pin, *args, invert=False, **kw):
        self._p = pin if isinstance(pin, Pin) else Pin(pin, *args, **kw)
        self._i = 1 if invert else 0

    def value(self, v=None):
        if v is None:
            return self._p.value() ^ self._i
        self._p.value((1 if v else 0) ^ self._i)

    def on(self):
        self.value(1)

    def off(self):
        self.value(0)

    def __call__(self, v=None):
        return self.value(v)


class I2S:
    RX = 0
    TX = 5
    MONO = 0
    STEREO = 1

    def __init__(self, *a, **k):
        pass

    def init(self, *a, **k):
        pass

    def deinit(self):
        pass

    def irq(self, *a, **k):
        pass

    def readinto(self, buf):
        return 0

    def write(self, buf):
        return len(buf)

    @staticmethod
    def shift(buf, bits, shift):
        pass


class USBDevice:
    BUILTIN_NONE = 0
    BUILTIN_DEFAULT = 1
    BUILTIN_CDC = 1
    XFER_SUCCESS = 0
    XFER_STALLED = 1
    XFER_FAILED = 2

    def active(self, v=None):
        return True

    def config(self, *a, **k):
        pass

    def remote_wakeup(self):
        return False

    def stall(self, *a):
        return False

    def submit_xfer(self, *a):
        return False


class __Mem:
    def __getitem__(self, a):
        return 0

    def __setitem__(self, a, w):
        pass


mem8 = __Mem()
mem16 = __Mem()
mem32 = __Mem()
mem_backup = bytearray(32)


def __pll_moeglich(hz):
    # check_sys_clock_hz aus dem pico-sdk: 12 MHz Quarz, VCO 750-1600 MHz
    if hz % 1000:
        return False
    khz = hz // 1000
    fb = 320
    while fb >= 16:
        vco = 12000 * fb
        if 750000 <= vco <= 1600000:
            a = 7
            while a >= 1:
                b = a
                while b >= 1:
                    if vco // (a * b) == khz and vco % (a * b) == 0:
                        return True
                    b -= 1
                a -= 1
        fb -= 1
    return False


def freq(f=None):
    if f is None:
        return __freq[0]
    if f < 48000000 or not __pll_moeglich(f):
        if not (f in (18000000, 24000000, 30000000, 36000000, 40000000, 42000000, 45000000)):
            raise ValueError('cannot change frequency')
    __freq[0] = f
    __d('takt_mhz', f // 1000000)
    # PicoBoy() ruft als Erstes freq(): die Stelle, an der die Tempobremse
    # in die unveraenderte picoboy.py eingehaengt wird.
    if 'picoboy' in __sys.modules:
        import _pbtakt
        _pbtakt.ein()


def reset():
    __sys.exit()


def soft_reset():
    __sys.exit()


def bootloader(*a):
    __sys.exit()


def deepsleep(ms=0):
    __sys.exit()


def lightsleep(ms=0):
    __time.sleep_ms(ms)


def idle():
    pass


def reset_cause():
    return PWRON_RESET


def unique_id():
    return b'\xe6bD\x98\xcf/`!'


def disable_irq():
    return 0


def enable_irq(state=0):
    pass


def bitstream(pin, encoding, timing, data):
    pass


def dht_readinto(pin, buf):
    __time.sleep_ms(20)
    raise OSError(110)


def time_pulse_us(pin, level, timeout=1000000):
    __time.sleep_us(timeout)
    return -1 if pin.value() == level else -2
