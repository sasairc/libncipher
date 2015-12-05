all: main.c n_cipher.c
	gcc main.c n_cipher.c `pkg-config --cflags --libs glib-2.0` -lm -g -o sample
