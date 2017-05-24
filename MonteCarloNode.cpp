#include "MonteCarloNode.h"
#include <random>
#include "RandomPlayout.h"
#include <iostream>
#include <math.h>

using namespace std;

//idek if these numbers are right
default_random_engine MonteCarloNode::gen_(1776);
uniform_int_distribution<int> MonteCarloNode::dist_(0, 10000);

double MonteCarloNode::uct(int t) {
    if (n_ == 0) {
        return 10000; //TODO: change later
    }

    return ((double) w_) / n_ + 0.45 * std::sqrt(std::log(t) / n_);
}

Move MonteCarloNode::move() {
    auto element = std::max_element(next_.begin(), next_.end(), [](const auto& a, const auto& b) {
        return a.second->n_ - a.second->pn_ < b.second->n_ - b.second->pn_;
    });

    return element->first;
}

void MonteCarloNode::initNext(const Board& board, int player, const vector<double>& komi, std::pair<Move, Move> last,
                              std::map<int, std::unordered_set<size_t>>& history) {
    const auto& playerHistory = history[((player + 1) % 2)];
    vector<Move> moves = board.getValidMoves(player + 1, playerHistory);

    //for each move, add to next
    auto cfgMap = cfgDistance(board, last.second);

    for (const auto& it : moves) {
        next_[it] = std::make_shared<MonteCarloNode>();
        next_[it]->p_ = (player + 1) % 2;
        next_[it]->w_ = 5;
        next_[it]->pn_ = 10;
        if (it.isPass()) {
            next_[it]->pn_ += 100;
            continue;
        }

        pos p = it.getCoor();
        if (!last.second.isPass()) {
            int dist = cfgMap[p.first][p.second] - 1;
            if (dist < 3) {
                if (dist == 2) {
                    next_[it]->w_ += 8;
                    next_[it]->pn_ += 8;
                } else if (dist == 1) {
                    next_[it]->w_ += 22;
                    next_[it]->pn_ += 22;
                } else {
                    next_[it]->w_ += 24;
                    next_[it]->pn_ += 24;
                }
            }
        }

        int height = std::min(std::min(p.first, p.second), std::min(SIZE - 1 - p.first, SIZE - 1 - p.second));
        if (height <= 2 && board.isEmpty(p)) {
            if (height <= 1) {
                next_[it]->pn_ += 10;
            } else {
                next_[it]->w_ += 10;
                next_[it]->pn_ += 10;
            }
        }
    }

    for (const auto& it : moves) {
        next_[it]->n_ = next_[it]->pn_;
    }
}

int MonteCarloNode::select(
        Board& board,
        int player,
        const vector<double>& komi,
        std::pair<Move, Move> last,
        map<int, unordered_set<size_t>>& history) {
    if (n_ < kN + pn_) {
        RandomPlayout rp(komi);
        int winner = rp.simulate(board, player, last, history);
        if (p_ == winner) {
            w_++;
        }

        n_++;
        return winner;
    }

    if (n_ == kN + pn_) {
        initNext(board, player, komi, last, history);
    }

    if (next_.empty()) {
        int winner = player;
        if (p_ == winner) {
            w_++;
        }

        n_++;
        return winner;
    }

    auto element = max_element(next_.begin(), next_.end(), [this](const auto& a, const auto& b) {
        return a.second->uct(n_) < b.second->uct(n_);
    });

    board.move(element->first);
    history[((player + 1) % 2)].insert(board.getHash());
    int winner = element->second->select(board, (player + 1) % 2, komi, { last.second, element->first }, history);

    if (winner == p_) {
        w_++;
    }
    n_++;
    return winner;
}

std::array<std::array<int, SIZE>, SIZE> MonteCarloNode::cfgDistance(const Board& board, const Move& m) {
    // common fate graph
    std::array<std::array<int, SIZE>, SIZE> cfgMap;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cfgMap[i][j] = -1;
        }
    }

    if (m.isPass()) {
        return cfgMap;
    }

    pos p = m.getCoor();
    cfgMap[p.first][p.second] = 0;

    std::vector<pos> fringe;
    fringe.push_back(p);
    while (!fringe.empty()) {
        pos c;
        c = fringe.back();
        fringe.pop_back();
        for (int i = 0; i < 4; i++) {
            int x = Board::dirs[0][i];
            int y = Board::dirs[1][i];
            pos d = {c.first + x, c.second + y};
            if (!board.inBounds(d) || (0 <= cfgMap[d.first][d.second] && cfgMap[d.first][d.second] <= cfgMap[c.first][c.second])) {
                continue;
            }
            int before = cfgMap[d.first][d.second];
            const auto& b = board.getBoard();
            if (b[d.first][d.second] != 0 && b[d.first][d.second] == b[c.first][c.second]) {
                cfgMap[d.first][d.second] = cfgMap[c.first][c.second];
            } else {
                cfgMap[d.first][d.second] = cfgMap[c.first][c.second] + 1;
            }

            if (before < 0 || before > cfgMap[d.first][d.second]) {
                fringe.push_back(d);
            }
        }
    }

    return cfgMap;
}
