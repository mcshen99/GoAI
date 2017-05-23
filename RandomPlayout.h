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

	std::map<pos, double> gen_playout(const Board& board, int player);

	std::map<pos, double> isPlayable(const Board& board, const Move& m);

public:
	RandomPlayout(std::vector<double> komi);

	Move move(const Board& board, int player);

	int simulate(Board& board, int player);
};