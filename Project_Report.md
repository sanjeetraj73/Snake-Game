# Project Report: Snake Game in C++

**Project Type:** Advanced Console-Based Mini-Project  
**Language:** C++17  
**Platform:** Ubuntu/Linux Terminal

## 1. Abstract

The Snake Game is an interactive console application developed in C++. The player controls a snake that moves around a bounded board and collects food. Each food item increases the score and the snake's length. The game ends if the snake hits a wall or its own body. The project demonstrates fundamental programming concepts alongside modular design and basic terminal input handling.

## 2. Objectives

- Apply loops, arrays/collections, conditions, and functions to a practical game.
- Display a board that updates after each movement.
- Detect food collection, wall collision, and self-collision.
- Track score, levels, and high score.
- Provide pause, quit, game-over, and replay functionality.

## 3. Software and Hardware Requirements

**Software**
- Ubuntu/Linux
- GNU C++ compiler (`g++`) supporting C++17
- Terminal emulator

**Hardware**
- Any computer capable of running Ubuntu/Linux
- Keyboard

## 4. Functional Features

1. **Movement:** The player uses WASD or arrow keys.
2. **Food:** Food appears at a random unoccupied position.
3. **Growth and scoring:** Eating food adds 10 points and extends the snake.
4. **Difficulty:** The movement delay decreases every 50 points, down to a minimum delay.
5. **Collision detection:** The game checks boundaries and the snake's body.
6. **Pause/resume:** Press `P` to pause or continue.
7. **High score:** The best score is saved in `snake_high_score.txt`.
8. **Replay:** After a game ends, the user can start another round.

## 5. Programming Concepts

### Loops
- `while` loops run the game, wait for keyboard input, and handle the paused state.
- `for` loops draw the board row by row and cell by cell.
- A loop generates a food position until an unoccupied cell is found.

### Arrays and collections
The game uses `std::vector<Position>` to represent the snake's body. The head is stored at the beginning of the vector, followed by the body segments. A vector is used instead of a fixed-size C-style array because the snake grows during play.

### Conditional statements
`if`, `else`, and `switch` statements process movement, reject reverse turns, detect collisions, update scores, and select what to draw in each board cell.

### Functions and classes
The `SnakeGame` class groups the game's state and behavior. Helper functions load/save the high score and read keyboard input. This makes the program easier to understand and maintain.

### File handling
`ifstream` reads the saved high score and `ofstream` writes a new high score.

## 6. Algorithm

1. Load the saved high score.
2. Initialize the snake, direction, score, and food.
3. Read keyboard input without waiting for Enter.
4. Update the snake's direction and calculate the next head position.
5. Check wall collision and self-collision.
6. If food is eaten, increase score and snake length, then generate new food.
7. Otherwise, move the head forward and remove the tail.
8. Redraw the board and wait according to the current speed.
9. Continue until collision, quit, or completion.
10. Save the high score and offer replay.

## 7. Compile and Run

Install the compiler:

```bash
sudo apt update
sudo apt install g++
```

Compile:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o snake_game
```

Run:

```bash
./snake_game
```

## 8. Sample Output

The exact score and food position vary because food is placed randomly.

```text
SNAKE GAME | Score: 20 | High score: 40 | Level: 1
+----------------------------------------+
|                                        |
|                                        |
|                 **o@                   |
|                                        |
|                     *                  |
|                                        |
+----------------------------------------+
Move: WASD/Arrows | P: Pause | Q: Quit
```

Legend: `@` = snake head, `o` = snake body, `*` = food. The illustration above is schematic; the actual board has one head and one food item.

Example end-of-game message:

```text
GAME OVER! Final score: 60 | High score: 60
Play again? (Y/N):
```

## 9. Testing Plan

| Test | Action | Expected result |
|---|---|---|
| Start | Launch the executable | Instructions appear |
| Movement | Press W/A/S/D or arrow keys | Snake changes direction |
| Food | Move onto food | Score increases by 10 and snake grows |
| Wall collision | Move into a boundary | Game over |
| Self-collision | Turn into the snake body | Game over |
| Pause | Press P | Movement pauses |
| Resume | Press P again | Movement continues |
| High score | Finish with a new best score | Score is saved to file |
| Replay | Answer Y after a round | A fresh game starts |
| Quit | Press Q | Current round ends |

## 10. Limitations

- This is a terminal-based game, not a graphical game.
- Keyboard handling uses Linux/POSIX terminal APIs, so the source is intended for Ubuntu/Linux rather than native Windows.
- High score is stored locally and is not shared across computers.
- The terminal should support ANSI escape sequences.

## 11. Future Enhancements

- Colorful terminal graphics
- Obstacles and bonus food
- Multiple difficulty modes
- Persistent leaderboard with player names
- Sound effects
- Graphical version using SFML or SDL

## 12. Conclusion

The project demonstrates how core C++ concepts can be combined to build an interactive application. It includes real-time input, dynamic board rendering, collision handling, scoring, file operations, and replay support. It can be extended into a graphical or multi-level game in the future.

**Student Name:** ____________________  
**Roll Number:** ____________________  
**Course / Semester:** ____________________  
**Submitted To:** ____________________
