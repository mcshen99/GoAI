#include "RandomPlayoutPlayer.h"

using std::ostream;
using std::vector;

ostream& RandomPlayoutPlayer::comment(ostream& s) const {
  return s;
}

Move RandomPlayoutPlayer::move(const Board& board, const std::vector<Move>& history) {
  vector<Move> last;
  for (int i = 0; i < 2; i++) {
    int index = history.size() - 2 + i;
    if (index >= 0) {
      last.push_back(history[index]);
    }
  }

  Board boardCopy(board);
  std::map<int, std::unordered_set<size_t>> historyCopy;
  Board b;
  int player = 0;
  for (const auto& m : history) {
    historyCopy[(player++) % 2].insert(b.getHash());
    b.move(m);
  }
  historyCopy[(player++) % 2].insert(b.getHash());

  return playout_.move(board, player_, historyCopy[(player + 1) % 2], last);
}

RandomPlayoutPlayer::RandomPlayoutPlayer(int player, vector<double> komi)
    : komi_(komi), player_(player), playout_(komi) {}

