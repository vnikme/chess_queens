#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <vector>


class TState {
    public:
        TState() {
            Players[0] = Players[1] = 0;
        }
        void Put(size_t player, int a, int b) {
            if (player > 1)
                throw std::logic_error("Player should be 0 or 1");
            Players[player] |= (1l << (a * 8 + b));
        }

        void Print() const {
            for (size_t i = 0; i < 8; ++i) {
                for (size_t j = 0; j < 8; ++j) {
                    if (Players[0] & (1l << (i * 8 + j)))
                        std::cout << 'w';
                    else if (Players[1] & (1l << (i * 8 + j)))
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
                if ((Players[player] & (1l << figure)) == 0)
                    continue;
                for (int da = -1; da <= 1; ++da)
                    for (int db = -1; db <= 1; ++db)
                        if (da != 0 || db != 0)
                            AddMoves(result, player, figure, da, db);
            }
            return result;
        }

    private:
        long long Players[2];

        void AddMoves(std::vector<TState> &moves, size_t player, size_t figure, int directionA, int directionB) const {
            TState obj(*this);
            obj.Players[player] &= ~(1l << figure);
            int x = figure / 8, y = figure % 8;
            for (int i = 1; ; ++i) {
                int a = x + i * directionA, b = y + i * directionB;
                if (a < 0 || a >= 8 || b < 0 || b >= 8)
                    break;                                          // Out of field
                if (obj.Players[player] & (1l << (a * 8 + b)))
                    break;                                          // Same color in this direction
                TState tmp(obj);
                tmp.Put(player, a, b);
                bool hasOpposite = (tmp.Players[1 - player] & (1l << (a * 8 + b)));
                if (hasOpposite)                                    // Beats opposite
                    tmp.Players[1 - player] &= ~(1l << (a * 8 + b));
                moves.push_back(tmp);
                if (hasOpposite)
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

bool IsTerminalExist(const TState &pos) {
    const auto &moves = pos.AllMoves(0);
    for (const auto &move : moves) {
        if (move.IsEmpty(1))
            return true;
    }
    return false;
}

int main() {
    auto positions = GenerateAllPositions(4);
    std::cout << positions.size() << std::endl;
    std::vector<TState> term;
    for (const auto &pos : positions) {
        if (!pos.IsEmpty(1) && IsTerminalExist(pos)) {
            term.push_back(pos);
        }
    }
    std::cout << term.size() << std::endl;
    for (const auto &pos : term) {
        pos.Print();
        std::cout << std::endl;
    }
    return 0;
}

