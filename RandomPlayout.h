#pragma once

#include <random>
#include "Board.h"
#include <map>
#include <unordered_map>
#include "MoveGenerator.h"

class RandomPlayout {
 private:
  const static int dirs[2][4];

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
      const std::unordered_set<size_t>& history,
      const std::vector<Move>& lastMoves);

  int simulate(
      Board& board,
      int player,
      std::pair<Move, Move> lastMove,
      std::array<std::unordered_set<size_t>, 2>& history);

  static bool isOkMove(const Board& board, const Move& m, const std::unordered_set<size_t>& history);
};