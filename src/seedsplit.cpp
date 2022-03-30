#include <iostream>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <birb2d/Timer.hpp>
#include <birb2d/Logger.hpp>
#include <birb2d/Values.hpp>
#include <birb2d/Entity.hpp>
#include <birb2d/Timestep.hpp>
#include <birb2d/Renderwindow.hpp>
#include <birb2d/Filesystem.hpp>

/* External libraries */
#include "inipp.h"

bool GameRunning = true;

struct Split
{
	std::string Name;
	std::string DigitalTime;
	double mills;
};

std::string datapath;
std::vector<Split> splits;
bool splitsEnabled;

/* Configurations */
Birb::ApplicationInfo appInfo("SeedSplit");
int timerSize = 32;
int splitSize = 20;
int splitSpacing = 0;
Birb::Color currentSplitColor = Birb::Colors::Yellow;

int main(int argc, char **argv)
{
	std::string fontPath = appInfo.ResLocation + "/fonts/manaspace/manaspc.ttf";
	datapath = (std::string)getenv("HOME") + "/.config/SeedSplit";

	/* Read the configuration file */
	inipp::Ini<char> ini;
	std::ifstream configfile(datapath + "/config.ini");
	ini.parse(configfile);
	std::cout << "Config:" << std::endl;
	ini.interpolate();
	ini.generate(std::cout);
	inipp::get_value(ini.sections["Theme"], "Font", fontPath);
	inipp::get_value(ini.sections["Theme"], "TimerSize", timerSize);
	inipp::get_value(ini.sections["Theme"], "SplitSize", splitSize);
	inipp::get_value(ini.sections["Theme"], "SplitSpacing", splitSpacing);

	{
		int colorR = -1;
		int colorG = -1;
		int colorB = -1;
		inipp::get_value(ini.sections["Theme"], "CurrentSplitR", colorR);
		inipp::get_value(ini.sections["Theme"], "CurrentSplitG", colorG);
		inipp::get_value(ini.sections["Theme"], "CurrentSplitB", colorB);

		if (colorR != -1 || colorG != -1 || colorB != -1)
		{
			if (colorR == -1)
				colorR = 0;

			if (colorG == -1)
				colorG = 0;

			if (colorB == -1)
				colorB = 0;

			currentSplitColor.r = colorR;
			currentSplitColor.g = colorG;
			currentSplitColor.b = colorB;
		}
	}

	std::cout << "Split r: " << std::to_string(currentSplitColor.r) << std::endl;
	std::cout << "Split g: " << std::to_string(currentSplitColor.g) << std::endl;
	std::cout << "Split b: " << std::to_string(currentSplitColor.b) << std::endl;

	/* Check if there were enough args */
	if (argc < 2)
	{
		splitsEnabled = false;
	}
	else
	{
		/* Load split stuff */
		splitsEnabled = true;

		/* Get data path */
		std::cout << "Data path: " << datapath << std::endl;
		if (Birb::Filesystem::Directory::Exists(datapath))
		{
			std::cout << "Existing datadir found" << std::endl;
		}
		else
		{
			std::cout << "Creating a new datadir" << std::endl;
			Birb::Filesystem::Directory::Create(datapath);
		}

		/* Check for selected splitfile */
		std::string splitFilePath = datapath + "/" + argv[1] + ".splits";
		std::cout << "Selected splits: " << argv[1] << std::endl;
		if (Birb::Filesystem::File::Exists(splitFilePath))
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
	}

	Birb::Window window("SeedSplit", Birb::Vector2int(400, 512), 240, true);
	Birb::TimeStep timeStep;

	/* Initialize timestep */
	timeStep.Init(&window);

	/* Resource variables */
	TTF_Font* mainFont = Birb::Resources::LoadFont(fontPath, timerSize);
	TTF_Font* splitFont = Birb::Resources::LoadFont(fontPath, splitSize);

	/* Gameloop variables */
	SDL_Event event;
	Birb::Entity e_totalTime("Total time", Birb::Vector2int(10, 10), Birb::EntityComponent::Text("00:00:000", mainFont, &Birb::Colors::White));
	Birb::Timer timer;
	int currentSplit = 0;

	/* Initialize split entities */
	std::vector<Birb::Entity> splitNameEntities;
	std::vector<Birb::Entity> splitTimeEntities;

	if (splitsEnabled)
	{
		for (int i = 0; i < splits.size(); i++)
		{
			splitNameEntities.push_back(Birb::Entity("Split name", Birb::Vector2int(10, splitSize + timerSize + (i * (splitSize + splitSpacing))), Birb::EntityComponent::Text(splits[i].Name, splitFont, &Birb::Colors::White)));
			splitTimeEntities.push_back(Birb::Entity("Split time", Birb::Vector2int(window.window_dimensions.x - 60 - splitSize, splitSize + timerSize + (i * (splitSize + splitSpacing))), Birb::EntityComponent::Text(splits[i].DigitalTime, splitFont, &Birb::Colors::White)));
		}
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
						if (splitsEnabled)
						{
							currentSplit = 0;
							for (int i = 0; i < splits.size(); i++)
								splitTimeEntities[i].SetText("00:00");

							/* Set color for the first split as selected */
							splitNameEntities[0].SetColor(&currentSplitColor); /* Reset color */
							splitTimeEntities[0].SetColor(&currentSplitColor); /* Reset color */
						}

						timer.Start();
					}

					/* Next split */
					else if (timer.running && event.key.keysym.scancode == 44)
					{
						/* Save split data */
						if (splitsEnabled)
						{
							splits[currentSplit].mills = timer.ElapsedMilliseconds();
							if (currentSplit == 0)
								splits[currentSplit].DigitalTime = timer.SplitDigitalFormat(0);
							else
								splits[currentSplit].DigitalTime = timer.SplitDigitalFormat(splits[currentSplit - 1].mills);

							/* Update split time entity */
							splitTimeEntities[currentSplit].SetText(splits[currentSplit].DigitalTime);

							splitNameEntities[currentSplit].SetColor(&Birb::Colors::White); /* Reset color for split name */
							splitTimeEntities[currentSplit].SetColor(&Birb::Colors::White); /* Reset color for split time */

							currentSplit++;

							if (currentSplit < splits.size())
							{
								splitNameEntities[currentSplit].SetColor(&currentSplitColor); /* Change color for new selected split name */
								splitTimeEntities[currentSplit].SetColor(&currentSplitColor); /* Change color for new selected split time */
							}
						}

						if (!splitsEnabled || currentSplit >= splits.size())
							timer.Stop();
					}
				}
			}

			timeStep.Step();
		}

		timeStep.End();

		/* Update the main timer text when if the elapsed milliseconds is divisible by 5
		 * this is to slow down the texture spamming a bit. Update the text though if the timer 
		 * has been stopped */
		if ((int)timer.ElapsedMilliseconds() % 2 == 0 || !timer.running)
		{
			e_totalTime.SetText(timer.DigitalFormat());
		}

		/* Update split time positions relative to window dimensions */
		if (splitsEnabled)
		{
			for (int i = 0; i < splits.size(); i++)
			{
				splitTimeEntities[i].rect.x = window.window_dimensions.x - 60 - splitSize;
			}

			/* Update split times */
			if (timer.running && currentSplit < splits.size())
			{
				splits[currentSplit].DigitalTime = timer.SplitDigitalFormat(splits[currentSplit - 1].mills);
				splitTimeEntities[currentSplit].SetText(splits[currentSplit].DigitalTime);
			}
		}

		/* Render stuff */
		window.Clear();
		{
			/* Draw the timer */
			Birb::Render::DrawEntity(e_totalTime);

			if (splitsEnabled)
			{
				/* Draw the divider line between main timer and splits */
				Birb::Render::DrawRect(Birb::Colors::White, Birb::Rect(10, 12 + timerSize, window.window_dimensions.x - 20, 2));

				/* Draw splits */
				for (int i = 0; i < splits.size(); i++)
				{
					Birb::Render::DrawEntity(splitNameEntities[i]);
					Birb::Render::DrawEntity(splitTimeEntities[i]);
				}
			}
		}
		window.Display();
		/* End of rendering */
	}
	timer.Stop();

	Birb::Debug::Log("Starting cleanup...");
	window.Cleanup();
	Birb::Debug::Log("SeedSplit should be closed now!");
	return 0;
}
