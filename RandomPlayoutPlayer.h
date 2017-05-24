#pragma once

#include "Player.h"
#include "RandomPlayout.h"

class RandomPlayoutPlayer : public Player {
private:
	std::vector<double> komi_;

	int player_;

	RandomPlayout playout_;

public:
	RandomPlayoutPlayer(int player, std::vector<double> komi);

	std::ostream& comment(std::ostream& s) const override;

	Move move(const Board& board, const std::vector<Move>& history) override;
};