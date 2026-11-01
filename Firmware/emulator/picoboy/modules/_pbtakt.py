# Tempo des echten PicoBoy: picoboy.py aus der Firmware bleibt unveraendert,
# ihre Zeichenbefehle bekommen beim ersten PicoBoy() die am Geraet gemessene
# Dauer umgehaengt (messung/, Stand Firmware mit einmaligem spi.init).
# takt(a, b): a us laufen mit der CPU, b us sind fester SPI-Anteil.
import _pb, picoboy

__d = _pb.d
__fertig = [False]


def ein():
    if __fertig[0]:
        return
    __fertig[0] = True
    S = picoboy.SH1106
    a_show, a_fill, a_pixel, a_text, a_line = S.show, S.fill, S.pixel, S.text, S.line
    a_hline, a_vline, a_rect, a_fill_rect = S.hline, S.vline, S.rect, S.fill_rect
    a_blit, a_scroll = S.blit, S.scroll

    def show(self, full_update=False):
        if full_update:
            n = self.pages
        else:
            n = 0
            m = self.pages_to_update
            while m:
                n += m & 1
                m >>= 1
        __d('anzeige_start')
        a_show(self, full_update)
        __d('anzeige_ende')
        __d('takt', K_SHOW + K_SEITE * n, 170 + 129 * n)

    def fill(self, c):
        a_fill(self, c)
        if c:
            __d('gezeichnet')
        __d('takt', K_FILL)

    def pixel(self, x, y, color=-1):
        if color == -1:
            return a_pixel(self, x, y)
        a_pixel(self, x, y, color)
        __d('gezeichnet')
        __d('takt', K_PIXEL)

    def text(self, t, x, y, color=1):
        a_text(self, t, x, y, color)
        __d('gezeichnet')
        __d('takt', K_TEXT + 41 * len(str(t)))

    def line(self, x0, y0, x1, y1, c):
        a_line(self, x0, y0, x1, y1, c)
        __d('gezeichnet')
        __d('takt', K_LINE + 3 * max(abs(x1 - x0), abs(y1 - y0)))

    def hline(self, x, y, w, c):
        a_hline(self, x, y, w, c)
        __d('gezeichnet')
        __d('takt', K_HLINE + abs(w) // 4)

    def vline(self, x, y, h, c):
        a_vline(self, x, y, h, c)
        __d('gezeichnet')
        __d('takt', K_VLINE + 3 * abs(h))

    def rect(self, x, y, w, h, c):
        a_rect(self, x, y, w, h, c)
        __d('gezeichnet')
        __d('takt', K_RECT + 2 * (abs(w) + abs(h)))

    def fill_rect(self, x, y, w, h, c):
        a_fill_rect(self, x, y, w, h, c)
        __d('gezeichnet')
        __d('takt', K_FILLR + abs(w * h) * 35 // 100)

    def blit(self, fbuf, x, y, key=-1, palette=None):
        a_blit(self, fbuf, x, y, key, palette)
        __d('gezeichnet')
        __d('takt', K_FILLR + fbuf.width * fbuf.height * 35 // 100)

    def scroll(self, x, y):
        a_scroll(self, x, y)
        __d('gezeichnet')
        __d('takt', K_FILL)

    def ellipse(self, x, y, xr, yr, c, f=False, m=15):
        super(S, self).ellipse(x, y, xr, yr, c, f, m)
        __d('gezeichnet')
        __d('takt', K_ELLIPSE + 14 * max(abs(xr), abs(yr)))

    def poly(self, x, y, coords, c, f=False):
        super(S, self).poly(x, y, coords, c, f)
        __d('gezeichnet')
        __d('takt', K_ELLIPSE + 14 * len(coords))

    S.show, S.fill, S.pixel, S.text, S.line = show, fill, pixel, text, line
    S.hline, S.vline, S.rect, S.fill_rect = hline, vline, rect, fill_rect
    S.blit, S.scroll, S.ellipse, S.poly = blit, scroll, ellipse, poly


K_SHOW = 264
K_SEITE = 1434
K_FILL = 2338
K_PIXEL = 178
K_TEXT = 422
K_LINE = 498
K_HLINE = 411
K_VLINE = 532
K_RECT = 418
K_FILLR = 417
K_ELLIPSE = 159
