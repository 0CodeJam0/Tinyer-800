CC = iccavr
CFLAGS =  -ID:\PROGRA~1\icc\include\ -e -DATMEGA -DATMega64  -l -g -Mavr_enhanced 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LD:\PROGRA~1\icc\lib\ -g -ucrtatmega.o -bfunc_lit:0x8c.0x10000 -dram_end:0x10ff -bdata:0x100.0x10ff -dhwstk_size:16 -beeprom:1.2048 -fihx_coff -S2
FILES = main.o 

FK:	$(FILES)
	$(CC) -o FK $(LFLAGS) @FK.lk   -lcatmega
main.o: E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/main.h D:/PROGRA~1/icc/include/stdlib.h D:/PROGRA~1/icc/include/_const.h D:/PROGRA~1/icc/include/limits.h D:/PROGRA~1/icc/include/string.h\
 D:/PROGRA~1/icc/include/_const.h D:/PROGRA~1/icc/include/avr/io.h D:/PROGRA~1/icc/include/avr/pgmspace.h D:/PROGRA~1/icc/include/avr/interrupt.h D:/PROGRA~1/icc/include/avr/eeprom.h D:/PROGRA~1/icc/include/avr/boot.h\
 D:/PROGRA~1/icc/include/avr/wdt.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/old_macros.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/_Settings.h\
 E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/printf_P.h D:/PROGRA~1/icc/include/avr/pgmspace.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/timer0.h\
 E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/uart.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/analog.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/twimaster.h\
 E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/menu.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/rc.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/fc.h\
 E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/gps.h E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong/spi.h
main.o:	E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong\main.c
	$(CC) -c $(CFLAGS) E:\�ҵ��ļ�\������ɻ�\helicoperfly\��Ҫ�ο�����\codes\V068�ɿ�\feikong\main.c
