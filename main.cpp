#include "Game.h"
#include "CommandLinePlayer.h"
#include <memory>
#include <vector>

using std::vector;
using std::shared_ptr;

int main() {
	auto p = std::make_shared<CommandLinePlayer>(1);
	auto q = std::make_shared<CommandLinePlayer>(2);
	vector<shared_ptr<Player>> players{ shared_ptr<Player>(p), shared_ptr<Player>(q) };
	vector<double> komi{ 0, 7.5 };
	Game g(players, komi);

	g.run();
}