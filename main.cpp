#include <SFML/Graphics.hpp>

typedef sf::Uint32 UInt;

struct TetrisConfig
{
	sf::Keyboard::Key moveLeftKey, moveRightKey, rotateKey, acceleratedDownKey, pauseKey;
	sf::Time delays[10], acceleratedDelay;
};

class TetrisFigure
{
public:
	TetrisFigure();
	TetrisFigure(const TetrisFigure& figure);
	void initFigure(UInt type);
	sf::Vector2i getTile(UInt ind) const;
	UInt getType() const;

	void move(int dx);
	void down();
	void rotate();
private:
	sf::Vector2i _figure[4];
	UInt _type;
};

class TetrisField
{
public:
	TetrisField(UInt w = 10, UInt h = 20);
	~TetrisField();

	sf::Vector2u getSize() const;
	UInt getTile(int x, int y) const;

	bool checkIntersection(const TetrisFigure& figure) const;
	UInt checkLines();
	bool isLine(UInt y) const;
	void moveDown(UInt y);
	void drawFigure(const TetrisFigure& figure);
	void clear();
private:
	UInt** _field;
	sf::Vector2u _size;
};

class Tetris
{
public:
	Tetris(const TetrisConfig& config);
	void run();
private:
	TetrisConfig _config;

	sf::RenderWindow _window;

	sf::Texture _backgroundTexture, _tilesTexture, _numbersTexture;
	sf::Sprite _background, _tiles, _numbers;

	TetrisField _field;
	TetrisFigure _figure;

	int _moveDirection;
	bool _rotating;
	sf::Time _timer, _delay;
	UInt _nextFigureType, _linesCount, _score, _level;
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

	void drawTile(int x, int y, UInt color);
	void drawField();
	void drawFigure();
	void drawNextFigure();
	void drawDigit(int x, int y, UInt digit);
	void drawNumber(int x, int y, UInt number);
};

int main()
{
	TetrisConfig config;

	config.moveLeftKey = sf::Keyboard::A;
	config.moveRightKey = sf::Keyboard::D;
	config.rotateKey = sf::Keyboard::W;
	config.acceleratedDownKey = sf::Keyboard::S;
	config.pauseKey = sf::Keyboard::Escape;

	config.delays[0] = sf::milliseconds(400);
	config.delays[1] = sf::milliseconds(450);
	config.delays[2] = sf::milliseconds(400);
	config.delays[3] = sf::milliseconds(350);
	config.delays[4] = sf::milliseconds(300);
	config.delays[5] = sf::milliseconds(250);
	config.delays[6] = sf::milliseconds(200);
	config.delays[7] = sf::milliseconds(150);
	config.delays[8] = sf::milliseconds(100);
	config.delays[9] = sf::milliseconds(50);
	config.acceleratedDelay = sf::milliseconds(25);

	Tetris(config).run();
}

// ------------< Tetris >------------

Tetris::Tetris(const TetrisConfig& config): _config(config)
{
	std::srand(std::time(nullptr));
	loadResources("resources");
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
	_backgroundTexture.loadFromFile(path + "/background.png");
	_tilesTexture.loadFromFile(path + "/tiles.png");
	_numbersTexture.loadFromFile(path + "/numbers.png");

	_background.setTexture(_backgroundTexture);
	_tiles.setTexture(_tilesTexture);
	_numbers.setTexture(_numbersTexture);
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
			if (event.key.code == _config.moveLeftKey)
				_moveDirection = -1;
			if (event.key.code == _config.moveRightKey)
				_moveDirection = 1;
			if (event.key.code == _config.rotateKey)
				_rotating = true;
			if (event.key.code == _config.acceleratedDownKey)
				_delay = _config.acceleratedDelay;
			if (event.key.code == _config.pauseKey)
				_pause = !_pause;
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == _config.acceleratedDownKey)
				_delay = _config.delays[_level];
			break;
		}
	}
}

