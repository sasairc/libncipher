all: sample.c n_cipher.c
	gcc sample.c n_cipher.c `pkg-config --cflags --libs glib-2.0` -lm -g -o sample

clean:
	-$(RM) -f sample
