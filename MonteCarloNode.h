#pragma once

#include "Board.h"
#include "RandomPlayout.h"
#include <random>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <vector>


struct MonteCarloNode {
private:
    const int kN = 5; // visit 5 times before expanding
    static std::default_random_engine gen_;
    static std::uniform_int_distribution<int> dist_;

    int w_; // wins for last player
    int n_; // number of times visited
    int pn_; // prior number of times visited
    int p_; // last player (init by parent)
    std::unordered_map<Move, std::shared_ptr<MonteCarloNode>> next_;

public:
    MonteCarloNode() : w_(0), n_(0), pn_(0), p_(0), next_() {}

    double uct(int t);

    Move move();

    double winPercentage(Move& m) {
        auto& x = *next_[m];
        return x.w_ * 1.0 / x.n_;
    }

    void initNext(const Board& board, int player, const std::vector<double>& komi, std::pair<Move, Move> last,
                  std::map<int, std::unordered_set<size_t>>& history);

    int select(Board& board, int player, const std::vector<double>& komi, std::pair<Move, Move> last,
               std::map<int, std::unordered_set<size_t>>& history);

    static std::array<std::array<int, SIZE>, SIZE> cfgDistance(const Board& board, const Move& m);
};
