#
# MicroPython SH1106 OLED driver, I2C and SPI interfaces
#
# The MIT License (MIT)
#
# Copyright (c) 2016 Radomir Dopieralski (@deshipu),
#               2017-2021 Robert Hammelrath (@robert-hh)
#               2021 Tim Weber (@scy)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

from micropython import const
import utime as time
import framebuf
from machine import Pin, SPI, I2C
import machine

# a few register definitions
_SET_CONTRAST        = const(0x81)
_SET_NORM_INV        = const(0xa6)
_SET_DISP            = const(0xae)
_SET_SCAN_DIR        = const(0xc0)
_SET_SEG_REMAP       = const(0xa0)
_LOW_COLUMN_ADDRESS  = const(0x00)
_HIGH_COLUMN_ADDRESS = const(0x10)
_SET_PAGE_ADDRESS    = const(0xB0)


class pbFrameBuffer(framebuf.FrameBuffer):
    def __init__(self, buffer, width, height, fbFormat, stride=-1):
        self.width = width
        self.height = height
        if stride < width:
            stride = width
        super().__init__(buffer, width, height, fbFormat, stride)

class SH1106(framebuf.FrameBuffer):

    def __init__(self, width, height, external_vcc, rotate=0):
        self.width = width
        self.height = height
        self.external_vcc = external_vcc
        self.flip_en = rotate == 180 or rotate == 270
        self.rotate90 = rotate == 90 or rotate == 270
        self.pages = self.height // 8
        self.bufsize = self.pages * self.width
        self.renderbuf = bytearray(self.bufsize)
        self.pages_to_update = 0

        if self.rotate90:
            self.displaybuf = bytearray(self.bufsize)
            # HMSB is required to keep the bit order in the render buffer
            # compatible with byte-for-byte remapping to the display buffer,
            # which is in VLSB. Else we'd have to copy bit-by-bit!
            super().__init__(self.renderbuf, self.height, self.width,
                             framebuf.MONO_HMSB)
        else:
            self.displaybuf = self.renderbuf
            super().__init__(self.renderbuf, self.width, self.height,
                             framebuf.MONO_VLSB)

        # flip() was called rotate() once, provide backwards compatibility.
        self.rotate = self.flip
        self.init_display()

    def init_display(self):
        self.reset()
        self.fill(0)
        self.poweron()
        # rotate90 requires a call to flip() for setting up.
        self.flip(self.flip_en)

    def poweroff(self):
        self.write_cmd(_SET_DISP | 0x00)

    def poweron(self):
        self.write_cmd(_SET_DISP | 0x01)

    def flip(self, flag=None, update=True):
        if flag is None:
            flag = not self.flip_en
        mir_v = flag ^ self.rotate90
        mir_h = flag
        self.write_cmd(_SET_SEG_REMAP | (0x01 if mir_v else 0x00))
        self.write_cmd(_SET_SCAN_DIR | (0x08 if mir_h else 0x00))
        self.flip_en = flag
        if update:
            self.show(True) # full update

    def sleep(self, value):
        self.write_cmd(_SET_DISP | (not value))

    def contrast(self, contrast):
        self.write_cmd(_SET_CONTRAST)
        self.write_cmd(contrast)

    def invert(self, invert):
        self.write_cmd(_SET_NORM_INV | (invert & 1))

    def show(self, full_update = False):
        # self.* lookups in loops take significant time (~4fps).
        (w, p, db, rb) = (self.width, self.pages,
                          self.displaybuf, self.renderbuf)
        if self.rotate90:
            for i in range(self.bufsize):
                db[w * (i % p) + (i // p)] = rb[i]
        if full_update:
            pages_to_update = (1 << self.pages) - 1
        else:
            pages_to_update = self.pages_to_update
        #print("Updating pages: {:08b}".format(pages_to_update))
        for page in range(self.pages):
            if (pages_to_update & (1 << page)):
                self.write_cmd(_SET_PAGE_ADDRESS | page)
                self.write_cmd(_LOW_COLUMN_ADDRESS | 2)
                self.write_cmd(_HIGH_COLUMN_ADDRESS | 0)
                self.write_data(db[(w*page):(w*page+w)])
        self.pages_to_update = 0

    def pixel(self, x, y, color):
        super().pixel(x, y, color)
        page = y // 8
        self.pages_to_update |= 1 << page

    def text(self, text, x, y, color=1):
        super().text(text, x, y, color)
        self.register_updates(y, y+7)

    def line(self, x0, y0, x1, y1, color):
        super().line(x0, y0, x1, y1, color)
        self.register_updates(y0, y1)

    def hline(self, x, y, w, color):
        super().hline(x, y, w, color)
        self.register_updates(y)

    def vline(self, x, y, h, color):
        super().vline(x, y, h, color)
        self.register_updates(y, y+h)

    def fill(self, color):
        super().fill(color)
        self.pages_to_update = (1 << self.pages) - 1

    def blit(self, fbuf, x, y, key=-1, palette=None):
        super().blit(fbuf, x, y, key, palette)
        self.register_updates(y, y+fbuf.height)

    def scroll(self, x, y):
        # my understanding is that scroll() does a full screen change
        super().scroll(x, y)
        self.pages_to_update =  (1 << self.pages) - 1

    def fill_rect(self, x, y, w, h, color):
        super().fill_rect(x, y, w, h, color)
        self.register_updates(y, y+h)

    def rect(self, x, y, w, h, color):
        super().rect(x, y, w, h, color)
        self.register_updates(y, y+h)

    def register_updates(self, y0, y1=None):
        # this function takes the top and optional bottom address of the changes made
        # and updates the pages_to_change list with any changed pages
        # that are not yet on the list
        start_page = y0 // 8
        end_page = y1 // 8 if y1 is not None else start_page
        # rearrange start_page and end_page if coordinates were given from bottom to top
        if start_page > end_page:
            start_page, end_page = end_page, start_page
        # make sure start_page is positive so we don't end up with negative indices.
        if start_page < 0:
            start_page = 0
        for page in range(start_page, end_page+1):
            self.pages_to_update |= 1 << page

    def reset(self, res):
        if res is not None:
            res(1)
            time.sleep_ms(1)
            res(0)
            time.sleep_ms(20)
            res(1)
            time.sleep_ms(20)

class PicoBoy(SH1106):
    LED_RED = Pin(5, Pin.OUT)#7
    LED_YELLOW = Pin(6, Pin.OUT)#6
    LED_GREEN = Pin(7, Pin.OUT)#5
    JOY_UP = Pin(1, Pin.IN, Pin.PULL_UP)#0
    JOY_DOWN = Pin(3, Pin.IN, Pin.PULL_UP)#3
    JOY_LEFT = Pin(4, Pin.IN, Pin.PULL_UP)#4
    JOY_RIGHT = Pin(2, Pin.IN, Pin.PULL_UP)#2
    JOY_CENTER = Pin(0, Pin.IN, Pin.PULL_UP)#1
    SPEAKER = Pin(15, Pin.OUT)#15
    
    def __init__(self,frequency=50000000):
        global i2c, scl, sda, DEV_ADDR, XOUT1, YOUT1, ZOUT1
        rotate=1
        external_vcc=False
        machine.freq(frequency)
        spi = SPI(0,baudrate=1000000,polarity=1,phase=1,bits=8,firstbit=SPI.MSB,sck=Pin(18),mosi=Pin(19),miso=Pin(16))
        dc = Pin(8)
        res = Pin(9)
        cs = Pin(10)
        self.rate = 10 * 1000 * 1000
        dc.init(dc.OUT, value=0)
        if res is not None:
            res.init(res.OUT, value=0)
        if cs is not None:
            cs.init(cs.OUT, value=1)
        self.spi = spi
        self.dc = dc
        self.res = res
        self.cs = cs
        super().__init__(128, 64, external_vcc, rotate)
        self.rotate(True)

        DEV_ADDR = 0x18    # STK8BA58
        XOUT1  = 0x02
        YOUT1  = 0x04
        ZOUT1  = 0x06
        sda=machine.Pin(20)
        scl=machine.Pin(21) 
        i2c=machine.I2C(0,sda=sda, scl=scl, freq=400000)

    def pressedUp(self):
        return self.JOY_UP.value()==0
            
    def pressedDown(self):
        return self.JOY_DOWN.value()==0
    
    def pressedLeft(self):
        return self.JOY_LEFT.value()==0
    
    def pressedRight(self):
        return self.JOY_RIGHT.value()==0
    
    def pressedCenter(self):
        return self.JOY_CENTER.value()==0
    
    def delay(self,millis):
        time.sleep_ms(millis)

    def write_cmd(self, cmd):
        self.spi.init(baudrate=self.rate, polarity=0, phase=0)
        if self.cs is not None:
            self.cs(1)
            self.dc(0)
            self.cs(0)
            self.spi.write(bytearray([cmd]))
            self.cs(1)
        else:
            self.dc(0)
            self.spi.write(bytearray([cmd]))

    def write_data(self, buf):
        self.spi.init(baudrate=self.rate, polarity=0, phase=0)
        if self.cs is not None:
            self.cs(1)
            self.dc(1)
            self.cs(0)
            self.spi.write(buf)
            self.cs(1)
        else:
            self.dc(1)
            self.spi.write(buf)

    def reset(self):
        super().reset(self.res)   

    def xAcc(self):
        global i2c, scl, sda, DEV_ADDR, XOUT1, YOUT1, ZOUT1
        i2c.writeto(DEV_ADDR, bytes([XOUT1]))
        data = i2c.readfrom(DEV_ADDR, 2)
        value = int(data[0] / 16) + data[1] * 16 
        if value >= 2048:
            value = value-4096
        return value/1024

    def yAcc(self):
        global i2c, scl, sda, DEV_ADDR, XOUT1, YOUT1, ZOUT1
        i2c.writeto(DEV_ADDR, bytes([YOUT1]))
        data = i2c.readfrom(DEV_ADDR, 2)
        value = int(data[0] / 16) + data[1] * 16 
        if value >= 2048:
            value = value-4096
        return value/1024

    def zAcc(self):
        global i2c, scl, sda, DEV_ADDR, XOUT1, YOUT1, ZOUT1
        i2c.writeto(DEV_ADDR, bytes([ZOUT1]))
        data = i2c.readfrom(DEV_ADDR, 2)
        value = int(data[0] / 16) + data[1] * 16 
        if value > 2048:
            value = value-4096
        return value/1024
    