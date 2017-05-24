#include "RandomPlayoutPlayer.h"

using std::ostream;
using std::vector;

ostream& RandomPlayoutPlayer::comment(ostream& s) const {
	return s;
}

Move RandomPlayoutPlayer::move(const Board& board, const std::unordered_set<std::pair<int, Board>>& history) {
	return playout_.move(board, player_);
}

RandomPlayoutPlayer::RandomPlayoutPlayer(int player, vector<double> komi) : komi_(komi), player_(player), playout_(komi) {}