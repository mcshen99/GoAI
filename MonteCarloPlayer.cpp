#include "MonteCarloPlayer.h"
#include <iostream>

using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::map;

Move MonteCarloPlayer::move(const Board& board) {
	MonteCarloNode root;
	int mod = 100;
	for (int i = 0; i < sims_; i++) {
		if ((i + 1) % mod == 0) {
			cout << (i + 1) / mod << "/" << sims_ / mod << endl;
		}
		Board boardCopy(board);
		map<int, vector<Board>> history;
		root.select(boardCopy, player_, komi_, history);
	}

	Move m = root.move();
	winP_ = root.winPercentage(m);

	return m;
}

ostream& MonteCarloPlayer::comment(ostream& s) const {
	return s << "Winning probability: " << winP_ << endl;
}