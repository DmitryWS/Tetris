#include "Tetris.hpp"

Tetris::Tetris() : _distribution(1, 7)
{
	std::srand(std::time(nullptr));
	loadResources("resources");

	_moveDelay = sf::milliseconds(100); // CONFIG
	_rotateDelay = sf::milliseconds(400); // CONFIG
	_acceleratedDownDelay = sf::milliseconds(25); // CONFIG
}

void Tetris::run()
{
	initialize();

	sf::Clock clock;
	while (_window.isOpen())
	{
		handleEvents();
		update(clock.restart());
		draw();
	}
}



void Tetris::loadResources(std::string path)
{
	_backgroundTexture.loadFromFile(path + "/textures/background.png");
	_tilesTexture.loadFromFile(path + "/textures/tiles.png");
	_numbersTexture.loadFromFile(path + "/textures/numbers.png");

	_background.setTexture(_backgroundTexture);
	_tiles.setTexture(_tilesTexture);
	_numbers.setTexture(_numbersTexture);

	_tetrisTheme.openFromFile(path + "/music/tetrisTheme.ogg");
	_tetrisTheme.setVolume(5); // CONFIG
	_tetrisTheme.setLoop(true);
}

void Tetris::initialize()
{
	_window.create(sf::VideoMode(800, 640), "Tetris");
	reset();
}

void Tetris::handleEvents()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			_window.close();
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::A) // CONFIG
			{
				if (!_left)
					_moveTimer = _moveDelay;
				_left = true;
			}
			if (event.key.code == sf::Keyboard::D) // CONFIG
			{
				if (!_right)
					_moveTimer = _moveDelay;
				_right = true;
			}
			if (event.key.code == sf::Keyboard::W) // CONFIG
			{
				if (!_rotating)
					_rotateTimer = _rotateDelay;
				_rotating = true;
			}
			if (event.key.code == sf::Keyboard::S) // CONFIG
				_downDelay = _acceleratedDownDelay;
			if (event.key.code == sf::Keyboard::Escape) // CONFIG
				_pause = !_pause;
			if (event.key.code == sf::Keyboard::Enter) // CONFIG
				reset();
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == sf::Keyboard::A) // CONFIG
				_left = false;
			if (event.key.code == sf::Keyboard::D) // CONFIG
				_right = false;
			if (event.key.code == sf::Keyboard::W) // CONFIG
				_rotating = false;
			if (event.key.code == sf::Keyboard::S) // CONFIG
				_downDelay = sf::milliseconds(400 - 50 * _level); // CONFIG
			break;
		}
	}
}

void Tetris::update(const sf::Time& elapsed)
{
	if (!_gameOver && !_pause)
	{
		_moveTimer += elapsed;
		if (_moveTimer > _moveDelay)
		{
			checkMoving();
			_moveTimer = sf::Time::Zero;
		}

		_rotateTimer += elapsed;
		if (_rotateTimer > _rotateDelay)
		{
			checkRotating();
			_rotateTimer = sf::Time::Zero;
		}
		
		_downTimer += elapsed;
		if (_downTimer > _downDelay)
		{
			if (!moveFigureDown())
				_gameOver = true;
			_downTimer = sf::Time::Zero;
		}
	}

}

void Tetris::draw()
{
	_window.clear();
	_window.draw(_background);
	drawField();
	drawFigure();
	drawNumber(16, 2, _level);
	drawNumber(16, 6, _linesCount);
	drawNumber(16, 10, _score);
	drawNextFigure();
	_window.display();
}



void Tetris::checkMoving()
{
	int moveDirection = (_left ? -1 : 0) + (_right ? 1 : 0);
	if (moveDirection != 0)
	{
		TetrisFigure ghostFigure(_figure);
		ghostFigure.move(moveDirection);
		if (!_field.checkIntersection(ghostFigure))
			_figure.move(moveDirection);
	}
}

void Tetris::checkRotating()
{
	if (_rotating)
	{
		TetrisFigure ghostFigure(_figure);
		ghostFigure.rotate();
		if (!_field.checkIntersection(ghostFigure))
			_figure.rotate();
	}
}



bool Tetris::moveFigureDown()
{
	TetrisFigure ghostFigure(_figure);
	ghostFigure.down();
	if (_field.checkIntersection(ghostFigure))
	{
		_field.drawFigure(_figure);
		int lines = _field.checkLines();
		_linesCount += lines;
		_score += 100 + 50 * lines * (lines + 1);
		_level = _score / 10000;
		if (_level > 9)
			_level = 9;
		if (!newFigure())
			return false;
	}
	else
		_figure.down();
	return true;
}

bool Tetris::newFigure()
{
	_figure.initFigure(_nextFigureType);
	_nextFigureType = _distribution(_randomEngine);
	return !_field.checkIntersection(_figure);
}

void Tetris::reset()
{
	_nextFigureType = 1 + rand() % 7;
	newFigure();
	_linesCount = 0;
	_score = 0;
	_level = 0;
	_gameOver = false;
	_pause = false;

	_left = false;
	_right = false;
	_rotating = false;
	_downDelay = sf::milliseconds(400); // CONFIG
	_field.clear();

	_tetrisTheme.play();
}



void Tetris::drawTile(int x, int y, unsigned short color)
{
	if (color < 1 || color > 7)
		return;
	_tiles.setTextureRect(sf::IntRect(32 * color - 32, 0, 32, 32));
	_tiles.setPosition(32 * x, 32 * y);
	_window.draw(_tiles);
}

void Tetris::drawField()
{
	for (int x = 0; x < _field.getSize().x; x++)
		for (int y = 0; y < _field.getSize().y; y++)
			drawTile(2 + x, y, _field.getTile(sf::Vector2i(x, y)) - 1);
}

void Tetris::drawFigure()
{
	int figureType = _figure.getType();
	for (int i = 0; i < 4; i++)
	{
		sf::Vector2i tile = _figure.getTile(i);
		drawTile(2 + tile.x, tile.y, (figureType + 1) /* CONFIG */ - 1);
	}
}

void Tetris::drawNextFigure()
{
	int allFigures[7][4] = {
		1, 3, 5, 7, // I
		1, 3, 4, 5, // J
		0, 2, 4, 5, // L
		0, 1, 2, 3, // O
		0, 2, 3, 5, // S
		0, 2, 3, 4, // T
		1, 2, 3, 4, // Z
	};
	for (int i = 0; i < 4; i++)
	{
		int x = 16 + allFigures[_nextFigureType - 1][i] / 2;
		int y = 16 - allFigures[_nextFigureType - 1][i] % 2;
		drawTile(x, y, _nextFigureType);
	}
}

void Tetris::drawDigit(int x, int y, unsigned short digit)
{
	if (digit >= 10)
		return;
	_numbers.setTextureRect(sf::IntRect(32 * digit, 0, 32, 32));
	_numbers.setPosition(32 * x, 32 * y);
	_window.draw(_numbers);
}

void Tetris::drawNumber(int x, int y, unsigned int number)
{
	unsigned int n = number;
	if (n > 999999)
		n = 999999;
	if (n == 0)
		drawDigit(x + 5, y, 0);
	else
	{
		int k = 5;
		while (n != 0)
		{
			drawDigit(x + k, y, n % 10);
			n /= 10;
			k--;
		}
	}
}