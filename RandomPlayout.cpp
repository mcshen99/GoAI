#include "RandomPlayout.h"
#include "Gtp.h"
#include <deque>

using namespace std;

const int RandomPlayout::dirs[2][4] = {{1, -1, 0, 0}, {0, 0, 1, -1}};

RandomPlayout::RandomPlayout(vector<double> komi) : komi_(komi) {}

Move RandomPlayout::move(
    const Board& board,
    int color,
    const unordered_set<size_t>& history,
    const std::vector<Move>& lastMoves) {
  vector<pos> heuristicMoves;
  heuristicMoves.reserve(18);
  for (auto& m : lastMoves) {
    int x = m.getCoor().first;
    int y = m.getCoor().second;
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        heuristicMoves.emplace_back(x + i, y + j);
      }
    }
  }

  MoveGenerator moveGenerator(board, color, heuristicMoves);
  for (auto p = moveGenerator.next(); !p.first; p = moveGenerator.next()) {
    Move m = p.second;
    if (isOkMove(board, m, history)) {
      return m;
    }
  }

  return Move::pass(color);
}

int RandomPlayout::simulate(
    Board& board,
    int player,
    std::pair<Move, Move> last,
    array<unordered_set<size_t>, 2>& history) {
  //if 2 players pass, game ends
  //need scoring phase, everything should be alive (don't put in locations), don't mark dead, add komi (constructor)
  bool lastPass = false;

  deque<Move> lastMoves;
  if (!last.first.isPass()) {
    lastMoves.push_back(last.first);
  }

  if (!last.second.isPass()) {
    lastMoves.push_back(last.second);
  }

  for (int count = 0; count < SIZE * SIZE * 3 / 2; ++count) {
    Move m = move(board, player + 1, history[(player + 1) % 2], {lastMoves.begin(), lastMoves.end()});
    if (m.isPass()) {
      if (lastPass) {
        break;
      } else {
        lastPass = true;
      }
    } else {
      lastPass = false;
      board.move(m);
      lastMoves.push_back(m);
      history[(player + 1) % 2].insert(board.getHash());
      if (lastMoves.size() > 2) {
        lastMoves.pop_front();
      }
    }
    player = (player + 1) % 2;
  }

  //do score here and return higher one
  vector<pos> locations;
  array<array<int, SIZE>, SIZE> territoryBoard = board.score(locations);
  vector<double> scores = komi_;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      int c = territoryBoard[i][j];
      if (c != 0) {
        scores[c - 1]++;
      }
    }
  }

  double maxScore = -1;
  int winner = -1;
  for (int i = 0; i < scores.size(); ++i) {
    if (scores[i] > maxScore) {
      maxScore = scores[i];
      winner = i;
    }
  }

  return winner;
}

bool RandomPlayout::isGroup(const Board& board, const Move& m) {
  const int dirs[2][4] = {{1, -1, 0, 0}, {0, 0, 1, -1}};

  pos p = m.getCoor();
  int c = m.getColor();

  bool group = false;
  for (int i = 0; i < 4; ++i) {
    int x = p.first + dirs[0][i];
    int y = p.second + dirs[1][i];
    if (board.inBounds({x, y}) && board.getBoard()[x][y] == c) {
      group = true;
      break;
    }
  }

  return group;
}

bool RandomPlayout::isAtari(const Board& board, const Move& m) {
  return Board::placeAndTest(
      board, {m}, [&m](const Board& b) {
        return b.hasLiberties(m.getCoor(), 1);
      });
}

bool RandomPlayout::isCapture(const Board& board, const Move& m) {
  return Board::placeAndTest(
      board, {m}, [&m](const Board& b) {
        const int dirs[2][4] = {{1, -1, 0, 0}, {0, 0, 1, -1}};

        int c = m.getCoor().first;
        int d = m.getCoor().second;
        for (int i = 0; i < 4; ++i) {
          int x = c + dirs[0][i];
          int y = d + dirs[1][i];
          if (!b.inBounds({x, y})) {
            continue;
          }
          if (b.getBoard()[x][y] == m.getColor()) {
            continue;
          }
          auto captured = b.getCaptured({x, y}, b.getBoard()[x][y]);
          if (!captured.empty()) {
            return true;
          }
        }

        return false;
      });
}

bool RandomPlayout::isEyeFilling(const Board& board, const Move& m) {
  int a = m.getCoor().first;
  int b = m.getCoor().second;

  for (int i = 0; i < 4; ++i) {
    int x = a + dirs[0][i];
    int y = b + dirs[1][i];

    if (!board.inBounds({x, y})) {
      continue;
    }

    if (board.getBoard()[x][y] != m.getColor()) {
      return false;
    }
  }

  bool center = true;
  int count = 0;
  for (int i = -1; i < 1; i++) {
    for (int j = -1; j < 1; j++) {
      int x = a + i;
      int y = b + j;
      if (!board.inBounds({x, y})) {
        center = false;
        continue;
      }

      if (board.getBoard()[x][y] != m.getColor()) {
        count++;
      }
    }
  }

  // is eye if middle and at most zero-one neighbors are empty
  return count <= 1 + center;
}

bool RandomPlayout::isOkMove(const Board& board, const Move& m, const unordered_set<size_t>& history) {
  return board.canMove(m) && history.find(board.getHash(m)) == history.end() && !isEyeFilling(board, m)
      && !(isGroup(board, m) && isAtari(board, m) && !isCapture(board, m));
}
