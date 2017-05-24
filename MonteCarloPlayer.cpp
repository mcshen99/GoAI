#include "MonteCarloPlayer.h"
#include <iostream>

using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::unordered_set;
using std::hash;

Move MonteCarloPlayer::move(const Board& board, const std::unordered_set<std::pair<int, Board>>& history) {
	MonteCarloNode root;
	for (int i = 0; i < sims_; i++) {
		if (mod_ > 0 && (i + 1) % mod_ == 0) {
			cout << (i + 1) / mod_ << "/" << sims_ / mod_ << endl;
		}
		Board boardCopy(board);

		map<int, unordered_set<size_t>> historyCopy;
		historyCopy.insert({ 0, {} });
		historyCopy.insert({ 1, {} });
		for (const auto& it : history) {
			historyCopy[it.first].insert(it.second.getHash());
		}

		root.select(boardCopy, player_, komi_, historyCopy);
	}

	Move m = root.move();
	winP_ = root.winPercentage(m);

	return m;
}

ostream& MonteCarloPlayer::comment(ostream& s) const {
	return s << "Winning probability: " << winP_ << endl;
}