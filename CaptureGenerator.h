#pragma once

#include <array>
#include <vector>
#include <string>
#include <set>
#include "Move.h"
#include "Board.h"

class CaptureGenerator {
public:
    CaptureGenerator(const Board& b, int color, const std::vector<Move>& moves);
    std::pair<bool, Move> next();

private:
    const Board& board_;
    int color_;
    std::vector<Move> moves_;
    std::vector<pos> answers_;

    int m_;
};
