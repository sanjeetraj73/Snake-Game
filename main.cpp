#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

using namespace std;

namespace {
constexpr int WIDTH = 40;
constexpr int HEIGHT = 20;
constexpr int POINTS_PER_FOOD = 10;
constexpr int LEVEL_SCORE_STEP = 50;
constexpr int START_DELAY_MS = 140;
constexpr int MIN_DELAY_MS = 55;
const string HIGH_SCORE_FILE = "snake_high_score.txt";

enum class Direction { Up, Down, Left, Right };

struct Position {
    int x{};
    int y{};

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class TerminalMode {
public:
    TerminalMode() {
        if (isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &original_) == 0) {
            termios raw = original_;
            raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
            raw.c_iflag &= static_cast<tcflag_t>(~(IXON | ICRNL));
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
                active_ = true;
            }
        }
        cout << "\033[?25l"; // Hide cursor.
    }

    ~TerminalMode() {
        if (active_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_);
        }
        cout << "\033[?25h\033[0m\n"; // Restore cursor and terminal colors.
    }

    TerminalMode(const TerminalMode&) = delete;
    TerminalMode& operator=(const TerminalMode&) = delete;

private:
    termios original_{};
    bool active_ = false;
};

bool keyAvailable() {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeval timeout{0, 0};
    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout) > 0;
}

int readKey() {
    if (!keyAvailable()) return -1;

    unsigned char ch = 0;
    if (read(STDIN_FILENO, &ch, 1) != 1) return -1;

    // Arrow keys arrive as ESC [ A/B/C/D.
    if (ch == 27) {
        unsigned char seq[2]{};
        if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
            read(STDIN_FILENO, &seq[1], 1) == 1 &&
            seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return 1001; // Up
                case 'B': return 1002; // Down
                case 'C': return 1003; // Right
                case 'D': return 1004; // Left
                default: break;
            }
        }
        return 27;
    }
    return static_cast<int>(ch);
}

int loadHighScore() {
    ifstream file(HIGH_SCORE_FILE);
    int score = 0;
    if (file >> score && score >= 0) return score;
    return 0;
}

void saveHighScore(int score) {
    ofstream file(HIGH_SCORE_FILE);
    if (file) file << score << '\n';
}

Position randomFood(const vector<Position>& snake, mt19937& rng) {
    uniform_int_distribution<int> xDist(1, WIDTH);
    uniform_int_distribution<int> yDist(1, HEIGHT);

    // A valid board has enough free cells; try random positions until one is free.
    for (;;) {
        Position candidate{xDist(rng), yDist(rng)};
        if (find(snake.begin(), snake.end(), candidate) == snake.end()) {
            return candidate;
        }
    }
}

class SnakeGame {
public:
    explicit SnakeGame(int bestScore)
        : highScore_(bestScore), rng_(random_device{}()) {
        reset();
    }

    void run() {
        TerminalMode terminal;
        showInstructions();

        while (!quit_) {
            if (paused_) {
                handlePausedInput();
                continue;
            }

            handleInput();
            if (quit_ || paused_) continue;

            update();
            draw();
            if (gameOver_) {
                showGameOver();
                break;
            }

            this_thread::sleep_for(chrono::milliseconds(speedDelay()));
        }

        if (score_ > highScore_) {
            highScore_ = score_;
            saveHighScore(highScore_);
        }
    }

    int score() const { return score_; }
    int highScore() const { return highScore_; }

private:
    vector<Position> snake_;
    Position food_{};
    Direction direction_ = Direction::Right;
    Direction nextDirection_ = Direction::Right;
    int score_ = 0;
    int highScore_ = 0;
    int foodsEaten_ = 0;
    bool paused_ = false;
    bool gameOver_ = false;
    bool quit_ = false;
    mt19937 rng_;

    void reset() {
        snake_.clear();
        snake_.push_back({WIDTH / 2, HEIGHT / 2});
        snake_.push_back({WIDTH / 2 - 1, HEIGHT / 2});
        snake_.push_back({WIDTH / 2 - 2, HEIGHT / 2});
        direction_ = Direction::Right;
        nextDirection_ = Direction::Right;
        score_ = 0;
        foodsEaten_ = 0;
        paused_ = false;
        gameOver_ = false;
        food_ = randomFood(snake_, rng_);
    }

    void showInstructions() {
        cout << "\033[2J\033[H";
        cout << "============================================\n";
        cout << "             SNAKE GAME - C++\n";
        cout << "============================================\n";
        cout << "Controls: W/A/S/D or arrow keys\n";
        cout << "P = Pause/Resume | Q = Quit\n";
        cout << "Eat food (*) to grow and earn " << POINTS_PER_FOOD << " points.\n";
        cout << "Avoid walls and your own body.\n";
        cout << "High score: " << highScore_ << "\n\n";
        cout << "Press any key to start...";
        cout.flush();
        while (!keyAvailable()) this_thread::sleep_for(chrono::milliseconds(20));
        (void)readKey();
        draw();
    }

