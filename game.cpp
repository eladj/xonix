#include "game.h"

Grid::Grid(){
	this->grid.resize(numRows);
	for (int row=0; row < numRows; ++row){
		this->grid.at(row).resize(numCols);
		for (int col=0; col < numCols; ++col){
			this->grid.at(row).at(col) = TileStatus::Empty;
		}
	}
}


int Grid::numFilledTiles() const {
	int sum = 0;
	for (int row=0; row < numRows; ++row){
		for (int col=0; col < numCols; ++col){
			sum += this->grid[row][col] == TileStatus::Filled ? 1 : 0;
		}
	}
	return sum;
}

void Grid::init(){
	// Pre-fill the borders
	for (int row=0; row < numRows; ++row){
		this->grid.at(row).at(0) = TileStatus::Filled;
		this->grid.at(row).at(numCols - 1) = TileStatus::Filled;
	}
	for (int col=0; col < numCols; ++col){
		this->grid.at(0).at(col) = TileStatus::Filled;
		this->grid.at(numRows - 1).at(col) = TileStatus::Filled;
	}
	this->initialFilledArea = this->numFilledTiles();
}


void Grid::set(int row, int col, TileStatus tileStatus){
	// TODO
	// this->grid[row][col] = tileStatus;
	this->grid.at(row).at(col) = tileStatus;
}

void Grid::add(int row, int col, TileStatus tileStatus){
	// TODO
	// this->grid[row][col] = this->grid.at(row).at(col) | tileStatus;
	this->grid.at(row).at(col) = this->grid.at(row).at(col) | tileStatus;
}

void Grid::remove(int row, int col, TileStatus tileStatus){
	// TODO
	// this->grid[row][col] = this->grid.at(row).at(col) | tileStatus;
	this->grid.at(row).at(col) = this->grid.at(row).at(col) & (~tileStatus);
}


TileStatus Grid::get(int row, int col) const{
	// TODO
	// return this->grid[row][col];
	return this->grid.at(row).at(col);
}

bool Grid::has(int row, int col, TileStatus tileStatus) const{
    return (this->grid.at(row).at(col) & tileStatus) == tileStatus;
}

void Grid::changeUniqueStatus(TileStatus currentStatus, TileStatus newStatus){
	for (int row=0; row < numRows; ++row){
		for (int col=0; col < numCols; ++col){
			if (this->grid[row][col] == currentStatus){
				this->grid[row][col] = newStatus;
			}
		}
	}	
}

void Grid::changeStatus(TileStatus currentStatus, TileStatus newStatus){
	for (int row=0; row < numRows; ++row){
		for (int col=0; col < numCols; ++col){
			if (this->has(row, col, currentStatus)){
				this->grid[row][col] = newStatus;
			}
		}
	}	
}


float Grid::areaFilled() const{
	float initTilesToFill = static_cast<float>(numTiles - this->initialFilledArea);
	float tilesAlreadyFilled = static_cast<float>(this->numFilledTiles() - this->initialFilledArea);
	return tilesAlreadyFilled / initTilesToFill;
}


void Game::initVariables(){
	this->window = nullptr;

	// Game logic
	this->life = 3;
	this->areaToWin = 80.0f;
	this->numEnemiesInside = 0;
	this->numEnemiesOutside = 3;
    
	// Standard initialization
	this->leftPressed = false;
	this->rightPressed = false;
	this->upPressed = false;
	this->downPressed = false;
	this->deltaTime = 0.0f;
	this->timer = 0.0f;
}

void Game::initWindow() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	this->videoMode.height = numRows * tileSize + textLineHeight;
	this->videoMode.width = numCols * tileSize;
	this->videoMode.bitsPerPixel = 32;

	this->window = std::make_unique<sf::RenderWindow>(
		this->videoMode,
		"Xonix",
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize,
		// sf::Style::Fullscreen,
		settings);
	this->window->setFramerateLimit(60);
	this->window->setKeyRepeatEnabled(false);
}


void Game::initText(){
	if (!this->font.loadFromFile("resources/AmericanCaptain.ttf")){
		std::cout << "Failed to load font!" << std::endl;
	}	
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(32);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("NONE");
	this->uiText.setPosition(sf::Vector2f(20.0f, numRows * tileSize));
}


