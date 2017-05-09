#
#    Makefile for libncipher
#

TARGET	= sample
SRCS	= $(wildcard *.c)
OBJS	= $(SRCS:.c=.o) $(wildcard ../src/*.o)

LIBS	= -lm
PKGCFG	= `pkg-config --cflags --libs glib-2.0` 

DEFLDFLAGS = $(LIBS)	\
		$(PKGCFG)

all: $(TARGET)

$(TARGET): $(OBJS)
ifeq ($(CMDLINE), 0)
	@echo "  BUILD   $@"
	@$(CC) $^ -o $@ $(DEFLDFLAGS)
else
	$(CC) $^ -o $@ $(DEFLDFLAGS)
endif

shared-sample: $(OBJS)
ifeq ($(CMDLINE), 0)
	@echo "  BUILD   $@"
	@$(CC) $^ -o $(TARGET) $(DEFLDFLAGS) -lncipher -DWITH_SHARED
else
	$(CC) $^ -o $(TARGET) $(DEFLDFLAGS) -lncipher -DWITH_SHARED
endif

%.o: %.c
ifeq ($(CMDLINE), 0)
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	-$(RM) -f $(TARGET)
	-$(RM) -f $(OBJS)
