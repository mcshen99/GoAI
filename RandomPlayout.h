#pragma once

#include <random>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "MoveGenerator.h"
#include "Board.h"

class RandomPlayout {
 private:
  const static int dirs[2][4];

  static std::minstd_rand gen_;

  std::vector<double> komi_;


  static bool isGroup(const Board& board, const Move& m);

  static bool isAtari(const Board& board, const Move& m);

  static bool isCapture(const Board& board, const Move& m);

  static bool isEyeFilling(const Board& board, const Move& m);

 public:
  RandomPlayout(std::vector<double> komi);

  Move move(
      const Board& board,
      int player,
      const std::unordered_set<size_t>& seen,
      const std::vector<Move>& lastMoves);

  int simulate(
      Board& board,
      int player,
      const std::vector<Move>& history,
      std::array<std::unordered_set<size_t>, 2>& seen,
      std::unordered_set<Move>& rave);

  static bool isOkMove(const Board& board, const Move& m, const std::unordered_set<size_t>& seen);
};