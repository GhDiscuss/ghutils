CFLAGS += -flto
CFLAGS += -O3

LDFLAGS += -lcurl
LDFLAGS += -flto

AR_OPT += rcs $@ $^
CFLAGS += -Iinclude -I.

lib_src := $(wildcard lib/*.c)
lib_obj := $(lib_src:.c=.o)

discuss_src := $(wildcard discuss/*.c)
discuss_obj := $(discuss_src:.c=.o)

ghdiscuss: ${discuss_src} libghutils.a
	${CC} -Wall -Wextra -Wno-unused ${CFLAGS} ${LDFLAGS} -o $@ $^

libghutils.a: ${lib_obj}
	${AR} ${AR_OPT}

.PHONY: clean clean-lib clean-discuss
clean-lib:
	rm -f libghutils.a ${lib_obj}

clean-discuss:
	rm -f ${discuss_obj} ghdiscuss

clean: clean-lib clean-discuss

install:
	install ghdiscuss /usr/local/bin
