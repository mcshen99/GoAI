#include "MonteCarloPlayer.h"
#include <iostream>
#include <histedit.h>

using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::map;

Move MonteCarloPlayer::move(const Board& board) {
	MonteCarloNode root;
	for (int i = 0; i < sims_; i++) {
		if (mod_ > 0 && (i + 1) % mod_ == 0) {
			cout << (i + 1) / mod_ << "/" << sims_ / mod_ << endl;
		}
		Board boardCopy(board);
		map<int, std::unordered_set<size_t>> history;
		root.select(boardCopy, player_, komi_, history);
	}

	Move m = root.move();
	winP_ = root.winPercentage(m);

	return m;
}

ostream& MonteCarloPlayer::comment(ostream& s) const {
	return s << "Winning probability: " << winP_ << endl;
}