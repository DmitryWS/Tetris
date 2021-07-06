#include "TetrisFigure.hpp"

TetrisFigure::TetrisFigure()
{
	_type = 0;
}

TetrisFigure::TetrisFigure(const TetrisFigure& figure)
{
	_type = figure._type;
	for (int i = 0; i < 4; i++)
		_figure[i] = figure._figure[i];
}

void TetrisFigure::initFigure(unsigned short type)
{
	if (type == 0 || type > 7)
		throw std::out_of_range("TetrisFigure::initFigure");

	_type = type;
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
		_figure[i].x = 4 + allFigures[type - 1][i] % 2;
		_figure[i].y = allFigures[type - 1][i] / 2;
	}
}



sf::Vector2i TetrisFigure::getTile(unsigned int ind) const
{
	if (ind < 4)
		return _figure[ind];
	throw std::out_of_range("TetrisFigure::getTile");
}

unsigned short TetrisFigure::getType() const
{
	return _type;
}



void TetrisFigure::move(int dx)
{
	for (int i = 0; i < 4; i++)
		_figure[i].x += dx;
}

void TetrisFigure::down()
{
	for (int i = 0; i < 4; i++)
		_figure[i].y++;
}

void TetrisFigure::rotate()
{
	sf::Vector2i center = _figure[1];
	for (int i = 0; i < 4; i++)
	{
		int x = _figure[i].y - center.y;
		int y = _figure[i].x - center.x;
		_figure[i].x = center.x - x;
		_figure[i].y = center.y + y;
	}
}