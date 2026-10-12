# Snake Game — Advanced C++ Mini-Project

A real-time console Snake game for Ubuntu/Linux, written in C++17. It uses only the C++ standard library and POSIX terminal APIs; no third-party game library is required.

## Features

- Real-time movement using **W/A/S/D** or arrow keys
- Food collection and snake growth
- Score, level, and persistent high score
- Speed increases every 50 points
- Wall and self-collision detection
- Pause/resume (`P`) and quit (`Q`)
- Replay prompt after the game ends
- Modular design using a class, helper functions, `vector`, loops, and conditionals

## Requirements

- Ubuntu or another Linux distribution with a terminal
- `g++` with C++17 support

## Install compiler

```bash
sudo apt update
sudo apt install g++
```

## Compile and run

From the project folder:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o snake_game
./snake_game
```

Run it in a real interactive terminal (not by piping input). Terminal raw mode is used to read keys without pressing Enter.

## Controls

| Key | Action |
|---|---|
| W / ↑ | Move up |
| S / ↓ | Move down |
| A / ← | Move left |
| D / → | Move right |
| P | Pause or resume |
| Q | Quit the current game |

## Files

- `main.cpp` — complete source code
- `Project_Report.md` — ready-to-edit project report
- `Makefile` — convenience build targets
- `snake_high_score.txt` — created automatically when a score is saved

## Notes

The high-score file is stored in the current working directory. Run the game from the same folder if you want to reuse the same high score. The terminal cursor and input settings are restored when the game exits normally.
