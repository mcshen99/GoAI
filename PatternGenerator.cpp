#include <iostream>
#include "PatternGenerator.h"

using namespace std;

const vector<PatternGenerator::hood> PatternGenerator::PATTERNS_SOURCE = {
    // hane pattern - enclosing hane
    {
        "XOX",
        "...",
        "???"
    },
    // hane pattern - non-cutting hane
    {
        "XO.",
        "...",
        "?.?"
    },
    // hane pattern - magari
    {
        "XO?",
        "X..",
        "x.?"
    },
    // generic pattern - katatsuke or diagonal attachment; similar to magari
    {
        ".O.",
        "X..",
        "..."
    },
    // cut1 pattern (kiri] - unprotected cut
    {
        "XO?",
        "O.o",
        "?o?"
    },
    // cut1 pattern (kiri] - peeped cut
    {
        "XO?",
        "O.X",
        "???"
    },
    // cut2 pattern (de]
    {
        "?X?",
        "O.O",
        "ooo"
    },
    // cut keima
    {
        "OX?",
        "o.O",
        "???"
    },
    // side pattern - chase
    {
        "X.?",
        "O.?",
        "   "
    },
    // side pattern - block side cut
    {
        "OX?",
        "X.O",
        "   "
    },
    // side pattern - block side connection
    {
        "?X?",
        "x.O",
        "   "
    },
    // side pattern - sagari
    {
        "?XO",
        "x.x",
        "   "
    },
    // side pattern - cut
    {
        "?OX",
        "X.O",
        "   "
    },

};

const unordered_set<array<array<int, 3>, 3>, PatternHash> PatternGenerator::PATTERNS = genPatterns(PATTERNS_SOURCE);

unordered_set<array<array<int, 3>, 3>, PatternHash>
PatternGenerator::genPatterns(const vector<PatternGenerator::hood>& source) {
  vector<PatternGenerator::hood> pats = source;
  pats = expandWildcards(pats, '?', {'.', 'X', 'O', ' '});
  pats = expandWildcards(pats, 'x', {'.', 'O', ' '});
  pats = expandWildcards(pats, 'o', {'.', 'X', ' '});
  pats = apply(pats, rotate90);
  pats = apply(pats, flipVertical);
  pats = apply(pats, flipHorizontal);
  pats = apply(pats, swapColors);

  unordered_set<array<array<int, 3>, 3>, PatternHash> patterns;
  for (const auto& h : pats) {
    patterns.insert(convert(h));
  }
  return patterns;
}

PatternGenerator::hood PatternGenerator::rotate90(const PatternGenerator::hood& p) {
  return {
      string() + p[2][0] + p[1][0] + p[0][0],
      string() + p[2][1] + p[1][1] + p[0][1],
      string() + p[2][2] + p[1][2] + p[0][2]
  };
}

PatternGenerator::hood PatternGenerator::flipVertical(const PatternGenerator::hood& p) {
  return {p[2], p[1], p[0]};
}

PatternGenerator::hood PatternGenerator::flipHorizontal(const PatternGenerator::hood& p) {
  return {
      string() + p[0][2] + p[0][1] + p[0][0],
      string() + p[1][2] + p[1][1] + p[1][0],
      string() + p[2][2] + p[2][1] + p[2][0]
  };
}

PatternGenerator::hood PatternGenerator::swapColors(const PatternGenerator::hood& p) {
  PatternGenerator::hood h;
  int count = 0;
  for (const string& s : p) {
    string t = s;
    replace(t.begin(), t.end(), 'X', 'Z');
    replace(t.begin(), t.end(), 'O', 'X');
    replace(t.begin(), t.end(), 'Z', 'O');
    // maybe unnecessary, but w/e.
    replace(t.begin(), t.end(), 'x', 'z');
    replace(t.begin(), t.end(), 'o', 'x');
    replace(t.begin(), t.end(), 'z', 'o');
    h[count++] = t;
  }

  return h;
}

vector<PatternGenerator::hood>
PatternGenerator::expandWildcards(const PatternGenerator::hood& p, char c, const vector<char>& exp) {
  vector<PatternGenerator::hood> h;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (p[i][j] == c) {
        for (char e : exp) {
          h.push_back(p);
          h.back()[i][j] = e;
        }

        return h;
      }
    }
  }

  // no wildcards, just return as is
  h.push_back(p);
  return h;
}

vector<PatternGenerator::hood> PatternGenerator::expandWildcards(
    const vector<PatternGenerator::hood>& p,
    char c,
    const vector<char>& exp) {
  vector<PatternGenerator::hood> pat;
  for (const auto& h : p) {
    auto v = expandWildcards(h, c, exp);
    for (const auto& hh : v) {
      pat.push_back(hh);
    }
  };

  if (pat.size() == p.size()) {
    return pat;
  } else {
    return expandWildcards(pat, c, exp);
  }
}

std::array<std::array<int, 3>, 3> PatternGenerator::convert(const PatternGenerator::hood& h) {
  std::array<std::array<int, 3>, 3> pats;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (h[i][j] == 'X') {
        pats[i][j] = 1;
      } else if (h[i][j] == 'O') {
        pats[i][j] = 2;
      } else if (h[i][j] == ' ') {
        pats[i][j] = -1;
      } else {
        pats[i][j] = 0;
      }
    }
  }

  return pats;
}

std::pair<bool, Move> PatternGenerator::next() {
  while (m_ < moves_.size()) {
    pos p = moves_[m_];

    if (board_.inBounds(p) && board_.getBoard()[p.first][p.second] == 0) {
      auto pat = board_.getPattern(p);
      if (PATTERNS.find(pat) != PATTERNS.end()) {
        m_++;
        return {false, Move::move(p, color_)};
      }
    }

    m_++;
  }

  return {true, Move::pass(color_)};
}

PatternGenerator::PatternGenerator(const Board& b, int color, const std::vector<pos>& moves) :
    board_(b), color_(color), moves_(moves), m_(0) {}
