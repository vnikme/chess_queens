#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>


class TState {
    public:
        void Put(size_t player, int a, int b) {
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            Players[player].emplace_back(a, b);
            SortUnique(Players[player]);
        }

        void Print() const {
            Print(Players[0]);
            Print(Players[1]);;
        }

        int Compare(const TState &rgt) const {
            for (size_t i = 0; i < 2; ++i) {
                const auto &p = Players[i];
                const auto &rp = rgt.Players[i];
                if (p.size() != rp.size())
                    return p.size() < rp.size() ? -1 : 1;
                for (size_t i = 0, cnt = p.size(); i < cnt; ++i)
                    if (p[i] != rp[i])
                        return p[i] < rp[i] ? -1 : 1;
            }
            return 0;
        }

        bool operator < (const TState &rgt) const {
            return Compare(rgt) < 0;
        }

        size_t Count(size_t player) const {
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            return Players[player].size();
        }

        std::vector<TState> AllMoves(size_t player) const {
            std::vector<TState> result;
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            for (size_t figure = 0; figure < Players[player].size(); ++figure)
                for (int da = -1; da <= 1; ++da)
                    for (int db = -1; db <= 1; ++db)
                        if (da != 0 || db != 0)
                            AddMoves(result, player, figure, da, db);
            return result;
        }

    private:
        std::vector<std::pair<int, int>> Players[2];

        static int Check(const std::vector<std::pair<int, int>> &positions, int a, int b) {
            auto p = std::make_pair(a, b);
            auto it = std::lower_bound(positions.begin(), positions.end(), p);
            if (it == positions.end() || *it != p)
                return -1;
            return it - positions.begin();;
        }

        static void Print(const std::vector<std::pair<int, int>> &positions) {
            std::cout << positions.size();
            for (const auto &obj : positions)
                std::cout << " (" << obj.first << ", " << obj.second << ')';
            std::cout << std::endl;
        }

        static void SortUnique(std::vector<std::pair<int, int>> &positions) {
            std::sort(positions.begin(), positions.end());
            positions.erase(std::unique(positions.begin(), positions.end()), positions.end());
        }

        void AddMoves(std::vector<TState> &moves, size_t player, size_t figure, int directionA, int directionB) const {
            TState obj(*this);
            auto pos = obj.Players[player][figure];
            obj.Players[player].erase(obj.Players[player].begin() + figure);
            for (int i = 1; ; ++i) {
                int a = pos.first + i * directionA, b = pos.second + i * directionB;
                if (a < 0 || a >= 8 || b < 0 || b >= 8)
                    break;                                          // Out of field
                if (Check(obj.Players[player], a, b) != -1)
                    break;                                          // Same color in this direction
                TState tmp(obj);
                tmp.Put(player, a, b);
                int index = Check(tmp.Players[1 - player], a, b);
                if (index != -1)                                    // Beat opposite
                    tmp.Players[1 - player].erase(tmp.Players[1 - player].begin() + index);
                moves.push_back(tmp);
                if (index != -1)
                    break;
            }
        }
};


TState ConstructState(const std::vector<size_t> &p0s, int p1) {
    TState result;
    if (p1 != -1)
        result.Put(1, p1 / 8, p1 % 8);
    for (size_t p0 : p0s)
        result.Put(0, p0 / 8, p0 % 8);
    return result;
}

void DoGenerateAllPositions(std::vector<size_t> p0s, int p1, size_t p0Start, size_t p0Count, std::vector<TState> &result) {
    if (p0Count == 0) {
        result.push_back(ConstructState(p0s, p1));
        return;
    }
    for (size_t p0 = p0Start; p0 < 64; ++p0) {
        if (p0 == p1)
            continue;
        p0s.push_back(p0);
        DoGenerateAllPositions(p0s, p1, p0 + 1, p0Count - 1, result);
        p0s.pop_back();
    }
}

std::vector<TState> GenerateAllPositions(size_t p0) {
    std::vector<TState> result;
    for (int p1 = -1; p1 < 64; ++p1)
        for (size_t i = 0; i <= p0; ++i)
            DoGenerateAllPositions(std::vector<size_t>(), p1, 0, i, result);
    return result;
}

int main() {
    for (size_t i = 0; i <= 5; ++i) {
        auto positions = GenerateAllPositions(i);
        std::cout << i << ": " << positions.size() << std::endl;
    }
    return 0;
}

