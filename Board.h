#pragma once

#include "Move.h"
#include <array>
#include <ostream>
#include <vector>

typedef std::pair<int, int> pos;

const int SIZE = 9;

struct Board {
private:
	const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };
	std::array<std::array<int, SIZE>, SIZE> board_;

	bool inBounds(pos position);

	std::vector<pos> getCaptured(pos position, int color);

	bool isSuicide(const Move& move);

public:
	Board();

	const std::array<std::array<int, SIZE>, SIZE>& getBoard() const;

	bool canMove(const Move& m);

	void move(const Move& m);

	bool operator==(const Board& b) const;

	std::array<std::array<int, SIZE>, SIZE> score(std::vector<pos> locations);
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