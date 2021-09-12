#include "game.h"

int main(){
	// Init Game engine
	Game game;

	//Game loop
	while (game.running() && !game.getEndGame()){
		//Update
		game.update();

		//Render
		game.render();
	}

	return 0;
}