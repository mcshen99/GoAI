#pragma once

#include "Player.h"
#include "MonteCarloNode.h"

class MonteCarloPlayer : public Player {
private:
	const int sims_;
	double winP_;
	int player_;
	std::vector<double> komi_;

public:
	MonteCarloPlayer(int sims, int player, std::vector<double> komi) : sims_(sims), player_(player), komi_(komi) {}

	Move move(const Board& board) override;

	std::ostream& comment(std::ostream& s) const override;
};