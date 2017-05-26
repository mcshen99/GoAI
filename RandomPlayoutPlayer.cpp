#include "RandomPlayoutPlayer.h"

using std::ostream;
using std::vector;

ostream &RandomPlayoutPlayer::comment(ostream &s) const {
  return s;
}

Move RandomPlayoutPlayer::move(const Board &board, const std::vector<Move> &history) {
  vector<Move> last;
  for (int i = 0; i < 2; i++) {
    int index = history.size() - 2 + i;
    if (index >= 0) {
      last.push_back(history[index]);
    }
  }
  return playout_.move(board, player_, last);
}

RandomPlayoutPlayer::RandomPlayoutPlayer(int player, vector<double> komi)
    : komi_(komi), player_(player), playout_(komi) {}

