#include "RandomPlayoutPlayer.h"

using std::ostream;
using std::vector;

ostream& RandomPlayoutPlayer::comment(ostream& s) const {
	return s;
}

Move RandomPlayoutPlayer::move(const Board& board) {
	return playout_.move(board, player_);
}

RandomPlayoutPlayer::RandomPlayoutPlayer(vector<double> komi, int player) : komi_(komi), player_(player), playout_(komi) {}