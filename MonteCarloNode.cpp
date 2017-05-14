#include "MonteCarloNode.h"
#include <random>

using std::default_random_engine;
using std::uniform_int_distribution;

//idek if these numbers are right
default_random_engine MonteCarloNode::gen_(1776);
uniform_int_distribution<int> MonteCarloNode::dist_(0, 10000);

double MonteCarloNode::uct(int t) {
	if (n_ == 0) {
		return 10000 + dist_(gen_);
	}

	return ((double)w_) / n_ + 0.45 * sqrt(log(t) / n_);
}

Move MonteCarloNode::move() {
	auto element = std::max_element(next_.begin(), next_.end(), [](const auto& a, const auto& b) {
		return a.second->n_ < b.second->n_;
	});

	return element->first;
}

void MonteCarloNode::initNext(const Game& game) {
	//don't get next if the game is over obv xd
	if (game.isGameOver()) {
		return;
	}

	//idek 
	std::vector<Move> moves = game.getMoves();

	//for each move, add to next
	for (Move& move : moves) {
		next_[move] = std::make_shared<MonteCarloNode>();
		next_[move]->p_ = game.getCurrentPlayer();
	}

}

int MonteCarloNode::select(Board& board) {
	if (n_ < kN) {
		int winner = RandomPlayout::simulate(board);
		if (p_ == winner) {
			w_++;
		}

		n_++;
		return winner;
	}

	if (n_ == kN) {
		initNext(board);
	}

	if (next_.empty()) {
		int winner = board.getCurrentPlayer();
		if (p_ == winner) {
			w_++;
		}

		n_++;
		return winner;
	}

	auto element = max_element(next_.begin(), next_.end(), [this](auto& a, auto& b) {
		return a.second->uct(n_) < b.second->uct(n_);
	});


	board.move(element->first);

	int winner = element->second->select(board);

	if (winner == p_) {
		w_++;
	}
	n_++;
	return winner;
}