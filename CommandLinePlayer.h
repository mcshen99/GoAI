#pragma once

#include "Player.h"
#include <ostream>

struct CommandLinePlayer : public Player {
private:
	int color_;

public:
	CommandLinePlayer(int color);

	Move move(const Board& board);

	std::ostream& comment(std::ostream& s) const;
};