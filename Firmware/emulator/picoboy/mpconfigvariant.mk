JSFLAGS += --js-library variants/picoboy/library_pb.js
JSFLAGS += -s STACK_SIZE=1048576
MICROPY_VFS_LFS2 = 1
SRC_C += variants/picoboy/pb_emu.c
FROZEN_MANIFEST ?= variants/picoboy/manifest.py
# Zeiger, die nur in WASM-Registern stehen, sieht die Speicherbereinigung
# nicht. Dieser Durchlauf legt sie vor jedem Aufruf auf den C-Stapel; so
# reicht es, den Stapel abzusuchen (kein ASYNCIFY, das Python vom Aufrufer
# abkoppeln wuerde).
JSFLAGS += -Os -s ASSERTIONS=0 -s BINARYEN_EXTRA_PASSES=--spill-pointers
MICROPY_VFS_FAT = 1
MICROPY_SSL_MBEDTLS = 1
CFLAGS += -DMICROPY_SSL_MBEDTLS=1 -I$(TOP)/lib/mbedtls/include -I$(VARIANT_DIR)
CFLAGS += -DMBEDTLS_CONFIG_FILE='"pb_mbedtls_config.h"'
SRC_THIRDPARTY_C += $(addprefix lib/mbedtls/library/,md5.c sha1.c sha256.c aes.c platform_util.c)
