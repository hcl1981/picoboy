# 1-Wire: auf dem Emulator haengt nichts am Bus.
def reset(pin):
    return False


def readbit(pin):
    return 1


def readbyte(pin):
    return 255


def writebit(pin, value):
    pass


def writebyte(pin, value):
    pass


def crc8(data):
    crc = 0
    for b in data:
        for _ in range(8):
            m = (crc ^ b) & 1
            crc >>= 1
            if m:
                crc ^= 0x8c
            b >>= 1
    return crc
