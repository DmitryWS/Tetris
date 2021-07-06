#ifndef TETRIS_HPP
#define TETRIS_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include "TetrisField.hpp"
#include "TetrisFigure.hpp"

class Tetris
{
public:
	Tetris();
	void run();
private:
	sf::RenderWindow _window;

	sf::Texture _backgroundTexture, _tilesTexture, _numbersTexture;
	sf::Sprite _background, _tiles, _numbers;
	sf::Music _tetrisTheme;

	TetrisField _field;
	TetrisFigure _figure;

	std::default_random_engine _randomEngine;
	std::uniform_int_distribution<int> _distribution;

	bool _left, _right, _rotating;
	sf::Time _downTimer, _downDelay, _moveTimer, _moveDelay, _rotateTimer, _rotateDelay, _acceleratedDownDelay;
	unsigned short _nextFigureType;
	unsigned int _linesCount, _score, _level;
	bool _gameOver, _pause;

	void loadResources(std::string path);
	void initialize();
	void handleEvents();
	void update(const sf::Time& elapsed);
	void draw();

	void checkMoving();
	void checkRotating();

	bool moveFigureDown();
	bool newFigure();
	void reset();

	void drawTile(int x, int y, unsigned short color);
	void drawField();
	void drawFigure();
	void drawNextFigure();
	void drawDigit(int x, int y, unsigned short digit);
	void drawNumber(int x, int y, unsigned int number);
};

#endif