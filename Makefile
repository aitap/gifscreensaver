CC = i686-w64-mingw32-gcc
CFLAGS = -Istb

%.o: %.gif
	i686-w64-mingw32-objcopy -I binary -O pe-i386 -B i386 $^ $@

saver.scr: saver.o scrnsave.o image.o
	$(CC) $(LDFLAGS) -o $@ $^ -lscrnsave -lgdi32 -mwindows
