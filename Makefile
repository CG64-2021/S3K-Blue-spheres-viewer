OBJS=main.c m_utils.c
CC=gcc

ICFLAGS=-ISDL2-2.26.5/i686-w64-mingw32/include/SDL2
ICFLAGS+=-ISDL2_mixer-2.6.3/i686-w64-mingw32/include/SDL2
ICFLAGS+=-ISDL2_image-2.6.3/i686-w64-mingw32/include/SDL2
LDFLAGS=-LSDL2-2.26.5/i686-w64-mingw32/lib
LDFLAGS+=-LSDL2_mixer-2.6.3/i686-w64-mingw32/lib
LDFLAGS+=-LSDL2_image-2.6.3/i686-w64-mingw32/lib
LIBS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image

OBJ_NAME=S3KBlueSpheres

all:$(OBJS)
	$(CC) $(OBJS) $(ICFLAGS) $(LDFLAGS) $(LIBS) -o $(OBJ_NAME)