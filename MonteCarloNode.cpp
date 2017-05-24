#include "MonteCarloNode.h"
#include <random>
#include "RandomPlayout.h"
#include <iostream>
#include <math.h>

using std::default_random_engine;
using std::uniform_int_distribution;
using std::map;
using std::vector;
using std::unordered_set;
using std::hash;
using std::cout;
using std::endl;

//idek if these numbers are right
default_random_engine MonteCarloNode::gen_(1776);
uniform_int_distribution<int> MonteCarloNode::dist_(0, 10000);

double MonteCarloNode::uct(int t) {
	if (n_ == 0) {
		return 10000; //TODO: change later
	}

	return ((double)w_) / n_ + 0.45 * std::sqrt(std::log(t) / n_);
}

Move MonteCarloNode::move() {
	auto element = std::max_element(next_.begin(), next_.end(), [](const auto& a, const auto& b) {
		return a.second->n_ < b.second->n_;
	});

	return element->first;
}

void MonteCarloNode::initNext(const Board& board, int player, const vector<double>& komi, unordered_set<size_t>& playerHistory) {
	vector<Move> moves = board.getValidMoves(player + 1, playerHistory);

	//for each move, add to next
	for (const auto& it : moves) {
		next_[it] = std::make_shared<MonteCarloNode>();
		next_[it]->p_ = (player + 1) % 2;
	}
}

int MonteCarloNode::select(Board& board, int player, const vector<double>& komi, map<int, unordered_set<size_t>>& history) {
	if (n_ < kN) {
		RandomPlayout rp(komi);
		int winner = rp.simulate(board, player, history); 
		if (p_ == winner) {
			w_++;
		}

		n_++;
		return winner;
	}

	if (n_ == kN) {
		initNext(board, player, komi, history[((player + 1) % 2)]);
	}

	if (next_.empty()) {
		int winner = player;
		if (p_ == winner) {
			w_++;
		}

		n_++;
		return winner;
	}

	auto element = max_element(next_.begin(), next_.end(), [this](const auto& a, const auto& b) {
		return a.second->uct(n_) < b.second->uct(n_);
	});

	board.move(element->first); 
	history[((player + 1) % 2)].insert(board.getHash());
	int winner = element->second->select(board, (player + 1) % 2, komi, history); 

	if (winner == p_) {
		w_++;
	}
	n_++;
	return winner;
}