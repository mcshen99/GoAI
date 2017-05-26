#include "MonteCarloPlayer.h"
#include <iostream>

using namespace std;

Move MonteCarloPlayer::move(const Board& board, const std::vector<Move>& history) {
  log.str(std::string());

  MonteCarloNode root;
  const clock_t begin = clock();
  for (int i = 0; i < sims_; i++) {
    if (mod_ > 0 && (i + 1) % mod_ == 0) {
      cout << (i + 1) / mod_ << "/" << sims_ / mod_ << endl;
    }
    Board boardCopy(board);

    map<int, unordered_set<size_t>> historyCopy;
    Board b;
    historyCopy[0].insert(b.getHash());
    int player = 0;
    for (const auto& move : history) {
      b.move(move);
      historyCopy[(player++) % 2].insert(b.getHash());
    }

    if (history.empty()) {
      std::pair<Move, Move> blank = {Move::pass(player), Move::pass(((player_ + 1) % 2) + 1)};
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
  log << "Visits: " << root.getNext().at(m)->priorVisits() << endl;

  probs.clear();
  visits.clear();
  priors.clear();
  const auto& next = root.getNext();
  for (const auto& entry : next) {
    probs.emplace_back(entry.first, std::to_string(entry.second->winPercentage()));
    visits.emplace_back(entry.first, std::to_string(entry.second->visits()));
    priors.emplace_back(entry.first, std::to_string(entry.second->priorVisits()));
  }

  return m;
}

ostream& MonteCarloPlayer::comment(ostream& s) const {
  return s << log.str() << std::flush;
}

std::vector<std::pair<Move, std::string>> MonteCarloPlayer::moveProbabilities() const {
  return probs;
}

std::vector<std::pair<Move, std::string>> MonteCarloPlayer::moveVisits() const {
  return visits;
}

std::vector<std::pair<Move, std::string>> MonteCarloPlayer::movePriors() const {
  return priors;
}
