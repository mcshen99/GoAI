#include "MonteCarloPlayer.h"
#include <iostream>

using namespace std;

Move MonteCarloPlayer::move(const Board& board, const vector<Move>& history) {
  log.str(string());

  MonteCarloNode root;
  const clock_t begin = clock();
  for (int i = 0; i < sims_; i++) {
    if (mod_ > 0 && (i + 1) % mod_ == 0) {
      cout << (i + 1) / mod_ << "/" << sims_ / mod_ << endl;
    }
    Board boardCopy(board);

    array<unordered_set<size_t>, 2> historyCopy;
    Board b;
    int player = 0;
    for (const auto& m : history) {
      historyCopy[(player++) % 2].insert(b.getHash());
      b.move(m);
    }
    historyCopy[(player++) % 2].insert(b.getHash());

    if (history.empty()) {
      pair<Move, Move> blank = {Move::pass(player), Move::pass(((player_ + 1) % 2) + 1)};
      root.select(boardCopy, player_, komi_, blank, historyCopy);
    } else if (history.size() == 1) {
      root.select(boardCopy, player_, komi_, {Move::pass(player), history.back()}, historyCopy);
    } else {
      root.select(boardCopy, player_, komi_, {history[history.size() - 2], history.back()}, historyCopy);
    }
  }

  Move m = root.move();
  winP_ = root.getNext().at(m)->winPercentage();

  log << sims_ / (float(clock() - begin) / CLOCKS_PER_SEC) << " sims / sec" << endl;
  log << "Winning probability: " << winP_ << endl;
  log << "Visits: " << root.getNext().at(m)->visits() << endl;
  if (mod_ > 0) {
    cout << log.str() << flush;
  }

  probs.clear();
  visits.clear();
  priors.clear();
  const auto& next = root.getNext();
  for (const auto& entry : next) {
    probs.emplace_back(entry.first, to_string(entry.second->winPercentage()));
    visits.emplace_back(entry.first, to_string(entry.second->visits()));
    priors.emplace_back(entry.first, to_string(entry.second->priorVisits()));
  }

  return m;
}

ostream& MonteCarloPlayer::comment(ostream& s) const {
  return s << log.str() << flush;
}

vector<pair<Move, string>> MonteCarloPlayer::moveProbabilities() const {
  return probs;
}

vector<pair<Move, string>> MonteCarloPlayer::moveVisits() const {
  return visits;
}

vector<pair<Move, string>> MonteCarloPlayer::movePriors() const {
  return priors;
}
