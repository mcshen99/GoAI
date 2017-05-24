#pragma once

#include <array>
#include <vector>
#include <string>
#include <set>
#include "Move.h"
#include "Board.h"

class PatternGenerator {
public:
    std::pair<bool, Move> next();
    PatternGenerator(const Board& b, int color, const std::vector<pos>& moves);

private:
    const Board& board_;
    int color_;
    std::vector<pos> moves_;

    int m_;

    typedef std::array<std::string, 3> hood;
    static std::set<std::array<std::array<int, 3>, 3>> genPatterns(const std::vector<hood>& source);

    template <class F>
    static std::vector<hood> apply(const std::vector<hood>& p, F f) {
        std::vector<hood> pat;
        for (const auto& h : p) {
            pat.push_back(f(h));
            pat.push_back(h);
        }

        return pat;
    }

    static hood rotate90(const hood& p);
    static hood flipVertical(const hood& p);
    static hood flipHorizontal(const hood& p);
    static hood swapColors(const hood& p);
    static std::vector<hood> expandWildcards(const hood& p, char c, const std::vector<char>& exp);
    static std::vector<hood> expandWildcards(const std::vector<hood>& p, char c, const std::vector<char>& exp);
    static std::array<std::array<int, 3>, 3> convert(const hood& h);

    // source patterns, X = 1, O = 2, x = !1, o = !2, ' ' = -1
    static const std::vector<hood> PATTERNS_SOURCE;
    // 1 is current player, -1 is edge of board
    static const std::set<std::array<std::array<int, 3>, 3>> PATTERNS;
};

