#include <memory>
#include <vector>

#include "Game.h"
#include "CommandLinePlayer.h"
#include "MonteCarloPlayer.h"
#include "Gtp.h"

using std::vector;
using std::shared_ptr;

int main(int argc, char* argv[]) {
  std::string mode = "cli";
  bool debug = false;
  int sims = 3000;
  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg == "--mode") {
      if (i + 1 < argc) {
        mode = argv[i + 1];
      }
    } else if (arg == "--sims") {
      if (i + 1 < argc) {
        sims = std::stoi(argv[i + 1]);
      }
    } else if (arg == "--debug") {
      debug = true;
    }

  }

  vector<double> komi{0, 7.5};

  if (mode == "cli") {
    auto p = std::make_shared<CommandLinePlayer>(1);
    auto q = std::make_shared<MonteCarloPlayer>(2, komi, sims, 500);
    vector<shared_ptr<Player>> players{shared_ptr<Player>(p), shared_ptr<Player>(q)};
    Game g(players, komi);
    g.run();
  } else if (mode == "gtp") {
    auto r1 = std::make_shared<MonteCarloPlayer>(1, komi, sims);
    auto r2 = std::make_shared<MonteCarloPlayer>(2, komi, sims);
    vector<shared_ptr<Player>> gtpPlayers{shared_ptr<Player>(r1), shared_ptr<Player>(r2)};
    Gtp gtp(gtpPlayers, debug);
    gtp.run();
  } else if (mode == "prof") {
    auto p = std::make_shared<MonteCarloPlayer>(1, komi, sims);
    Board b;
    vector<Move> history;
    p->move(b, history);
    p->comment(std::cout);
  }

  return 0;
}