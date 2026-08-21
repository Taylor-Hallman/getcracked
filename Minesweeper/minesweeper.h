#include <vector>
#include <string>
#include <unordered_set>
#include <stack>
#include <array>

namespace std {
    template<>
    struct hash<pair<int, int>> {
        size_t operator()(const pair<int, int>& p) const {
            return hash<int>()(p.first) ^ hash<int>()(p.second);
        }
    };
}

std::vector<std::vector<std::string>> minesweeper(const std::vector<std::vector<std::string>>& matrix) {
    constexpr std::array<std::pair<int, int>, 8> adj{ { {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1} } };

    std::vector<std::vector<std::string>> resultMatrix(matrix.size(), std::vector<std::string>(matrix[0].size()));

    std::unordered_set<std::pair<int, int>> visited;
    std::stack<std::pair<int, int>> s;
    s.emplace(0, 0);
    while (!s.empty()) {
        auto [y, x] = s.top();
        s.pop();
        if (!visited.emplace(y, x).second)
            continue;

        bool isMine{ matrix[y][x] == "*" };

        int mines{ 0 };
        for (auto [deltaY, deltaX] : adj) {
            int adjY{ y + deltaY };
            int adjX{ x + deltaX };
            if (adjY < 0 || adjY >= matrix.size() || adjX < 0 || adjX >= matrix[0].size())
                continue;
            if (matrix[adjY][adjX] == "*")
                ++mines;
            s.emplace(adjY, adjX);
        }
        resultMatrix[y][x] = isMine ? "*" : std::to_string(mines);
    }

    return resultMatrix;
}
