//
// Created by Jeffrey Shen on 5/24/17.
//

#include "CaptureGenerator.h"

CaptureGenerator::CaptureGenerator(const Board& b, int color, const std::vector<pos>& moves) :
    board_(b), color_(color), moves_(moves), m_(0) {}

std::pair<bool, Move> CaptureGenerator::next() {
  while (m_ < moves_.size()) {
    if (!answers_.empty()) {
      Move m = Move::move(answers_.back(), color_);
      answers_.pop_back();
      return {false, m};
    }

    pos p = moves_[m_];

    if (board_.inBounds(p)) {
      auto answers = board_.fixAtari(p, color_);
      for (auto p : answers) {
//                if (board_.getBoard()[p.first][p.second])
        // should be always safe
        answers_.push_back(p);
      }
    }

    m_++;
  }

  if (!answers_.empty()) {
    Move m = Move::move(answers_.back(), color_);
    answers_.pop_back();
    return {false, m};
  }

  return {true, Move::pass(color_)};
}
