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

clean:
	rm -rf build
	rm -f *.o
