#ifndef TETRIS_FIELD_HPP
#define TETRIS_FIELD_HPP

#include <SFML/System.hpp>
#include "TetrisFigure.hpp"

class TetrisField
{
public:
	TetrisField();
	~TetrisField();

	sf::Vector2u getSize() const;
	unsigned short getTile(sf::Vector2i coords) const;

	bool checkIntersection(const TetrisFigure& figure) const;
	unsigned int checkLines();
	void drawFigure(const TetrisFigure& figure);
	void clear();
private:
	sf::Vector2u _size;
	unsigned short** _field;

	bool isLine(unsigned int y) const;
	void moveDown(unsigned int y);
};

#endif