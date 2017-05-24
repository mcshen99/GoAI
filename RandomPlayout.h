#pragma once

#include <random>
#include "Board.h"
#include <map>
#include "MoveGenerator.h"

class RandomPlayout {
private:
	struct GeneratorState {
		int x;
		int y;
	};

	const static int dirs[2][4];

	std::vector<double> komi_;

	static bool isGroup(const Board& board, const Move& m);

	static bool isAtari(const Board& board, const Move& m);

	static bool isCapture(const Board& board, const Move& m);

	static bool isEyeFilling(const Board& board, const Move& m);

	std::map<pos, double> gen_playout(const Board& board, int player);

public:
	RandomPlayout(std::vector<double> komi);

	Move move(const Board& board, int player);

	int simulate(Board& board, int player, const std::map<int, std::unordered_set<size_t>>& history);

	static bool isOkMove(const Board& board, const Move& m);
};