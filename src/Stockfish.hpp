#pragma once

#include <string>
#include <vector>

#include "../Config.hpp"
#include "StockfishInput.hpp"

enum class StockfishElo {
    Elo1320 = 1320,
    Elo1450 = 1450,
    Elo1600 = 1600,
    Elo1750 = 1750,
    Elo1950 = 1950,
    Elo2200 = 2200,
    Elo2550 = 2550,
    Elo3190 = 3190
};

class Stockfish {
public:
    Stockfish(StockfishElo elo, const std::string& executablePath =
    #ifdef _WIN32
        "stockfish.exe"
    #else
        "stockfish"
    #endif
    );

    void setElo(StockfishElo newElo);
    void newGame();
    std::string move(const std::vector<std::string>& moveHistory);

private:
    void waitFor(const std::string& token);

    StockfishInput inputHandler;
    StockfishElo elo;
};
