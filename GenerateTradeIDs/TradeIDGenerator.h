#include <print>
#include <sstream>
#include <string>
#include <vector>
#include <random>

struct matchedTrade {
    long long passiveID;
    long long aggressorID;
    std::string passiveTime;
    std::string aggressorTime;
    std::string instrument;
};

int timeToSeconds(const std::string& time) {
    std::istringstream iss{ time };
    std::vector<std::string> timeParts(3);
    auto i{0uz};
    while (std::getline(iss, timeParts[i++], ':'));
    return std::stoi(timeParts[0]) * 3600 + std::stoi(timeParts[1]) * 60 + std::stoi(timeParts[2]);
}

// Implement this
std::string generateTradeID(const matchedTrade& tradeDetails) {
    std::random_device rd;
    std::mt19937 g(rd());

    std::string tradeId = "";
    int diff{ timeToSeconds(tradeDetails.aggressorTime) - timeToSeconds(tradeDetails.passiveTime) };
    tradeId += std::to_string(diff) + '-';

    std::string sequence{ std::to_string(tradeDetails.passiveID).substr(0, 3) + std::to_string(tradeDetails.aggressorID).substr(0, 3) };
    std::shuffle(sequence.begin(), sequence.end(), g);
    tradeId += sequence + '-';

    for (const char& c : tradeDetails.instrument) {
        tradeId += isalpha(c) ? std::to_string(tolower(c) - 'a' + 1) : std::to_string(c - '0'); 
    }

    return tradeId;
}
