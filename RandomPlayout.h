#pragma once

#include <random>
#include "Board.h"
#include <map>

class RandomPlayout {
private:
	const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };

	std::default_random_engine gen_;

	std::uniform_real_distribution<double> dist_;

	std::vector<double> komi_;

	//pair<bool, vector<pos>> fix_atari(pos position, pos coor, bool singlept_ok = false, bool twolib_test = true, bool twolib_edgeonly = false);

public:
	RandomPlayout(std::vector<double> komi);

	Move move(const Board& board, int player);

	int simulate(Board& board, int player);

	std::map<pos, double> gen_playout(const Board& board, int player);
};