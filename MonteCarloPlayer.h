#pragma once

#include <sstream>
#include "Player.h"
#include "MonteCarloNode.h"

class MonteCarloPlayer : public Player {
private:
	int player_;
	std::vector<double> komi_;

	const int sims_;
	int mod_;
	double winP_;

	std::ostringstream log;

public:
	MonteCarloPlayer(int color, std::vector<double> komi, int sims = 10000, int mod = 0) : player_(color - 1), komi_(komi), sims_(sims), mod_(mod) {}

	Move move(const Board& board, const std::vector<Move>& history) override;

	std::ostream& comment(std::ostream& s) const override;
};