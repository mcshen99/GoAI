#include "MoveGenerator.h"
#include <random>

using std::default_random_engine;
using std::uniform_int_distribution;

default_random_engine MoveGenerator::gen_;
uniform_int_distribution<int> MoveGenerator::dist_(0, SIZE - 1);

MoveGenerator::MoveGenerator(const Board& b, int player) : current_({ dist_(gen_), dist_(gen_) }), count_(0), board_(b), player_(player) { }

std::pair<bool, Move> MoveGenerator::next() {
	while (!empty() && board_.getBoard()[current_.first][current_.second] != 0) {
		current_.first = (current_.first + 1) % SIZE;
		if (current_.first == 0) {
			current_.second = (current_.second + 1) % SIZE;
		}

		count_++;
	}

	std::pair<bool, Move> p = { empty(), Move::move(current_, player_) };
	if (!empty()) {
		current_.first = (current_.first + 1) % SIZE;
		if (current_.first == 0) {
			current_.second = (current_.second + 1) % SIZE;
		}
		count_++;
	}

	return p;
}

bool MoveGenerator::empty() {
	return count_ >= SIZE * SIZE;
}