void Game::spawnEnemies(){
	// - Clean any enemy from current grid
	// - Place enemies in random empty position, with a random direction
	std::vector<std::tuple<int, int>> empty_ij;
	std::vector<std::tuple<int, int>> filled_ij;
	for (int row=0; row < numRows; ++row){
		for (int col=0; col < numCols; ++col){
			if (this->grid.has(row, col, TileStatus::Empty)){
				empty_ij.push_back(std::make_tuple(row, col));
			}
			if (this->grid.has(row, col, TileStatus::Filled)){
				filled_ij.push_back(std::make_tuple(row, col));
			}			
			if (this->grid.has(row, col, TileStatus::Enemy)){
				this->grid.remove(row, col, TileStatus::Enemy);
			}
		}
	}

	std::uniform_int_distribution<int> randomDistributionDirection(0, 1);
	for (int enemyInd=0; enemyInd < this->numEnemiesOutside; ++enemyInd){
		std::uniform_int_distribution<int> randomDistributionPosition(0, empty_ij.size()-1);
		int r = randomDistributionPosition(this->randomGenerator);
		int x, y;
		std::tie(y, x) = empty_ij.at(r);
		this->enemies.at(enemyInd).x = x;
		this->enemies.at(enemyInd).y = y;

		// Generate random direction
		if (randomDistributionDirection(this->randomGenerator) == 1) this->enemies.at(enemyInd).dx *= -1;
		if (randomDistributionDirection(this->randomGenerator) == 1) this->enemies.at(enemyInd).dy *= -1;		
		empty_ij.erase(empty_ij.begin() + r);  // Not very efficient
	}

	for (int enemyInd=0; enemyInd < this->numEnemiesInside; ++enemyInd){
		std::uniform_int_distribution<int> randomDistributionPosition(0, filled_ij.size()-1);
		int r = randomDistributionPosition(this->randomGenerator);
		int x, y;
		std::tie(y, x) = filled_ij.at(r);
		this->enemies.at(this->numEnemiesOutside + enemyInd).x = x;
		this->enemies.at(this->numEnemiesOutside + enemyInd).y = y;

		// Generate random direction
		if (randomDistributionDirection(this->randomGenerator) == 1) this->enemies.at(enemyInd).dx *= -1;
		if (randomDistributionDirection(this->randomGenerator) == 1) this->enemies.at(enemyInd).dy *= -1;		
		filled_ij.erase(filled_ij.begin() + r);  // Not very efficient
	}	
}

void Game::initEnemies(){
	std::uniform_real_distribution<float> randomDistribution(0.0f, 1.0f);
	this->enemies.clear();

	// Add enemies in the unfilled area
	for (int i=0; i < this->numEnemiesOutside; ++i){
		Enemy enemy;
		enemy.type = EnemyType::Empty;
		this->enemies.push_back(enemy);
	}

	// Add enemies in the filled area
	for (int i=0; i < this->numEnemiesInside; ++i){
		Enemy enemy;
		enemy.type = EnemyType::Filled;
		this->enemies.push_back(enemy);
	}	

	spawnEnemies();
}

Game::Game(){
    std::random_device rd;
	this->randomGenerator.seed(rd());

	this->gameState = GameState::PLAY;

	this->initVariables();
	this->initWindow();
	this->grid.init();
	this->initText();
	this->initEnemies();
}

const bool Game::running() const {
	return this->window->isOpen();
}

void Game::pollEvents(){
	while (this->window->pollEvent(this->event)){
		switch (this->event.type){
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			switch (this->event.key.code){
				case sf::Keyboard::Escape:
					this->window->close();
					break;
				case sf::Keyboard::Left:
					leftPressed = true;
					break;
				case sf::Keyboard::Right:
					rightPressed = true;
					break;
				case sf::Keyboard::Up:
					upPressed = true;
					break;
				case sf::Keyboard::Down:
					downPressed = true;
					break;					
				default:
					break;
			}
			
			break;
		case sf::Event::KeyReleased:
			switch (this->event.key.code){
				case sf::Keyboard::Left:
					leftPressed = false;
					break;
				case sf::Keyboard::Right:
					rightPressed = false;
					break;
				case sf::Keyboard::Up:
					upPressed = false;
					break;
				case sf::Keyboard::Down:
					downPressed = false;
					break;
				default:
					break;
			}	
			break;
		default:
			break;			
		}
	}
}

void Game::lose(){
	this->life--;
	for (auto& enemy : this->enemies){
		this->grid.remove(enemy.y, enemy.x, TileStatus::Enemy);
	}
	this->initEnemies();

	this->grid.remove(player.y, player.x, TileStatus::Player);
	player.x = 0;
	player.y = 0;
}


void Game::tryFill(int row, int col){
	bool foundEnemy = false;
	this->fillArea(row, col, foundEnemy);	
	if (foundEnemy) {
		this->grid.changeStatus(TileStatus::MaybeToFill, TileStatus::Empty);
	} else {
		this->grid.changeStatus(TileStatus::MaybeToFill, TileStatus::Filled);
	}
}


void Game::fillArea(int row, int col, bool& foundEnemy){
	if (row < 0 or row >= numRows or col < 0 or col >= numCols) return;

	if (this->grid.has(row, col, TileStatus::Enemy)){
		foundEnemy = true;
		return;
	}

	if (this->grid.get(row, col) == TileStatus::Filled or 
		this->grid.get(row, col) == TileStatus::NewFilled or 
		this->grid.get(row, col) == TileStatus::MaybeToFill){
		return;
	}

	if (this->grid.get(row, col) == TileStatus::Empty){
		this->grid.set(row, col, TileStatus::MaybeToFill);
		fillArea(row, col+1, foundEnemy);
		fillArea(row, col-1, foundEnemy);
		fillArea(row+1, col, foundEnemy);
		fillArea(row-1, col, foundEnemy);
	}
}


