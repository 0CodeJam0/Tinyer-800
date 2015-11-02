CC = iccavr
CFLAGS =  -ID:\PROGRA~1\icc\include\ -e -DATMEGA -DATMega64  -l -g -Mavr_enhanced 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LD:\PROGRA~1\icc\lib\ -g -ucrtatmega.o -bfunc_lit:0x8c.0x10000 -dram_end:0x10ff -bdata:0x100.0x10ff -dhwstk_size:16 -beeprom:1.2048 -fihx_coff -S2
FILES = main.o 

FK:	$(FILES)
	$(CC) -o FK $(LFLAGS) @FK.lk   -lcatmega
main.o: E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/main.h D:/PROGRA~1/icc/include/stdlib.h D:/PROGRA~1/icc/include/_const.h D:/PROGRA~1/icc/include/limits.h D:/PROGRA~1/icc/include/string.h\
 D:/PROGRA~1/icc/include/_const.h D:/PROGRA~1/icc/include/avr/io.h D:/PROGRA~1/icc/include/avr/pgmspace.h D:/PROGRA~1/icc/include/avr/interrupt.h D:/PROGRA~1/icc/include/avr/eeprom.h D:/PROGRA~1/icc/include/avr/boot.h\
 D:/PROGRA~1/icc/include/avr/wdt.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/old_macros.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/_Settings.h\
 E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/printf_P.h D:/PROGRA~1/icc/include/avr/pgmspace.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/timer0.h\
 E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/uart.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/analog.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/twimaster.h\
 E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/menu.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/rc.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/fc.h\
 E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/gps.h E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong/spi.h
main.o:	E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong\main.c
	$(CC) -c $(CFLAGS) E:\我的文件\四旋翼飞机\helicoperfly\主要参考资料\codes\V068飞控\feikong\main.c
