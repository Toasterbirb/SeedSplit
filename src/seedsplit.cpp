#include <iostream>
#include <unistd.h>
#include <vector>
#include <fstream>
#include "Timer.hpp"
#include "Logger.hpp"
#include "Values.hpp"
#include "Entity.hpp"
#include "Timestep.hpp"
#include "Renderwindow.hpp"

/* Boost */
#include <boost/filesystem.hpp>

bool GameRunning = true;

struct Split
{
	std::string Name;
	std::string DigitalTime;
	double mills;
};

std::string datapath;
std::vector<Split> splits;

int main(int argc, char **argv)
{
	/* Check if there were enough args */
	if (argc < 2)
		return 1;

	/* Get data path */
	datapath = (std::string)getenv("HOME") + "/.local/share/SeedSplit";
	std::cout << "Data path: " << datapath << std::endl;
	if (boost::filesystem::exists(datapath))
	{
		std::cout << "Existing datadir found" << std::endl;
	}
	else
	{
		std::cout << "Creating a new datadir" << std::endl;
		boost::filesystem::create_directories(datapath);
	}


	/* Check for selected splitfile */
	std::string splitFilePath = datapath + "/" + argv[1] + ".splits";
	std::cout << "Selected splits: " << argv[1] << std::endl;
	if (boost::filesystem::exists(splitFilePath))
	{
		std::cout << "Splits were found!" << std::endl;

		/* Load splits */
		std::fstream splitFile;
		splitFile.open(splitFilePath, std::ios::in);

		/* Check if file was opened successfully and read the data */
		std::string line;
		if (splitFile.is_open())
		{
			while (std::getline(splitFile, line))
			{
				Split newSplit;
				newSplit.Name = line;
				newSplit.DigitalTime = "00:00";
				newSplit.mills = -1;
				splits.push_back(newSplit);
			}
		}
	}
	else
	{
		std::cout << "Could not find splits" << std::endl;
		return 1;
	}

	/* Print out all loaded splits */
	std::cout << "Loaded splits:" << std::endl;
	for (int i = 0; i < splits.size(); i++)
		std::cout << splits[i].Name << std::endl;

	Birb2D::Window window("SeedSplit", Vector2int(400, 512), 240);
	Birb2D::TimeStep timeStep;

	/* Initialize timestep */
	timeStep.Init();

	/* Resource variables */
	TTF_Font* mainFont = Birb2D::Resources::LoadFont("./res/fonts/manaspace/manaspc.ttf", 32);
	TTF_Font* splitFont = Birb2D::Resources::LoadFont("./res/fonts/manaspace/manaspc.ttf", 20);

	/* Gameloop variables */
	SDL_Event event;
	Birb2D::Entity e_totalTime("Total time", Vector2int(10, 10), Birb2D::TextComponent("00:00:000", mainFont, &Colors::White));
	Birb2D::Timer timer;
	int currentSplit = 0;

	/* Initialize split entities */
	std::vector<Birb2D::Entity> splitNameEntities;
	std::vector<Birb2D::Entity> splitTimeEntities;
	for (int i = 0; i < splits.size(); i++)
	{
		splitNameEntities.push_back(Birb2D::Entity("Split name", Vector2int(10, 50 + (i * 24)), Birb2D::TextComponent(splits[i].Name, splitFont, &Colors::White)));
		splitTimeEntities.push_back(Birb2D::Entity("Split time", Vector2int(window.window_dimensions.x - 80, 50 + (i * 24)), Birb2D::TextComponent(splits[i].DigitalTime, splitFont, &Colors::White)));
	}

	while (GameRunning)
	{

		timeStep.Start();
		while (timeStep.Running())
		{
			while (SDL_PollEvent(&event) != 0)
			{
				window.EventTick(event, &GameRunning);
				
				/* Handle keypresses */
				if (event.type == SDL_KEYDOWN)
				{
					/* Start timer */
					if (!timer.running && event.key.keysym.scancode == 44)
					{
						/* Reset splits */
						currentSplit = 0;
						for (int i = 0; i < splits.size(); i++)
							splitTimeEntities[i].SetText("00:00");

						timer.Start();
					}

					/* Next split */
					else if (timer.running && event.key.keysym.scancode == 44)
					{
						/* Save split data */
						splits[currentSplit].mills = timer.ElapsedMilliseconds();
						if (currentSplit == 0)
							splits[currentSplit].DigitalTime = timer.SplitDigitalFormat(0);
						else
							splits[currentSplit].DigitalTime = timer.SplitDigitalFormat(splits[currentSplit - 1].mills);

						/* Update split time entity */
						splitTimeEntities[currentSplit].SetText(splits[currentSplit].DigitalTime);

						currentSplit++;

						if (currentSplit >= splits.size())
							timer.Stop();
					}
				}
			}

			timeStep.Step();
		}

		timeStep.End();

		/* Update the main timer text */
		e_totalTime.SetText(timer.DigitalFormat());

		/* Render stuff */
		window.Clear();
		{
			/* Draw the timer */
			Birb2D::Render::DrawEntity(e_totalTime);

			/* Draw the divider line between main timer and splits */
			Birb2D::Render::DrawRect(Colors::White, Rect(10, 40, window.window_dimensions.x - 20, 2));

			/* Draw splits */
			for (int i = 0; i < splits.size(); i++)
			{
				Birb2D::Render::DrawEntity(splitNameEntities[i]);
				Birb2D::Render::DrawEntity(splitTimeEntities[i]);
			}
		}
		window.Display();
		/* End of rendering */
	}
	timer.Stop();

	Debug::Log("Starting cleanup...");
	window.Cleanup();
	SDL_Quit();
	Debug::Log("SeedSplit should be closed now!");
	return 0;
}
