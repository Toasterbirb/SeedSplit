CC=g++
SRCDIR=./Birb2D/src
outputDir=./build
WarningFlags=-Wpedantic -pedantic -Wall -Wextra
SDL_FLAGS=-lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx
INCLUDES=-I./Birb2D/include

all: seedsplit

seedsplit: seedsplit.o utils.o renderwindow.o timer.o timestep.o values.o entity.o logger.o math.o
	mkdir -p build
	rsync -av ./res ./build/
	$(CC) $^ $(SDL_FLAGS) -lboost_filesystem $(WarningFlags) -o $(outputDir)/SeedSplit

seedsplit.o: ./src/seedsplit.cpp
	$(CC) -c $(INCLUDES) -lboost_filesystem $^ -o seedsplit.o

math.o: $(SRCDIR)/math.cpp
	$(CC) -c $(INCLUDES) $(SDL_FLAGS) $(WarningFlags) $^ -o math.o

logger.o: $(SRCDIR)/logger.cpp
	$(CC) -c $^ -o logger.o

entity.o: $(SRCDIR)/entity.cpp
	$(CC) -c $(INCLUDES) $^ -o entity.o

values.o: $(SRCDIR)/values.cpp
	$(CC) -c $(INCLUDES) $(WarningFlags) $^ -o values.o

timestep.o: $(SRCDIR)/timestep.cpp
	$(CC) -c $(INCLUDES) $(WarningFlags) $^ -o timestep.o

timer.o: $(SRCDIR)/timer.cpp
	$(CC) -c $(INCLUDES) $(WarningFlags) $^ -o timer.o

renderwindow.o: $(SRCDIR)/renderwindow.cpp
	$(CC) -c $(INCLUDES) $(WarningFlags) $^ -o renderwindow.o

utils.o: $(SRCDIR)/utils.cpp
	$(CC) -c $(INCLUDES) $(WarningFlags) $^ -o utils.o

clean:
	rm -rf build
	rm -f *.o
