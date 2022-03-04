CC=g++
outputDir=./build
WarningFlags=-Wpedantic -pedantic -Wall -Wextra
SDL_FLAGS=-lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx

all: seedsplit

seedsplit: seedsplit.o
	mkdir -p build
	rsync -av ./res ./build/
	$(CC) $^ -lbirb2d $(SDL_FLAGS) $(WarningFlags) -o $(outputDir)/SeedSplit

run_seedsplit: seedsplit
	cd build ; ./SeedSplit taikurin_hattu

seedsplit.o: ./src/seedsplit.cpp
	$(CC) -O3 -c -I./include $^ -o seedsplit.o

install:
	cp -f ./build/SeedSplit /usr/local/bin/SeedSplit
	mkdir -p /usr/local/share/SeedSplit
	cp -rf ./build/res /usr/local/share/SeedSplit/

uninstall:
	rm -f /usr/local/bin/SeedSplit
	rm -rf /usr/local/share/SeedSplit

clean:
	rm -rf build
	rm -f *.o
