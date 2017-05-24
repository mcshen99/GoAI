#pragma once

#include <random>
#include "Board.h"
#include <map>

class RandomPlayout {
private:
	struct GeneratorState {
		int x;
		int y;
	};

	const static int dirs[2][4];

	static std::default_random_engine gen_;

	static std::uniform_real_distribution<double> dist_;

	std::vector<double> komi_;

	bool isGroup(const Board& board, const Move& m);

	bool isAtari(const Board& board, const Move& m);

	bool isCapture(const Board& board, const Move& m);

	bool isEyeFilling(const Board& board, const Move& m);

	bool isOkMove(const Board& board, const Move& m);

	std::map<pos, double> gen_playout(const Board& board, int player);

public:
	RandomPlayout(std::vector<double> komi);

	Move move(const Board& board, int player);

	int simulate(Board& board, int player, const std::map<int, std::unordered_set<size_t>>& history);
};