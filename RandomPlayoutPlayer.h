#pragma once

#include "Player.h"
#include "RandomPlayout.h"

class RandomPlayoutPlayer : public Player {
private:
	std::vector<double> komi_;

	int player_;

	RandomPlayout playout_;

public:
	RandomPlayoutPlayer(std::vector<double> komi, int player);

	std::ostream& comment(std::ostream& s) const override;

	Move move(const Board& board) override;
};