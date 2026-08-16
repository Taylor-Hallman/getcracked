#include <unordered_map>
#include <set>
#include <string>
#include <optional>

class ScoreBoard {
private:
    std::unordered_map<std::string, std::multiset<long long, std::greater<long long>>> m_gameScores;
    std::unordered_map<long long, std::pair<
        std::unordered_map<std::string, std::multiset<long long, std::greater<long long>>>::iterator, 
        std::multiset<long long>::iterator>> 
    m_scores;
public:
    void submitScore(long long ticketId, std::string game, long long score) {
        auto multiset_itr = m_gameScores[game].insert(score);
        auto map_itr = m_gameScores.find(game);
        m_scores[ticketId] = std::make_pair(map_itr, multiset_itr);
    }

    void revokeScore(long long ticketId) {
        if (!m_scores.contains(ticketId))
            return;
        auto [map_itr, multiset_itr] = m_scores.at(ticketId);
        map_itr->second.erase(multiset_itr);
    }

    std::optional<long long> topScore(std::string game) {
        if (!m_gameScores.contains(game) || !m_gameScores.at(game).size())
            return std::nullopt;
        return *(m_gameScores.at(game).begin());
    }
};
