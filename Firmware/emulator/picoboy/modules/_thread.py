# Der zweite Kern des RP2040 fehlt dem Emulator: im Browser laeuft genau
# ein Python. Sperren funktionieren, ein neuer Thread nicht.
class LockType:
    def __init__(self):
        self._zu = False

    def acquire(self, waitflag=1, timeout=-1):
        if self._zu and not waitflag:
            return False
        self._zu = True
        return True

    def release(self):
        self._zu = False

    def locked(self):
        return self._zu

    def __enter__(self):
        self.acquire()
        return self

    def __exit__(self, *a):
        self.release()


def allocate_lock():
    return LockType()


def get_ident():
    return 1


def stack_size(n=0):
    return 4096


def exit():
    raise SystemExit


def start_new_thread(f, a, k=None):
    raise OSError('core1 fehlt im Emulator')
