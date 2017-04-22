#pragma once

#include <vector>
#include <memory>
#include "Player.h"
#include <unordered_set>
#include <utility>

namespace std {
	template<>
	struct std::hash<std::pair<int, Board>> {
		size_t operator()(const std::pair<int, Board>& state) const {
			return state.first + std::hash<Board>{}(state.second) * 31;
		}
	};
}

class Game {
private:
	std::vector<std::shared_ptr<Player>> players_;
	Board board_;
	std::unordered_set<std::pair<int, Board>> states_;
	int player_;
	std::vector<bool> resigned_;
	int numPasses_;
	int numOut_;
	std::vector<double> komi_;

public:
	Game(std::vector<std::shared_ptr<Player>> players, std::vector<double> komi);

	int run();
};