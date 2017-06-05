#include "MonteCarloNode.h"
#include <iostream>

using namespace std;

double MonteCarloNode::eval(int t) {
  if (n_ == 0) {
    // should never reach here
    return 10000;
  }

  double beta = rn_ / (rn_ + n_ + rn_ * n_ * kRaveBias);
  return beta * rq_ + (1 - beta) * q_;
}

void MonteCarloNode::update(double z) {
  n_++;
  q_ += (z - q_) / n_;
}

void MonteCarloNode::updateRave(double z) {
  rn_++;
  rq_ += (z - rq_) / rn_;
}

void MonteCarloNode::updateRaveChildren(const unordered_set<Move>& rave, int winner) {
  for (const auto& it : next_) {
    if (rave.find(it.first) != rave.end()) {
      if (it.second->p_ == winner) {
        it.second->updateRave(1.0);
      } else {
        it.second->updateRave(0.0);
      }
    }
  }
}

Move MonteCarloNode::move() {
  auto element = std::max_element(
      next_.begin(), next_.end(), [](const auto& a, const auto& b) {
        return a.second->n_ < b.second->n_;
      });

  return element->first;
}

void MonteCarloNode::initNext(
    const Board& board,
    int player,
    const vector<double>& komi,
    const MonteCarloContext& context) {
  const auto& playerHistory = context.seen[((player + 1) % 2)];
  int color = player + 1;
  vector<Move> moves = board.getValidMoves(color, playerHistory);

  //for each move, add to next
  auto cfgMap = cfgDistance(board, context.history);

  for (const auto& it : moves) {
    next_[it] = std::make_shared<MonteCarloNode>();
    next_[it]->p_ = player;
    next_[it]->q_ = 0.5;
    if (it.isPass()) {
      continue;
    }

    pos p = it.getCoor();
    if (!context.history.empty()) {
      int dist = cfgMap[p.first][p.second] - 1;
      if (dist < 3) {
        if (dist == 2) {
          next_[it]->q_ += 0.08;
        } else if (dist == 1) {
          next_[it]->q_ += 0.22;
        } else {
          next_[it]->q_ += 0.24;
        }
      }
    }

    int height = std::min(std::min(p.first, p.second), std::min(SIZE - 1 - p.first, SIZE - 1 - p.second));
    if (height <= 2 && board.isEmpty(p)) {
      if (height <= 1) {
        next_[it]->q_ -= 0.10;
      } else {
        next_[it]->q_ += 0.10;
      }
    }
  }
  vector<pos> all;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      all.emplace_back(i, j);
    }
  }

  CaptureGenerator capture(board, color, all);
  for (auto p = capture.next(); !p.first; p = capture.next()) {
    Move m = p.second;
    if (RandomPlayout::isOkMove(board, m, playerHistory)) {
      auto it = next_.find(m);
      if (it != next_.end()) {
        it->second->q_ += 0.15;
      }
    }
  }

  PatternGenerator pattern(board, color, all);
  for (auto p = pattern.next(); !p.first; p = pattern.next()) {
    Move m = p.second;
    if (RandomPlayout::isOkMove(board, m, playerHistory)) {
      auto it = next_.find(m);
      if (it != next_.end()) {
        it->second->q_ += 0.10;
      }
    }
  }

  for (const auto& it : moves) {
    next_[it]->q_ = std::min(std::max(0.0, next_[it]->q_), 1.0);
    next_[it]->pq_ = next_[it]->q_;
    next_[it]->rq_ = next_[it]->q_;
    next_[it]->rn_ = kM;
    next_[it]->n_ = kM;
  }
}

int MonteCarloNode::select(
    Board& board,
    int player,
    const vector<double>& komi,
    MonteCarloContext& context) {
  if (n_ < kN + kM) {
    RandomPlayout rp(komi);
    int winner = rp.simulate(board, player, context.history, context.seen, context.rave);

    if (p_ == winner) {
      update(1.0);
    } else {
      update(0.0);
    }
    return winner;
  }

  if (n_ == kN + kM) {
    initNext(board, player, komi, context);
  }

  auto element = max_element(
      next_.begin(), next_.end(), [this](const auto& a, const auto& b) {
        return a.second->eval(n_) < b.second->eval(n_);
      });

  board.move(element->first);
  context.seen[((player + 1) % 2)].insert(board.getHash());
  context.history.push_back(element->first);
  context.parents.push_back(this);

  // state of history, seen, and parents are undefined after select
  int winner = element->second->select(board, (player + 1) % 2, komi, context);

  // rave contains everything after
  updateRaveChildren(context.rave, winner);
  context.rave.insert(element->first);

  if (p_ == winner) {
    update(1.0);
  } else {
    update(0.0);
  }
  return winner;
}

std::array<std::array<int, SIZE>, SIZE> MonteCarloNode::cfgDistance(const Board& board, const vector<Move>& history) {
  // common fate graph
  std::array<std::array<int, SIZE>, SIZE> cfgMap;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      cfgMap[i][j] = -1;
    }
  }

  if (history.empty() || history.back().isPass()) {
    return cfgMap;
  }

  const Move& m = history.back();
  pos p = m.getCoor();
  cfgMap[p.first][p.second] = 0;

  std::vector<pos> fringe;
  fringe.push_back(p);
  while (!fringe.empty()) {
    pos c;
    c = fringe.back();
    fringe.pop_back();
    for (int i = 0; i < 4; i++) {
      int x = Board::dirs[0][i];
      int y = Board::dirs[1][i];
      pos d = {c.first + x, c.second + y};
      if (!board.inBounds(d) ||
          (0 <= cfgMap[d.first][d.second] && cfgMap[d.first][d.second] <= cfgMap[c.first][c.second])) {
        continue;
      }
      int before = cfgMap[d.first][d.second];
      const auto& b = board.getBoard();
      if (b[d.first][d.second] != 0 && b[d.first][d.second] == b[c.first][c.second]) {
        cfgMap[d.first][d.second] = cfgMap[c.first][c.second];
      } else {
        cfgMap[d.first][d.second] = cfgMap[c.first][c.second] + 1;
      }

      if (before < 0 || before > cfgMap[d.first][d.second]) {
        fringe.push_back(d);
      }
    }
  }

  return cfgMap;
}
