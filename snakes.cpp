#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <cstdlib>
#include <windows.h>
#include <ctime>
#include <conio.h>

using namespace std;

typedef vector<vector<int>> vvi;
typedef vector<int> vi;

void gotoxy(short x, short y) {
    COORD c = { y, x };  
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

#define MAP_FILE "map.txt"

#define WALL "#";
#define SNAKE "?"
#define EMPTY " "
#define FRUIT "@"

#define TAIL 3

struct Coordinate {
    int x;
    int y;
    Coordinate(int x = -1, int y = -1) {
        this->x = x;
        this->y = y;
    }
};

enum Direction {
    NONE = -1,
    UP = 8,
    DOWN = 2,
    LEFT = 4,
    RIGHT = 6
};

class Map {
private:
    int height;
    int width;
    vvi grid;
    /*
        0 - walls
        1 - empty
        2 - fruit
        3 - tail
    */
    void print() {
        gotoxy(0, 0);
        for(int r = 0; r < height; r++) {
            for(int c = 0; c < width; c++) {
                switch(grid[r][c]) {
                    case 0:
                        cout << WALL;
                        break;
                    case 1:
                        cout << EMPTY;
                        break;
                    default:
                        cout << SNAKE;
                        break;
                }
            }
            cout << "\n";
        }
    }
    void gen_fruit() {
        int r, c;
        do {
            r = rand() % height;
            c = rand() % width;
        } while(grid[r][c] != 1);
        grid[r][c] = 2;
        gotoxy(r, c);
        cout << FRUIT;
    }
public:
    Map(const string mapf_name) {
        ifstream map_file;
        map_file.open(mapf_name);
        if(!map_file) {
            cout << "404: map file <" << mapf_name << "> could not be accessed!\n";
            exit(-1);
        }
        map_file >> height >> width;
        grid.resize(height, vi(width, 1));
        for(int r = 0; r < height; r++) {
            // boundary 
            for(int c = 0; c < width; c++) {
                if(r == 0 || r == height - 1 || c == 0 || c == width - 1) {
                    grid[r][c] = 0;
                }
            }
        }
        // extra obstacles
        while(!map_file.eof()) {
            int ulx, uly;
            int lrx, lry;
            map_file >> ulx >> uly >> lrx >> lry;
            for(int r = ulx; r < lrx; r++) {
                for(int c = uly; c < lry; c++) {
                    grid[r][c] = 0;
                }
            }
        }
        map_file.close();
    }
    friend class Game;
    friend class Snake;
};

class Snake {
private:
    Coordinate head;
    Direction dir;
    bool alive;
    int points;
    int len;
    int move(Map &map) {
        if(dir == NONE) {
            return 0;
        }
        switch(dir) {
            case UP:
                head.x -= 1;
                break;
            case LEFT:
                head.y -= 1;
                break;
            case DOWN:
                head.x += 1;
                break;
            case RIGHT: 
                head.y += 1;
                break;
        }
        int head_val = map.grid[head.x][head.y];
        if(head_val == 0 || (head_val >= TAIL && head_val < TAIL + len - 1)) {
            alive = false;
            return -1;
        }
        if(head_val == 2) {
            ++len;
            map.gen_fruit();
            points += 5;
            // gotoxy(map.height)
        }
        else {
            for(int r = 0; r < map.height; r++) {
                for(int c = 0; c < map.width; c++) {
                    if(map.grid[r][c] == TAIL) {
                        map.grid[r][c] = 1;
                        gotoxy(r, c);
                        cout << EMPTY;
                    }
                    else if(map.grid[r][c] > TAIL) {
                        --map.grid[r][c];
                    }
                }
            }
        }
        map.grid[head.x][head.y] = TAIL + (len - 1);
        gotoxy(head.x, head.y);
        cout << "?";
    }
    void change_dir(char key) {
        switch(key) {
            case 'w':
                if(dir != DOWN) {
                    dir = UP;
                }
                break;
            case 'a':
                if(dir != RIGHT) {
                    dir = LEFT;
                }
                break;
            case 's':
                if(dir != UP && dir != NONE) {
                    dir = DOWN;
                }
                break;
            case 'd':
                if(dir != LEFT) {
                    dir = RIGHT;
                }
                break;
        }
    }
public:
    Snake() {
        dir = NONE;
        len = 2;
        alive = true;
        points = 0;
    }
    friend class Game;
};

class Game {
private:
    Map map;
    Snake snake;
    int lag;
public:
    Game() 
    : map(MAP_FILE) {
        lag = 100;
        // init snake
        int sx, sy;
        do {
            sx = rand() % map.height;
            sy = rand() % map.width;
        } while(map.grid[sx][sy] != 1 || map.grid[sx + 1][sy] != 1);
        snake.head.x = sx;
        snake.head.y = sy;
        map.grid[sx][sy] = TAIL + 1;
        map.grid[sx + 1][sy] = TAIL;
        // printing map
        map.print();
        cout << "POINTS:\n";
        map.gen_fruit();
    }
    void run() {
        while(snake.alive) {
            Sleep(lag);
            if(kbhit()) {
                snake.change_dir(getch());
            }
            snake.move(map);
            gotoxy(map.height, 10);
            cout << snake.points;
        }
        // system("pause");        
    }
};

int main() {
    srand(time(NULL));
    Game game;
    game.run();
    return 0;
}