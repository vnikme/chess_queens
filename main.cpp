#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>


class TState {
    public:
        TState() {
            Players[0] = Players[1] = 0;
        }

        void Put(size_t player, int a, int b) {
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            Players[player] |= (1ul << (a * 8 + b));
        }

        unsigned long long GetPlayerPosition(size_t player) const {
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            return Players[player];
        }

        void Print() const {
            for (size_t i = 0; i < 8; ++i) {
                for (size_t j = 0; j < 8; ++j) {
                    if (Players[0] & (1ul << (i * 8 + j)))
                        std::cout << 'w';
                    else if (Players[1] & (1ul << (i * 8 + j)))
                        std::cout << 'b';
                    else
                        std::cout << '.';
                }
                std::cout << std::endl;
            }
        }

        int Compare(const TState &rgt) const {
            for (size_t i = 0; i < 2; ++i) {
                if (Players[i] != rgt.Players[i])
                    return Players[i] < rgt.Players[i] ? -1 : 1;
            }
            return 0;
        }

        bool operator < (const TState &rgt) const {
            return Compare(rgt) < 0;
        }

        bool operator == (const TState &rgt) const {
            return Compare(rgt) == 0;
        }

        size_t IsEmpty(size_t player) const {
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            return Players[player] == 0;
        }

        std::vector<TState> AllMoves(size_t player) const {
            std::vector<TState> result;
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            for (size_t figure = 0; figure < 64; ++figure) {
                if ((Players[player] & (1ul << figure)) == 0)
                    continue;
                for (int da = -1; da <= 1; ++da)
                    for (int db = -1; db <= 1; ++db)
                        if (da != 0 || db != 0)
                            AddMoves(result, player, figure, da, db);
            }
            return result;
        }

    private:
        unsigned long long Players[2];

        void AddMoves(std::vector<TState> &moves, size_t player, size_t figure, int directionA, int directionB) const {
            TState obj(*this);
            obj.Players[player] &= ~(1ul << figure);
            int x = figure / 8, y = figure % 8;
            for (int i = 1; ; ++i) {
                int a = x + i * directionA, b = y + i * directionB;
                if (a < 0 || a >= 8 || b < 0 || b >= 8)
                    break;                                          // Out of field
                if (obj.Players[player] & (1ul << (a * 8 + b)))
                    break;                                          // Same color in this direction
                TState tmp(obj);
                tmp.Put(player, a, b);
                bool hasOpposite = (tmp.Players[1 - player] & (1ul << (a * 8 + b)));
                if (hasOpposite)                                    // Beats opposite
                    tmp.Players[1 - player] &= ~(1ul << (a * 8 + b));
                moves.push_back(tmp);
                if (hasOpposite)
                    break;
            }
        }
};


namespace std {
    template <>
    struct hash<TState>
    {
        size_t operator()(const TState &s) const {
            return s.GetPlayerPosition(0) ^ s.GetPlayerPosition(1);
        }
    };
}

//using TAssocArray = std::map<TState, int>;
using TAssocArray = std::unordered_map<TState, int>;


TState ConstructState(const std::vector<size_t> &p0s, size_t p1) {
    TState result;
    result.Put(1, p1 / 8, p1 % 8);
    for (size_t p0 : p0s)
        result.Put(0, p0 / 8, p0 % 8);
    return result;
}