void Game::updatePlayer(){
	int dx=0;
	int dy=0;
	if (leftPressed) dx=-1;
	else if (rightPressed) dx=1;
	else if (upPressed) dy=-1;
	else if (downPressed) dy=1;
	else return;  // If nothing was pressed we don't need to update anything

	// Remove previous player position
	int oldX=player.x;
	int oldY=player.y;
	this->grid.remove(player.y, player.x, TileStatus::Player);

	// Update player position in the grid
	this->player.x += dx;
	this->player.y += dy;

	// Avoid moving outside the grid
	if (this->player.x < 0) this->player.x = 0;
	if (this->player.x > numCols - 1) this->player.x = numCols - 1;
	if (this->player.y < 0) this->player.y = 0;
	if (this->player.y > numRows - 1) this->player.y = numRows - 1;

	// Update area and player status
	if (this->grid.get(player.y, player.x) == TileStatus::Empty){
		this->grid.set(player.y, player.x, TileStatus::NewFilled);
	} else if (this->grid.get(player.y, player.x) == TileStatus::Filled and this->grid.get(oldY, oldX) == TileStatus::NewFilled) {
		// We reached back to a filled area
		// Try to fill area if it doesn't have enemy inside
		this->tryFill(oldY-1, oldX);
		this->tryFill(oldY+1, oldX);
		this->tryFill(oldY, oldX+1);
		this->tryFill(oldY, oldX-1);
		
		// Switch the NewFilled into Filled
		this->grid.changeStatus(TileStatus::NewFilled, TileStatus::Filled);
	} else if (this->grid.get(player.y, player.x) == TileStatus::NewFilled) {
		// Inner loop, we hit our own trace
		this->grid.changeStatus(TileStatus::NewFilled, TileStatus::Empty);
		lose();
	}
	this->grid.add(player.y, player.x, TileStatus::Player);
}


void Game::updateEnemies(){
	for (auto& enemy : this->enemies){
		// Remove old enemy position
		this->grid.remove(enemy.y, enemy.x, TileStatus::Enemy);

		// Update player position in the grid
		// Flip direction if hitting a filled area
		enemy.x += enemy.dx;
		if (this->grid.get(enemy.y, enemy.x) == TileStatus::Filled){
			enemy.dx = -enemy.dx;
			enemy.x += enemy.dx;
		}

		enemy.y += enemy.dy;
		if (this->grid.get(enemy.y, enemy.x) == TileStatus::Filled){
			enemy.dy = -enemy.dy;
			enemy.y += enemy.dy;
		}

		// Check if enemy hit the player or new filled tile
		TileStatus tmp = this->grid.get(enemy.y, enemy.x);
		if (((tmp & TileStatus::Player) == TileStatus::Player) or ((tmp & TileStatus::NewFilled) == TileStatus::NewFilled)){
			this->grid.changeStatus(TileStatus::NewFilled, TileStatus::Empty);
			lose();
		}

		// Add new position
		this->grid.add(enemy.y, enemy.x, TileStatus::Enemy);
	}
}

void Game::updateText(){
	float fps = 1.0 / this->deltaTime;
	
	std::stringstream ss;
	ss.precision(1);
	ss << "Area: " << std::fixed << this->grid.areaFilled()*100.0f << "%" << " | "
		<< "Life: " << this->life << " | "
		<< "FPS: " << std::fixed << fps <<" \n";

	this->uiText.setString(ss.str());
}

void Game::update() {
	this->deltaTime = this->clock.getElapsedTime().asSeconds();
	this->timer += this->deltaTime;
	this->clock.restart();

	this->pollEvents();

	if (this->gameState == GameState::PLAY)
	{
		if (this->timer > moveDelay){
			this->updatePlayer();
			this->updateEnemies();
			this->timer = 0.0f;
		}
		this->updateText();
	}

	// End game condition
	if (this->life <= 0){
		this->gameState = GameState::END;
		this->window->close();
	}
}

void Game::renderText(){
	this->window->draw(this->uiText);
}

void Game::renderGrid(){
	for (int row=0; row < numRows; ++row){
		for (int col=0; col < numCols; ++col){
			sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
			tile.setPosition(sf::Vector2f(col*tileSize, row*tileSize));
			sf::Color color;
			if (this->grid.has(row, col, TileStatus::Player)){
				color = sf::Color::Green;
			} else if (this->grid.has(row, col, TileStatus::Enemy)){
				color = sf::Color::Red;
			} else if (this->grid.has(row, col, TileStatus::Empty)){
				color = sf::Color::Black;
			} else if (this->grid.has(row, col, TileStatus::Filled)){
				color = sf::Color::White;
			} else if (this->grid.has(row, col, TileStatus::NewFilled)){
				color = sf::Color::Cyan;
			} else {
				// Default
				color = sf::Color::Yellow;
			}

			tile.setFillColor(color);
			this->window->draw(tile);
		}
	}
}


void Game::render() {
	// Clear old frame
	this->window->clear();

	// Draw game objects
	this->renderText();
	this->renderGrid();

	// Display frame in window
	this->window->display();
}