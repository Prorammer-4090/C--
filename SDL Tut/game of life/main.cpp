#include "screen.h"

const int GAME_WIDTH = 640;
const int GAME_HEIGHT = 480;

bool isAlive(std::array<std::array<int, GAME_HEIGHT>, GAME_WIDTH>& game,
            const int x, 
            const int y){

}

int main(){

    std::array<std::array<int, GAME_HEIGHT>, GAME_WIDTH> display {};
    std::array<std::array<int, GAME_HEIGHT>, GAME_WIDTH> swap {};

    // Create random points
    for (auto& row : display){
        std::generate(row.begin(), row.end(), []() {return rand() % 10 == 0 ? 1 : 0;});
    }

    while(true){

    }
}