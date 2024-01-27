#include "Board.hpp"
#include "Utilities.hpp"

#include <cassert>
#include <algorithm>
#include <cstdio>


Board::Board(){}


Board::Board(const std::string& filename) {
	board = readCsvData(filename);
	NCOLS = (int) board[0].size(), NROWS = (int) board.size();
	positions.resize(NCOLS * NROWS);
	for (int i = 0; i < NROWS; ++i)
		for (int j = 0; j < NCOLS; ++j)
			positions[board[i][j]] = Node{ i, j };
}

Board::Board(const Board& other) {
	NROWS = other.NROWS;
	NCOLS = other.NCOLS;
	board = other.board;
	positions = other.positions;
	appliedCommands = other.appliedCommands;
	numMoviments = other.numMoviments;
}

int Board::shiftColumn(Node start, Node end) {
	int srow = start.row;
	int scol = start.col;
	int erow = end.row;
	int ecol = end.col;

	assert(scol == ecol);
	return shiftColumn(scol, erow - srow);
}

int Board::shiftRow(Node start, Node end) {
	int srow = start.row;
	int scol = start.col;
	int erow = end.row;
	int ecol = end.col;

	bool sameRow = srow % (NROWS / 2) == erow % (NROWS / 2);
	assert(sameRow);

	return shiftRow(srow, srow == erow ? (srow < NROWS / 2 ? ecol - scol : scol - ecol) : (srow > erow ? scol + ecol + 1 : -scol - ecol - 1));

}

std::pair<int, int> Board::getDimensions() {
	return std::make_pair(NROWS, NCOLS);
}

int Board::getElementAt(Node position) const {
	return board[position.row][position.col];
}

Node Board::getPosition(int num) const {
	return positions[num];
}

std::string Board::getAppliedCommands() {
	return appliedCommands;
}

int Board::getTotalMoviments() {
	return numMoviments;
}

int Board::euclidianDistance(const Board& other) {
	int dist = 0;
	int tmax = NROWS * NCOLS;
	for (int i = 0; i < tmax; ++i) {
		auto posa = other.positions[i];
		auto posb = positions[i];

		dist += abs(posa.row - posb.row) + abs(posa.col - posb.col);
	}
	return dist;
}

void Board::addAppliedCommand(char type, int position, int delta) {
	auto symbol = type + std::to_string(position);
	numMoviments += delta;
	for (int i = 0; i < delta; ++i)
		appliedCommands += symbol;
}

int Board::shiftColumn(int column, int delta) {
	assert(column >= 0 && column < NCOLS);

	if (delta == 0)
		return 0;

	delta %= NROWS;
	if (delta < 0)
		delta += NROWS;

	std::vector<int> tempVec;
	for (int i = 0; i < NROWS; ++i)
		tempVec.push_back(board[i][column]);
	for (int i = 0; i < NROWS; ++i) {
		board[i][column] = tempVec[(i - delta + NROWS) % NROWS];
		positions[board[i][column]] = Node{ i, column };
	}
	addAppliedCommand('r', column, delta);

	return delta;
}

int Board::shiftRow(int row, int delta) {
	assert(row >= 0 && row < NROWS);

	if (delta == 0)
		return 0;

	if (row * 2 >= NROWS) {
		row -= NROWS / 2;
	}

	delta %= 2 * NCOLS;
	if (delta < 0)
		delta += 2 * NCOLS;

	auto rowVec = board[row];
	auto rowVec2 = board[row + NROWS / 2];

	std::reverse(rowVec2.begin(), rowVec2.end());
	rowVec.insert(rowVec.end(), rowVec2.begin(), rowVec2.end());
	for (int i = 0; i < (int)rowVec.size(); ++i) {
		int rowpos = i >= NCOLS ? row + NROWS / 2 : row;
		int colpos = i >= NCOLS ? 2 * NCOLS - i - 1 : i;
		board[rowpos][colpos] = rowVec[(i - delta + 2 * NCOLS) % (2 * NCOLS)];
		positions[board[rowpos][colpos]] = Node{ rowpos, colpos };
	}
	addAppliedCommand('s', row, delta);

	return delta;
}

void Board::show() const {
	printf("    ");
	for (int j = 0; j < NCOLS; ++j)
		printf("%3d%c", j, j == NCOLS - 1 ? '\n' : ' ');
	for (int j = 0; j < NCOLS; ++j)
		printf("----");
	printf("----\n");
	for (int i = 0; i < NROWS; ++i) {
		printf("%3d|", i);
		for (int j = 0; j < NCOLS; ++j)
			printf("%3d%c", board[i][j], j == NCOLS - 1 ? '\n' : ' ');
	}
}

void Board::applyCommands(std::string commands) {
	char comm = 0;
	int pos = 0;
	commands += "p";
	for (char c: commands) {
		if (c >= '0' && c <= '9')
			pos = pos * 10 + c - '0';
		else {
			if (comm == 's')
				shiftRow(pos, 1);
			if (comm == 'r')
				shiftColumn(pos, 1);
			comm = c;
			pos = 0;
		}
	}
}