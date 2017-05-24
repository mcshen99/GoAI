#include <memory>
#include <vector>

#include "Game.h"
#include "CommandLinePlayer.h"
#include "MonteCarloPlayer.h"
#include "Gtp.h"
#include "RandomPlayoutPlayer.h"

using std::vector;
using std::shared_ptr;

int main() {
	vector<double> komi{ 0, 7.5 };
	auto p = std::make_shared<CommandLinePlayer>(1);
	auto q = std::make_shared<MonteCarloPlayer>(2, komi, 10000, 500);
	vector<shared_ptr<Player>> players{ shared_ptr<Player>(p), shared_ptr<Player>(q) };
//	Game g(players, komi);
//
//	g.run();

    auto r1 = std::make_shared<MonteCarloPlayer>(1, komi, 40000);
    auto r2 = std::make_shared<MonteCarloPlayer>(2, komi, 40000);
	vector<shared_ptr<Player>> gtpPlayers{ shared_ptr<Player>(r1), shared_ptr<Player>(r2) };
	//Gtp
	Gtp gtp(gtpPlayers);
	gtp.run();
}