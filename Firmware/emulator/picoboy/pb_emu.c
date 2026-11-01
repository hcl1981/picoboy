// Was der PicoBoy-Emulator von C aus braucht: die Bruecke zum Worker
// (Modul _pb), Warten mit Stop und Timern, eine Uhr in Mikrosekunden.
#include <stdint.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/stream.h"
#include "py/gc.h"

extern double pb_js_jetzt_us(void);
extern int pb_js_hook(int n);
extern void pb_js_warte(int ms);
extern int pb_js_ruf(const char *name, int a, int b, int c, int d);

MP_REGISTER_ROOT_POINTER(mp_obj_t pb_tick_cb);

void pb_nach_init(void) {
    MP_STATE_VM(pb_tick_cb) = MP_OBJ_NULL;
}

static void pb_melden(int f) {
    if (f & 1) {
        mp_sched_keyboard_interrupt();
    }
    if ((f & 2) && MP_STATE_VM(pb_tick_cb) != MP_OBJ_NULL) {
        mp_sched_schedule(MP_STATE_VM(pb_tick_cb), mp_const_none);
    }
}

void pb_vm_hook(const void *code) {
    const byte *p = code;
    int im_heap = p >= MP_STATE_MEM(area).gc_pool_start && p < MP_STATE_MEM(area).gc_pool_end;
    pb_melden(pb_js_hook(im_heap ? PB_HOOK_TAKT : 0));
}

void mp_hal_delay_ms(mp_uint_t ms) {
    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        pb_melden(pb_js_hook(0));
        mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS);
        uint32_t um = mp_hal_ticks_ms() - start;
        if (um >= ms) {
            break;
        }
        uint32_t rest = ms - um;
        pb_js_warte(rest > 20 ? 20 : rest);
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    if (us >= 2000) {
        mp_hal_delay_ms(us / 1000);
        return;
    }
    uint32_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
    }
}

mp_uint_t mp_hal_ticks_us(void) {
    return (mp_uint_t)(uint64_t)pb_js_jetzt_us();
}

mp_uint_t mp_hal_ticks_ms(void) {
    return (mp_uint_t)((uint64_t)pb_js_jetzt_us() / 1000);
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return mp_hal_ticks_us();
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    return poll_flags & MP_STREAM_POLL_WR;
}

// Auf dem Geraet der Eingabepuffer der Konsole; hier liest ihn niemand.
#include "py/ringbuf.h"
static uint8_t pb_ein[16];
ringbuf_t stdin_ringbuf = {pb_ein, sizeof(pb_ein), 0, 0};

// Zeitstempel fuer FAT-Dateien
uint32_t get_fattime(void) {
    return ((2026 - 1980) << 25) | (1 << 21) | (1 << 16);
}

extern uint32_t mp_js_random_u32(void);
void mp_hal_get_random(size_t n, void *buf) {
    uint8_t *b = buf;
    for (size_t i = 0; i < n; ++i) {
        b[i] = mp_js_random_u32() & 0xff;
    }
}

// _pb.d(name, a, b, c, d) -> int
static mp_obj_t pb_d(size_t n_args, const mp_obj_t *args) {
    int w[4] = {0, 0, 0, 0};
    for (size_t i = 1; i < n_args; ++i) {
        w[i - 1] = mp_obj_get_int_truncated(args[i]);
    }
    return mp_obj_new_int(pb_js_ruf(mp_obj_str_get_str(args[0]), w[0], w[1], w[2], w[3]));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pb_d_obj, 1, 5, pb_d);

static mp_obj_t pb_set_tick(mp_obj_t f) {
    MP_STATE_VM(pb_tick_cb) = f;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(pb_set_tick_obj, pb_set_tick);

static const mp_rom_map_elem_t pb_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__pb) },
    { MP_ROM_QSTR(MP_QSTR_d), MP_ROM_PTR(&pb_d_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_tick), MP_ROM_PTR(&pb_set_tick_obj) },
};
static MP_DEFINE_CONST_DICT(pb_globals, pb_globals_table);

const mp_obj_module_t pb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pb_globals,
};
MP_REGISTER_MODULE(MP_QSTR__pb, pb_module);