    bool isOpposite(Direction a, Direction b) const {
        return (a == Direction::Up && b == Direction::Down) ||
               (a == Direction::Down && b == Direction::Up) ||
               (a == Direction::Left && b == Direction::Right) ||
               (a == Direction::Right && b == Direction::Left);
    }

    void handleInput() {
        while (keyAvailable()) {
            int key = readKey();
            Direction candidate = direction_;

            switch (key) {
                case 'w': case 'W': case 1001: candidate = Direction::Up; break;
                case 's': case 'S': case 1002: candidate = Direction::Down; break;
                case 'a': case 'A': case 1004: candidate = Direction::Left; break;
                case 'd': case 'D': case 1003: candidate = Direction::Right; break;
                case 'p': case 'P': paused_ = true; draw(); return;
                case 'q': case 'Q': quit_ = true; return;
                default: continue;
            }

            // Prevent a 180-degree turn into the snake's own neck.
            if (!isOpposite(candidate, direction_)) {
                nextDirection_ = candidate;
            }
        }
    }

    void handlePausedInput() {
        cout << "\033[" << (HEIGHT + 5) << ";1H";
        cout << "\033[2K  PAUSED - press P to resume or Q to quit";
        cout.flush();

        while (paused_ && !quit_) {
            if (!keyAvailable()) {
                this_thread::sleep_for(chrono::milliseconds(30));
                continue;
            }
            int key = readKey();
            if (key == 'p' || key == 'P') {
                paused_ = false;
                draw();
            } else if (key == 'q' || key == 'Q') {
                quit_ = true;
            }
        }
    }

    void update() {
        direction_ = nextDirection_;
        Position head = snake_.front();

        switch (direction_) {
            case Direction::Up:    --head.y; break;
            case Direction::Down:  ++head.y; break;
            case Direction::Left:  --head.x; break;
            case Direction::Right: ++head.x; break;
        }

        // Wall collision.
        if (head.x < 1 || head.x > WIDTH || head.y < 1 || head.y > HEIGHT) {
            gameOver_ = true;
            return;
        }

        const bool eating = (head == food_);

        // Moving into the current tail cell is legal only when the tail moves away.
        auto bodyEnd = snake_.end();
        if (!eating && !snake_.empty()) --bodyEnd;
        if (find(snake_.begin(), bodyEnd, head) != bodyEnd) {
            gameOver_ = true;
            return;
        }

        snake_.insert(snake_.begin(), head);

        if (eating) {
            score_ += POINTS_PER_FOOD;
            ++foodsEaten_;
            if (score_ > highScore_) {
                highScore_ = score_;
                saveHighScore(highScore_);
            }

            if (static_cast<int>(snake_.size()) == WIDTH * HEIGHT) {
                gameOver_ = true;
                return;
            }
            food_ = randomFood(snake_, rng_);
        } else {
            snake_.pop_back();
        }
    }

    int speedDelay() const {
        const int level = score_ / LEVEL_SCORE_STEP;
        return max(MIN_DELAY_MS, START_DELAY_MS - level * 10);
    }

    void draw() const {
        cout << "\033[2J\033[H";
        cout << "SNAKE GAME  |  Score: " << score_
             << "  |  High score: " << highScore_
             << "  |  Level: " << (score_ / LEVEL_SCORE_STEP + 1) << "\n";
        cout << '+' << string(WIDTH, '-') << "+\n";

        for (int y = 1; y <= HEIGHT; ++y) {
            cout << '|';
            for (int x = 1; x <= WIDTH; ++x) {
                Position p{x, y};
                if (p == snake_.front()) {
                    cout << '@';
                } else if (find(snake_.begin() + 1, snake_.end(), p) != snake_.end()) {
                    cout << 'o';
                } else if (p == food_) {
                    cout << '*';
                } else {
                    cout << ' ';
                }
            }
            cout << "|\n";
        }
        cout << '+' << string(WIDTH, '-') << "+\n";
        cout << "Move: WASD/Arrows | P: Pause | Q: Quit\n";
        cout.flush();
    }

    void showGameOver() const {
        cout << "\033[" << (HEIGHT + 5) << ";1H";
        cout << "\033[2K  GAME OVER! Final score: " << score_
             << " | High score: " << highScore_ << "\n";
        cout << "  Press any key to exit...";
        cout.flush();
        while (!keyAvailable()) this_thread::sleep_for(chrono::milliseconds(20));
        (void)readKey();
    }
};
} // namespace

int main() {
    if (!isatty(STDIN_FILENO)) {
        cerr << "Please run this game in an interactive terminal.\n";
        return 1;
    }

    cout << "\033[?25l";
    int bestScore = loadHighScore();
    bool playAgain = true;

    while (playAgain) {
        SnakeGame game(bestScore);
        game.run();
        bestScore = max(bestScore, game.highScore());

        cout << "\nPlay again? (Y/N): ";
        char answer = 'n';
        if (!(cin >> answer)) break;
        playAgain = (answer == 'y' || answer == 'Y');
    }

    cout << "Thanks for playing Snake Game!\n";
    return 0;
}
