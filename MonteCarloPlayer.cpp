#include "MonteCarloPlayer.h"
#include <iostream>
#include <time.h>

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

    MonteCarloContext context;
    context.history = history;

    // TODO handle cases where player can play twice in a row
    Board b;
    int player = 0;
    for (const auto& m : history) {
      context.seen[(player++) % 2].insert(b.getHash());
      b.move(m);
    }
    context.seen[(player++) % 2].insert(b.getHash());

    root.select(boardCopy, player_, komi_, context);
  }

  Move m = root.move();
  winP_ = root.getNext().at(m)->winProbability();

  log << sims_ / (float(clock() - begin) / CLOCKS_PER_SEC) << " sims / sec" << endl;
  log << "Winning probability: " << winP_ << endl;
  log << "Visits: " << root.getNext().at(m)->visits() << endl;
  log << board << endl;
  if (mod_ > 0) {
    cout << log.str() << flush;
  }

  probs.clear();
  visits.clear();
  raveProbs.clear();
  raveVisits.clear();
  priors.clear();
  const auto& next = root.getNext();
  for (const auto& entry : next) {
    probs.emplace_back(entry.first, to_string(entry.second->winProbability()));
    visits.emplace_back(entry.first, to_string(entry.second->visits()));
    raveProbs.emplace_back(entry.first, to_string(entry.second->raveWinProbability()));
    raveVisits.emplace_back(entry.first, to_string(entry.second->raveVisits()));
    priors.emplace_back(entry.first, to_string(entry.second->priorProbs()));
  }

  primaryVar.clear();
  MonteCarloNode* node = root.getNext().at(m).get();
  while (!node->getNext().empty()) {
    Move var = node->move();
    if (var.isPass()) {
      break;
    }

    primaryVar.emplace_back(var.getCoor());
    node = node->getNext().at(var).get();
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

vector<pair<Move, string>> MonteCarloPlayer::raveMoveProbabilities() const {
  return raveProbs;
}

vector<pair<Move, string>> MonteCarloPlayer::raveMoveVisits() const {
  return raveVisits;
}

vector<pair<Move, string>> MonteCarloPlayer::movePriorProbabilities() const {
  return priors;
}

std::vector<pos> MonteCarloPlayer::primaryVariation() const {
  return primaryVar;
}
