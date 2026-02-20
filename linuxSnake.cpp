#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <random>
#include <deque>
using namespace std;

enum Direction {
    Up,
    Down,
    Right,
    Left
};

char getcharr() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(c != EOF)
        return (char)c;
    return 0;
}


void spawnApple(int &x, int &y, const vector<string>& map, const deque<pair<int,int>>& body, mt19937& gen) {
    int minX = 1;
    int maxX = map[0].size() - 2;
    int minY = 1;
    int maxY = map.size() -2;

    uniform_int_distribution<int> distX(minX, maxX);
    uniform_int_distribution<int> distY(minY, maxY);

    while(true)
    {
        int ax = distX(gen);
        int ay = distY(gen);

        bool collides = false;
        for (auto &seg : body)
        {
            if(seg.first == ax && seg.second == ay)
            {
                collides = true;
                break;
            }
        }
        if(!collides)
        {
            x = ax;
            y = ay;
            return;
        }
    }
}

void updateMap(const vector<string>& mapToPrint){
    system("clear");
    for(const auto& row : mapToPrint){
        cout << row << '\n';
    }
}

int main(){
    mt19937 gen(std::random_device{}());
    vector<string> map = {
        "|-------------------------|",
        "|                         |",
        "|                         |",
        "|                         |",
        "|                         |",
        "|                         |",
        "|                         |",
        "|                         |",
        "|                         |",
        "|                         |",
        "|-------------------------|"
    };
    char headChar = '@';
    char tailChar = '#';
    char appleChar = 'c';
    int snakeLength = 1;

    int playerX = 1;
    int playerY = 1;

    int appleX;
    int appleY;
    
    Direction currentDirection = Right;

    deque<pair<int,int>> body;
    body.push_front({playerX,playerY});

    spawnApple(appleX, appleY, map, body, gen);
    map[appleY][appleX] = appleChar;

    updateMap(map);

    while(true)
    {
        char key = getcharr();
        int nextX = playerX;
        int nextY = playerY;

        if(key == 'w' && currentDirection != Down) currentDirection = Up;
        else if(key == 'd' && currentDirection != Left) currentDirection = Right;
        else if(key == 's' && currentDirection != Up) currentDirection = Down;
        else if(key == 'a' && currentDirection != Right) currentDirection = Left;
        else if(key == 'q') break;

        
        switch (currentDirection)
        {
            case Up:
                nextY--;
                break;
            case Down:
                nextY++;
                break;
            case Right:
                nextX++;
                break;
            case Left:
                nextX--;
                break;
            default:
                nextX++;
                break;
        }

        if(map[nextY][nextX] == '-' || map[nextY][nextX] == '|') return 0;

        for(auto &segment : body) {
            if(segment.first == nextX && segment.second == nextY)
                return 0;
        }

        if(map[nextY][nextX] == appleChar){
            if(body.size() >= (map[0].size() - 2)*(map.size() - 2)){
                for(int i = 0; i < 20; i++) cout << "You won!!" << endl;
                return 0;
            }
            map[nextY][nextX] = ' ';
            spawnApple(appleX, appleY, map, body, gen);
            snakeLength++;
        }

        playerX = nextX;
        playerY = nextY;

        body.push_front({playerX, playerY});
        
        if(body.size() > snakeLength) {
            pair<int, int> tail = body.back();
            map[tail.second][tail.first] = ' ';
            body.pop_back();
        }

        for(int y = 1; y < map.size() - 1; y++) {
            for(int x = 1; x < map[y].size() - 1; x++){
                map[y][x] = ' '; //If you dont understand whats happening.
                //Dont worry. Because i didnt add comments earlier, idk how this code even works
            }
        }
        
        bool first = true;
        for(auto &segment : body) {
            map[segment.second][segment.first] = first ? headChar :tailChar;
            first = false;
        }
        map[appleY][appleX] = appleChar;
        updateMap(map);
        this_thread::sleep_for(chrono::milliseconds(250));
    }
    return 0;
    //Before you call me an idiot. Remember that we all start somewhere btw
}