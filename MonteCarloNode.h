#pragma once

#include "Game.h"
#include <random>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <vector>


struct MonteCarloNode {
private:
	static const int kN = 5; // visit 5 times before expanding
	static std::default_random_engine gen_;
	static std::uniform_int_distribution<int> dist_;

	int w_; // wins for last player
	int n_; // number of times visited
	int p_; // last player (init by parent)
	std::unordered_map<Move, std::shared_ptr<MonteCarloNode>> next_;

public:
	MonteCarloNode() : w_(0), n_(0), p_(0), next_() {}

	double uct(int t);

	Move move();

	double winPercentage(Move& m) {
		auto& x = *next_[m];
		return x.w_ * 1.0 / x.n_;
	}

	void initNext(const Game& game);

	int select(Board& board);
};
