#include <memory>
#include <vector>

#include "Game.h"
#include "CommandLinePlayer.h"
#include "RandomPlayoutPlayer.h"
#include "Gtp.h"

using std::vector;
using std::shared_ptr;

int main() {
	vector<double> komi{ 0, 7.5 };
	auto p = std::make_shared<CommandLinePlayer>(1);
	auto q = std::make_shared<RandomPlayoutPlayer>(komi, 2);
	vector<shared_ptr<Player>> players{ shared_ptr<Player>(p), shared_ptr<Player>(q) };
//	Game g(players, komi);
//
//	g.run();

	// Gtp
	Gtp gtp(players);
	gtp.run();
}