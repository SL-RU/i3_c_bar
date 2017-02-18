include ./skb/config.mk

INC=-I. -I./skb
SRC = ./skb/skb.c main.c helpers.c
SRC_B=skb.c main.c helpers.c
OBJ = ${SRC_B:.c=.o}

all: options bar

options:
	@echo skb build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo "LD       = ${LD}"

.c.o:
	@echo "${CC} $(INC) -c ${CFLAGS}"
	@echo CC $<
	@${CC} $(INC) -c ${CFLAGS} $<

bar: ${OBJ}
	@echo LD bar
	@${LD} -o bar ${OBJ} ${LDFLAGS}
	@strip bar

clean: 
	@echo cleaning
	@rm -f bar ${OBJ}

.PHONY: all options clean
