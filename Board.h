#pragma once

#include "Move.h"
#include <array>
#include <ostream>
#include <vector>
#include <unordered_set>

const int SIZE = 9;

struct Board {
 private:
  const static std::array<std::array<size_t, SIZE>, SIZE> TABLE;

  mutable std::array<std::array<int, SIZE>, SIZE> board_ = {};
  size_t hash_ = 0;

  static std::array<std::array<size_t, SIZE>, SIZE> getTable();

 public:
  const static int dirs[2][4];

  bool isSuicide(const Move& move) const;

  int liberties(pos p) const;
  bool hasLiberties(pos p, int lib) const;

  std::vector<Move> getValidMoves(int color, const std::unordered_set<size_t>& playerHistory) const;

  std::vector<pos> getCaptured(pos position, int color) const;

  bool isCaptured(pos position, int color) const;

  const std::array<std::array<int, SIZE>, SIZE>& getBoard() const;

  bool canMove(const Move& m) const;

  void move(const Move& m);

  bool operator==(const Board& b) const;

  std::array<std::array<int, SIZE>, SIZE> score(std::vector<pos> locations) const;

  int size() const { return SIZE; }

  bool inBounds(pos position) const;

  size_t getHash(const Move& m) const;

  size_t getHash() const;

  std::array<std::array<int, 3>, 3> getPattern(pos p) const;

  // check a box around p if its empty
  bool isEmpty(pos p, int s = 2) const;

  std::vector<pos> fixAtari(pos p, int playerColor) const;

  template<class Handler>
  bool floodfill(pos position, int color, Handler h) const {
    if (color == 0) {
      return false;
    }

    std::array<pos, SIZE * SIZE> s;
    int ssize = 0;

    s[ssize++] = position;

    std::array<std::array<bool, SIZE>, SIZE> visited = {false};
    visited[position.first][position.second] = true;

    while (ssize > 0) {
      pos next = s[--ssize];

      int a = next.first;
      int b = next.second;
      h.handlePiece(a, b);

      for (int i = 0; i < 4; ++i) {
        int x = a + dirs[0][i];
        int y = b + dirs[1][i];
        if (!inBounds({x, y}) || visited[x][y]) {
          continue;
        }

        if (board_[x][y] == 0) {
          if (h.handleLiberty(a, b)) {
            return false;
          }
        } else if (board_[x][y] == color) {
          s[ssize++] = {x, y};
        } else {
          h.handleOpp(a, b);
        }
        visited[x][y] = true;
      }
    }

    return true;
  }

  template<class Test>
  static bool placeAndTest(const Board& board, std::vector<Move> moves, Test test) {
    for (auto& m : moves) {
      auto p = m.getCoor();
      if (m.isResign() || m.isPass() || board.board_[p.first][p.second] != 0) {
        return false;
      }
    }

    for (auto& m : moves) {
      auto p = m.getCoor();
      board.board_[p.first][p.second] = m.getColor();
    }

    bool result = test(board);

    for (auto& m : moves) {
      auto p = m.getCoor();
      board.board_[p.first][p.second] = 0;
    }

    return result;
  }
};

std::ostream& operator<<(std::ostream& s, const Board& b);

namespace std {
template<>
struct hash<Board> {
  size_t operator()(const Board& board) const {
    return board.getHash();
  }
};
}

namespace std {
template<>
struct hash<std::pair<int, Board>> {
  size_t operator()(const pair<int, Board>& state) const {
    return state.first + hash<Board>{}(state.second) * 31;
  }
};
}