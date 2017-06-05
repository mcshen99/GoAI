#pragma once

#include "Board.h"
#include "RandomPlayout.h"
#include <random>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <vector>

class MonteCarloNode;
struct MonteCarloContext {
  std::vector<Move> history;
  std::vector<MonteCarloNode*> parents;
  std::array<std::unordered_set<size_t>, 2> seen;
  std::unordered_set<Move> rave;
};

class MonteCarloNode {
 private:
  const int kM = 50; // prior N which represents the confidence in our priors
  const int kN = 2; // visit kN times before expanding
  const double kRaveBias = 1.0 / 3500;

  double q_; // q value, the "probability" for last player
  int n_; // number of times visited
  double rq_; // rave q value
  int rn_; // rave n value
  double pq_; // prior q value
  int p_; // last player (init by parent)
  std::unordered_map<Move, std::shared_ptr<MonteCarloNode>> next_;

 public:
  MonteCarloNode() : q_(0), n_(0), rq_(0), rn_(0), pq_(0), p_(0),  next_() {}

  double eval(int t);

  Move move();

  double winProbability() {
    return q_;
  }

  int visits() {
    return n_;
  }

  double priorProbs() {
    return pq_;
  }

  double raveVisits() {
    return rn_;
  }

  double raveWinProbability() {
    return rq_;
  }

  const std::unordered_map<Move, std::shared_ptr<MonteCarloNode>>& getNext() {
    return next_;
  };

  void update(double z);
  void updateRave(double z);
  void updateRaveChildren(const std::unordered_set<Move>& rave, int winner);

  void initNext(
      const Board& board,
      int player,
      const std::vector<double>& komi,
      const MonteCarloContext& context);

  int select(
      Board& board,
      int player,
      const std::vector<double>& komi,
      MonteCarloContext& context);

  static std::array<std::array<int, SIZE>, SIZE> cfgDistance(const Board& board, const std::vector<Move>& m);
};
