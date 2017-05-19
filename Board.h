#pragma once

#include "Move.h"
#include <array>
#include <ostream>
#include <vector>

const int SIZE = 9;

struct Board {
private:
	const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };

	mutable std::array<std::array<int, SIZE>, SIZE> board_;
public:
	Board();
	
	bool isSuicide(const Move& move) const;

	int liberties(pos p) const;

	std::vector<pos> getCaptured(pos position, int color) const;

	const std::array<std::array<int, SIZE>, SIZE>& getBoard() const;

	bool canMove(const Move& m) const;

	void move(const Move& m);

	bool operator==(const Board& b) const;

	std::array<std::array<int, SIZE>, SIZE> score(std::vector<pos> locations) const;

	int size() const { return SIZE; }

	bool inBounds(pos position) const;

	template <class Test>
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
		size_t operator()(const Board &board) const {
			auto& b = board.getBoard();
			int h = 0;
			for (size_t i = 0; i < b.size(); i++) {
				for (size_t j = 0; j < b[i].size(); j++) {
					h *= 31;
					h += b[i][j];
				}
			}

			return h;
		}
	};
}