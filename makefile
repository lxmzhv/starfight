NAME=starfight

CC=g++
#DEBUG=-g -pg
DEBUG=-O2
CFLAGS=`pkg-config --cflags gtk+-2.0` -Iinclude -Iinclude_lcore $(DEBUG)
LFLAGS=`pkg-config --libs gtk+-2.0` #-pg

CPP_FILES=cpp/* cpp_lcore/*
H_FILES=include/* include_lcore/*
SRC_FILES=$(CPP_FILES) $(H_FILES)
OBJ_FILES=obj/ui.o obj/ai.o obj/utils.o obj/draw.o obj/game.o

all: $(NAME) tags messages

rebuild: clean all

run: all
	./$(NAME)

messages: locale/ru/LC_MESSAGES/$(NAME).mo locale/en/LC_MESSAGES/$(NAME).mo
	
locale/ru/LC_MESSAGES/$(NAME).mo: po/ru.po
	msgfmt -o $@ po/ru.po

locale/en/LC_MESSAGES/$(NAME).mo: po/en.po
	msgfmt -o $@ po/en.po

new_messages: cpp/ui.cpp
	xgettext -C -k_ -o po/id.po cpp/*
	msgmerge -U po/ru.po po/id.po
	msgmerge -U po/en.po po/id.po
	gvim -d "po/ru.po~" po/ru.po 

tags: $(SRC_FILES)
	ctags --c-kinds=+px -f .tags $(SRC_FILES)
	mkvimsyntax .

tags-legacy: $(SRC_FILES)
	ctags --defines --typedefs --typedefs-and-c++ -f .tags $(SRC_FILES)

$(NAME): $(OBJ_FILES)
	$(CC) obj/*.o $(LFLAGS) -o $(NAME)

obj/ui.o:	cpp/ui.cpp $(H_FILES)
	$(CC) -c $(CFLAGS) -o $@ cpp/ui.cpp

obj/ai.o:	cpp/ai.cpp $(H_FILES)
	$(CC) -c $(CFLAGS) -o $@ cpp/ai.cpp

obj/draw.o:	cpp/draw.cpp $(H_FILES)
	$(CC) -c $(CFLAGS) -o $@ cpp/draw.cpp

obj/game.o:	cpp/game.cpp $(H_FILES)
	$(CC) -c $(CFLAGS) -o $@ cpp/game.cpp

obj/utils.o:	cpp/utils.cpp $(H_FILES)
	$(CC) -c $(CFLAGS) -o $@ cpp/utils.cpp

#	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(NAME) obj/*.o .tags .syntax.vim
