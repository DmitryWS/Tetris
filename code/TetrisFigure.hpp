#ifndef TETRIS_FIGURE_HPP
#define TETRIS_FIGURE_HPP

#include <SFML/System.hpp>

class TetrisFigure
{
public:
	TetrisFigure();
	TetrisFigure(const TetrisFigure& figure);
	void initFigure(unsigned short type);

	sf::Vector2i getTile(unsigned int ind) const;
	unsigned short getType() const;

	void move(int dx);
	void down();
	void rotate();
private:
	sf::Vector2i _figure[4];
	unsigned short _type;
};

#endif