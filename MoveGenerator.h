#pragma once

#include <vector>

#include "Board.h"
#include "RandomPlayout.h"
#include "PatternGenerator.h"
#include "CaptureGenerator.h"

class MoveGenerator {
 private:
  bool usePattern_;
  bool useCapture_;
  PatternGenerator pattern_;
  CaptureGenerator capture_;

  pos current_;
  int count_;

  const Board& board_;
  int color_;

  static std::default_random_engine gen_;
  static std::uniform_int_distribution<int> dist_;
  static std::uniform_real_distribution<double> real_;

 public:
  MoveGenerator(const Board& b, int color, const std::vector<pos>& moves);

  std::pair<bool, Move> next();

  bool empty();
};