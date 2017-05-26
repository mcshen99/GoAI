#include "MoveGenerator.h"

using std::default_random_engine;
using std::uniform_int_distribution;
using std::uniform_real_distribution;

default_random_engine MoveGenerator::gen_;
uniform_int_distribution<int> MoveGenerator::dist_(0, SIZE - 1);
uniform_real_distribution<double> MoveGenerator::real_(0.0, 1.0);

MoveGenerator::MoveGenerator(const Board& b, int color, const std::vector<pos>& moves) :
    usePattern_(real_(gen_) < 0.95),
    useCapture_(real_(gen_) < 0.95),
    pattern_(b, color, moves),
    capture_(b, color, moves),
    current_({dist_(gen_), dist_(gen_)}),
    count_(0),
    board_(b),
    color_(color) {}

std::pair<bool, Move> MoveGenerator::next() {
  if (useCapture_) {
    auto move = capture_.next();
    if (!move.first) {
      return move;
    }
  }

  if (usePattern_) {
    auto move = pattern_.next();
    if (!move.first) {
      return move;
    }
  }

  while (!empty() && board_.getBoard()[current_.first][current_.second] != 0) {
    current_.first = (current_.first + 1) % SIZE;
    if (current_.first == 0) {
      current_.second = (current_.second + 1) % SIZE;
    }

    count_++;
  }

  std::pair<bool, Move> p = {empty(), Move::move(current_, color_)};
  if (!empty()) {
    current_.first = (current_.first + 1) % SIZE;
    if (current_.first == 0) {
      current_.second = (current_.second + 1) % SIZE;
    }
    count_++;
  }

  return p;
}

bool MoveGenerator::empty() {
  return count_ >= SIZE * SIZE;
}