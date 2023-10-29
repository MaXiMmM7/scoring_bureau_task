#include "PingPong.h"
#include <iostream>


int main(int argc, char* argv[]){
	
	Game::PingPongHandler h;

	if(argc == 2){
		h.start(std::string(argv[1]));
	}else{
		std::cerr << "Wrong input." << std::endl;
	}

	return 0;
}