void Tetris::update(const sf::Time& elapsed)
{
	if (!_gameOver && !_pause)
	{
		checkMoving();
		checkRotating();
		_timer += elapsed;
		if (_timer > _delay)
		{
			if (!moveFigureDown())
				_gameOver = true;
			_timer = sf::Time::Zero;
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
	if (_moveDirection != 0)
	{
		TetrisFigure ghostFigure(_figure);
		ghostFigure.move(_moveDirection);
		if (!_field.checkIntersection(ghostFigure))
			_figure.move(_moveDirection);
	}
	_moveDirection = 0;
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
	_rotating = false;
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
		_level = _score / 100000;
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
	_nextFigureType = 1 + rand() % 7;
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

	_moveDirection = 0;
	_rotating = false;
	_delay = _config.delays[0];
	_field.clear();
}



void Tetris::drawTile(int x, int y, UInt color)
{
	if (color < 1 || color > 7)
		return;
	_tiles.setTextureRect(sf::IntRect(32 * color - 32, 0, 32, 32));
	_tiles.setPosition(32 * x, 32 * y);
	_window.draw(_tiles);
}

void Tetris::drawField()
{
	for (UInt x = 0; x < _field.getSize().x; x++)
		for (UInt y = 0; y < _field.getSize().y; y++)
			drawTile(2 + x, y, _field.getTile(x, y));
}

void Tetris::drawFigure()
{
	UInt figureType = _figure.getType();
	for (UInt i = 0; i < 4; i++)
	{
		sf::Vector2i tile = _figure.getTile(i);
		drawTile(2 + tile.x, tile.y, figureType);
	}
}

void Tetris::drawNextFigure()
{
	int allFigures[7][4] = {
		0, 2, 3, 5,
		0, 2, 3, 4,
		1, 2, 3, 4,
		0, 2, 4, 5,
		1, 3, 5, 7,
		0, 1, 2, 3,
		1, 3, 4, 5,
	};
	for (UInt i = 0; i < 4; i++)
	{
		int x = 16 + allFigures[_nextFigureType - 1][i] / 2;
		int y = 16 - allFigures[_nextFigureType - 1][i] % 2;
		drawTile(x, y, _nextFigureType);
	}
}

void Tetris::drawDigit(int x, int y, UInt digit)
{
	if (digit >= 10)
		return;
	_numbers.setTextureRect(sf::IntRect(32 * digit, 0, 32, 32));
	_numbers.setPosition(32 * x, 32 * y);
	_window.draw(_numbers);
}

void Tetris::drawNumber(int x, int y, UInt number)
{
	UInt n = number;
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

// ------------< TetrisFigure> ------------

TetrisFigure::TetrisFigure()
{
	_type = 0;
}

TetrisFigure::TetrisFigure(const TetrisFigure& figure)
{
	_type = figure._type;
	for (UInt i = 0; i < 4; i++)
		_figure[i] = figure._figure[i];
}

void TetrisFigure::initFigure(UInt type)
{
	if (type == 0 || type > 7)
		return;
	_type = type;
	int allFigures[7][4] = {
		0, 2, 3, 5,
		0, 2, 3, 4,
		1, 2, 3, 4,
		0, 2, 4, 5,
		1, 3, 5, 7,
		0, 1, 2, 3,
		1, 3, 4, 5,
	};
	for (UInt i = 0; i < 4; i++)
	{
		_figure[i].x = 4 + allFigures[type - 1][i] % 2;
		_figure[i].y = allFigures[type - 1][i] / 2;
	}
}

sf::Vector2i TetrisFigure::getTile(UInt ind) const
{
	if (ind < 4)
		return _figure[ind];
	return sf::Vector2i(0, 0);
}

UInt TetrisFigure::getType() const
{
	return _type;
}



void TetrisFigure::move(int dx)
{
	for (UInt i = 0; i < 4; i++)
		_figure[i].x += dx;
}

void TetrisFigure::down()
{
	for (UInt i = 0; i < 4; i++)
		_figure[i].y++;
}

void TetrisFigure::rotate()
{
	sf::Vector2i center = _figure[1];
	for (UInt i = 0; i < 4; i++)
	{
		int x = _figure[i].y - center.y;
		int y = _figure[i].x - center.x;
		_figure[i].x = center.x - x;
		_figure[i].y = center.y + y;
	}
}

// ------------< TetrisField >------------

TetrisField::TetrisField(UInt w, UInt h)
{
	_size = sf::Vector2u(w, h);
	_field = new UInt*[h];
	for (UInt i = 0; i < h; i++)
		_field[i] = new UInt[w]();
}

TetrisField::~TetrisField()
{
	for (UInt i = 0; i < _size.y; i++)
		delete[] _field[i];
	delete[] _field;
}

sf::Vector2u TetrisField::getSize() const
{
	return _size;
}

UInt TetrisField::getTile(int x, int y) const
{
	if (0 <= x && x < _size.x && 0 <= y && y < _size.y)
		return _field[y][x];
	return 8;
}

bool TetrisField::checkIntersection(const TetrisFigure& figure) const
{
	for (UInt i = 0; i < 4; i++)
	{
		sf::Vector2i tile = figure.getTile(i);
		if (getTile(tile.x, tile.y) != 0)
			return true;
	}
	return false;
}

UInt TetrisField::checkLines()
{
	UInt count = 0;
	for (UInt y = 0; y < _size.y; y++)
		if (isLine(y))
		{
			count++;
			moveDown(y);
		}
	return count;
}

bool TetrisField::isLine(UInt y) const
{
	if (y >= _size.y)
		return false;
	for (UInt x = 0; x < _size.x; x++)
		if (_field[y][x] == 0)
			return false;
	return true;
}

void TetrisField::moveDown(UInt y)
{
	if (y >= _size.y)
		return;
	for (UInt y0 = y; y0 >= 1; y0--)
		for (UInt x = 0; x < _size.x; x++)
		{
			_field[y0][x] = _field[y0 - 1][x];
			_field[y0 - 1][x] = 0;
		}
}

void TetrisField::drawFigure(const TetrisFigure& figure)
{
	UInt type = figure.getType();
	for (UInt i = 0; i < 4; i++)
	{
		sf::Vector2i tile = figure.getTile(i);
		_field[tile.y][tile.x] = type;
	}
}

void TetrisField::clear()
{
	for (UInt y = 0; y < _size.y; y++)
		for (UInt x = 0; x < _size.x; x++)
			_field[y][x] = 0;
}
