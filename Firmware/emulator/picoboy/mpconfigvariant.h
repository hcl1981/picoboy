// PicoBoy-Emulator: MicroPython fuer den Browser, eingestellt wie die
// Firmware des PicoBoy (ports/rp2, Board PICOBOY).
#define MICROPY_PICOBOY_EMU                     (1)

// Wie auf dem Geraet: ein einziger Heap fester Groesse mit echter
// Speicherbereinigung. Die pyscript-Variante vergroessert stattdessen
// den Heap und raeumt nie auf, solange Python laeuft.
#define MICROPY_GC_SPLIT_HEAP                   (0)
#define MICROPY_GC_SPLIT_HEAP_AUTO              (0)

#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_SYS_PLATFORM                 "rp2"
#define MICROPY_HW_BOARD_NAME                   "PicoBoy"
#define MICROPY_HW_MCU_NAME                     "RP2040"
#define MICROPY_VFS_POSIX                       (0)
#define MICROPY_PY_JS                           (0)
#define MICROPY_PY_JSFFI                        (0)
#define MICROPY_PY_ASYNCIO_TASK_QUEUE_PUSH_CALLBACK (0)
#define MICROPY_ENABLE_SCHEDULER                (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE    (0)
#define MICROPY_PY_OS_UNAME                     (1)
#define MICROPY_PY_OS_URANDOM                   (1)
#define MICROPY_PY_OS_SYNC                      (1)
#define MICROPY_PY_OS_DUPTERM                   (1)
#define MICROPY_PY_OS_DUPTERM_NOTIFY            (1)
#define MICROPY_PY_HASHLIB_MD5                  (1)
#define MICROPY_PY_HASHLIB_SHA1                 (1)
#define MICROPY_PY_CRYPTOLIB                    (1)
#define MICROPY_PY_CRYPTOLIB_CTR                (1)
#undef MICROPY_BOARD_BUILD_NAME
#define MICROPY_BOARD_BUILD_NAME                "PICOBOY"
#define MICROPY_PY_TIME_INCLUDEFILE             "ports/webassembly/variants/picoboy/modtime.c"
#define MICROPY_PY_FRAMEBUF                     (1)
#define CSTACK_SIZE                             (11200)
#define MICROPY_FATFS_ENABLE_LFN                (1)
#define MICROPY_FATFS_LFN_CODE_PAGE             437
#define MICROPY_FATFS_RPATH                     (2)
#define MICROPY_FATFS_MAX_SS                    (4096)
#define MICROPY_CAN_OVERRIDE_BUILTINS           (1)

// Der Interpreter meldet sich alle PB_HOOK_TAKT Rueckspruenge und
// Funktionsenden. Daran haengen Stop, Timer und die Tempobremse. Gebremst
// wird nur Programmcode aus dem Heap: eingefrorene Module sind hier in
// Python geschriebene Hardware, deren Rechenzeit das Geraet nicht hat.
#define PB_HOOK_TAKT (16)
#define MICROPY_VM_HOOK_INIT static uint vm_hook_divisor = PB_HOOK_TAKT;
#define MICROPY_VM_HOOK_POLL if (--vm_hook_divisor == 0) { \
        vm_hook_divisor = PB_HOOK_TAKT; \
        extern void pb_vm_hook(const void *code); \
        pb_vm_hook(code_state->fun_bc->bytecode); \
}
#define MICROPY_VM_HOOK_LOOP MICROPY_VM_HOOK_POLL
#define MICROPY_VM_HOOK_RETURN MICROPY_VM_HOOK_POLL

extern void pb_nach_init(void);
#include <stddef.h>
#include <stdint.h>
extern void mp_hal_get_random(size_t n, void *buf);