void DoGenerateAllPositions(std::vector<size_t> p0s, size_t p1, size_t p0Start, size_t p0Count, std::vector<TState> &result) {
    if (p0Count == 0) {
        auto pos = ConstructState(p0s, p1);
        result.push_back(pos);
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
    for (size_t p1 = 0; p1 < 64; ++p1) {
        for (size_t i = 0; i <= p0; ++i) {
            DoGenerateAllPositions(std::vector<size_t>(), p1, 0, i, result);
        }
    }
    return result;
}

bool IsTerminalExist(const TState &pos) {
    const auto &moves = pos.AllMoves(0);
    for (const auto &move : moves) {
        if (move.IsEmpty(1))
            return true;
    }
    return false;
}

int MaxDistanceToSet(const TState &pos, const TAssocArray &positions) {
    int result = -1;
    const auto &moves = pos.AllMoves(1);
    for (const auto &move : moves) {
        const auto it = positions.find(move);
        if (it == positions.end())
            return -1;
        if (it->second > result)
            result = it->second;
    }
    return result;
}

int MinDistanceToSet(const TState &pos, const TAssocArray &positions) {
    int result = -1;
    const auto &moves = pos.AllMoves(0);
    for (const auto &move : moves) {
        const auto it = positions.find(move);
        if (it == positions.end())
            continue;
        if (result == -1 || it->second < result)
            result = it->second;
    }
    return result;
}

void BuildPath(TState pos, const TAssocArray &dist0, const TAssocArray &dist1) {
    for (;;) {
        pos.Print();
        std::cout << std::endl;
        const auto it = dist0.find(pos);
        if (it == dist0.end())
            throw std::logic_error("Can't build path");
        int length0 = it->second;
        if (length0 == 0)
            break;
        for (const auto &move : pos.AllMoves(0)) {
            const auto it = dist1.find(move);
            if (it != dist1.end() && it->second == length0) {
                pos = it->first;
                break;
            }
        }
        pos.Print();
        std::cout << std::endl;
        for (const auto &move : pos.AllMoves(1)) {
            const auto it = dist0.find(move);
            if (it != dist0.end() && it->second + 1 == length0) {
                pos = it->first;
                break;
            }
        }
    }
}

void SearchPaths(const std::vector<TState> &positions, TAssocArray &dist0, TAssocArray &dist1) {
    for (const auto &pos : positions) {
        if (IsTerminalExist(pos)) {
            dist0[pos] = 0;
        }
    }
    std::cout << dist0.size() << std::endl;
    int maxDist = -1;
    TState maxPos;
    for (;;) {
        size_t oldSize0 = dist0.size();
        for (const auto &pos : positions) {
            int posDist = MaxDistanceToSet(pos, dist0);
            if (posDist != -1) {
                dist1[pos] = posDist + 1;
            }
        }
        std::cout << dist1.size() << std::endl;
        for (const auto &pos : positions) {
            if (dist0.find(pos) != dist0.end())
                continue;
            int posDist = MinDistanceToSet(pos, dist1);
            if (posDist != -1) {
                dist0[pos] = posDist;
                if (posDist > maxDist) {
                    maxDist = posDist;
                    maxPos = pos;
                }
            }
        }
        std::cout << dist0.size() << std::endl;
        if (oldSize0 == dist0.size())
            break;
    }
    //BuildPath(maxPos, dist0, dist1);
}

void DumpDistances(const std::vector<TState> &positions, TAssocArray &dist0, TAssocArray &dist1) {
    std::ofstream fout("dist.txt");
    fout << positions.size() << std::endl;
    for (const auto &pos : positions) {
        fout << pos.GetPlayerPosition(0) << ' ' << pos.GetPlayerPosition(1);
        const auto it0 = dist0.find(pos);
        const auto it1 = dist1.find(pos);
        fout << ' ' << (it0 != dist0.end() ? it0->second : -1) << ' ' << (it1 != dist1.end() ? it1->second : -1) << std::endl;
    } 
}

void PrintUnreachable(const std::vector<TState> &positions, const TAssocArray &dist0) {
    for (const auto &pos : positions) {
        if (dist0.find(pos) == dist0.end()) {
            pos.Print();
            std::cout << std::endl;
        }
    }
}

int main() {
    auto positions = GenerateAllPositions(3);
    std::cout << positions.size() << std::endl;
    TAssocArray dist0, dist1;
    SearchPaths(positions, dist0, dist1);
    //DumpDistances(positions, dist0, dist1);
    //PrintUnreachable(positions, dist0);
    return 0;
}

