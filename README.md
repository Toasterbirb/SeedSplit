# SeedSplit
Simple timer with splits, useful for speedrunning

![Screnshot](./screenshot.png)

## Installation (building)
Just run the build.sh script. If you want to compile manually:
```
mkdir build
cmake ..
make -j$(nproc)
cp ../res ./
```

## Adding splits
Split files go to ~/.local/share/SeedSplit

For example: ~/.local/share/SeedSplit/taikurin_hattu.splits

One split per line. Example (check the screenshot for reference):
```
Piisamirotta
Kakun koristelu
Hemulin kukat
Viljonkan talo
Hattivattien saari
Muumipapan muistelmat
Nipsun luola
Tiuhti ja Viuhti
Nuuskamuikkusen teltta
Taikuri
```
