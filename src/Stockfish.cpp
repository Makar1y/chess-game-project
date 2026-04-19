// STOCKFISH PIPELINE TIK WINDOWSAMS

#include "Stockfish.hpp"

#include <sstream>

Stockfish::Stockfish(StockfishElo elo, const std::string& executablePath)
    : inputHandler(executablePath), elo(elo) {
    inputHandler.sendCommand("uci");
    waitFor("uciok");

    inputHandler.sendCommand("setoption name UCI_LimitStrength value true");

    std::ostringstream eloCommand;
    eloCommand << "setoption name UCI_Elo value " << (int)elo;
    inputHandler.sendCommand(eloCommand.str());

    inputHandler.sendCommand("isready");
    waitFor("readyok");
}

void Stockfish::setElo(StockfishElo newElo) {
    elo = StockfishElo(newElo);

    std::ostringstream eloCommand;
    eloCommand << "setoption name UCI_Elo value " << (int)elo;
    inputHandler.sendCommand(eloCommand.str());
}

void Stockfish::newGame() {
    inputHandler.sendCommand("ucinewgame");
    inputHandler.sendCommand("isready");
    waitFor("readyok");
}

std::string Stockfish::move(const std::vector<std::string>& moveHistory) {
    std::ostringstream command;
    command << "position startpos";

    if (!moveHistory.empty()) {
        command << " moves";
        for (const std::string& move : moveHistory) {
            command << " " << move;
        }
    }

    inputHandler.sendCommand(command.str());
    inputHandler.sendCommand("go movetime " + std::to_string(STOCKFISH_MOVE_TIME_MS));

    while (true) {
        std::string line = inputHandler.readLine();

        if (line.rfind("bestmove ", 0) == 0) {
            std::istringstream stream(line);
            std::string keyword;
            std::string bestMove;
            stream >> keyword >> bestMove;
            return bestMove;
        }
    }
}

void Stockfish::waitFor(const std::string& token) {
    while (true) {
        std::string line = inputHandler.readLine();
        if (line.find(token) != std::string::npos) {
            return;
        }
    }
}
