#!/usr/bin/python
# coding: utf-8


def ConstructRepresentation(player0, player1):
    result = [['.' for j in range(8)] for i in range(8)]
    for i in range(64):
        t0, t1 = player0 % 2, player1 % 2
        player0, player1 = player0 / 2, player1 / 2
        a, b = i / 8, i % 8
        if t0:
            result[a][b] = 'w'
        if t1:
            result[a][b] = 'b'
    result = [''.join(row) for row in result]
    result = '\n'.join(result)
    return result + '\n'


def main():
    for line in open("dist.txt", "rt"):
        if line and line[-1] == '\n':
            line = line[:-1]
        cols = line.split(' ')
        if len(cols) != 4:
            continue
        player0, player1, dist0, dist1 = map(int, cols)
        if dist0 != -1:
            continue
        field = ConstructRepresentation(player0, player1)
        if len(filter(lambda x: x == 'w', field)) == 4:
            print(field)


if __name__ == "__main__":
    main()

