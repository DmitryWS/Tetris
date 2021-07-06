#include "TetrisField.hpp"

TetrisField::TetrisField()
{
	_size = sf::Vector2u(10, 20);
	_field = new unsigned short*[_size.y];
	for (int i = 0; i < _size.y; i++)
		_field[i] = new unsigned short[_size.x]();
}

TetrisField::~TetrisField()
{
	for (int i = 0; i < _size.y; i++)
		delete[] _field[i];
	delete[] _field;
}



sf::Vector2u TetrisField::getSize() const
{
	return _size;
}

unsigned short TetrisField::getTile(sf::Vector2i coords) const
{
	if (0 <= coords.x && coords.x < _size.x && 0 <= coords.y && coords.y < _size.y)
		return _field[coords.y][coords.x];
	return 1;
}



bool TetrisField::checkIntersection(const TetrisFigure& figure) const
{
	for (int i = 0; i < 4; i++)
	{
		sf::Vector2i tile = figure.getTile(i);
		if (getTile(tile) != 0)
			return true;
	}
	return false;
}

unsigned int TetrisField::checkLines()
{
	unsigned int count = 0;
	for (int y = 0; y < _size.y; y++)
		if (isLine(y))
		{
			count++;
			moveDown(y);
		}
	return count;
}

void TetrisField::drawFigure(const TetrisFigure& figure)
{
	unsigned short type = figure.getType();
	for (int i = 0; i < 4; i++)
	{
		sf::Vector2i tile = figure.getTile(i);
		_field[tile.y][tile.x] = type + 1; // CONFIG
	}
}

void TetrisField::clear()
{
	for (int y = 0; y < _size.y; y++)
		for (int x = 0; x < _size.x; x++)
			_field[y][x] = 0;
}



bool TetrisField::isLine(unsigned int y) const
{
	if (y >= _size.y)
		return false;
	for (int x = 0; x < _size.x; x++)
		if (_field[y][x] == 0)
			return false;
	return true;
}

void TetrisField::moveDown(unsigned int y)
{
	if (y >= _size.y)
		return;
	for (int y0 = y; y0 >= 1; y0--)
		for (int x = 0; x < _size.x; x++)
		{
			_field[y0][x] = _field[y0 - 1][x];
			_field[y0 - 1][x] = 0;
		}
}