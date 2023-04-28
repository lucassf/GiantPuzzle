#define _CRT_SECURE_NO_WARNINGS

#include "StateMachine.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>


void solveColumn(int fixedRow, Board& boardA, const Board& boardB, int row, int mainColumn) {
	int NROWS = boardA.getDimensions().first;
	if (row < 0)
		row += NROWS;
	int num2 = boardB.getElementAt(Node{ row, mainColumn });
	int num1 = boardB.getElementAt(Node{ (row + NROWS / 2) % NROWS, mainColumn });

	Node node1, node2;
	State state1, state2;

	do {
		node1 = boardA.getPosition(num1);
		node2 = boardA.getPosition(num2);

		state1 = getNextState(node1, true, mainColumn, fixedRow);
		state2 = getNextState(node2, false, mainColumn, fixedRow);
		stateAction(state1, state2, node1, node2, mainColumn, fixedRow, boardA);
	} while (state1 != State::CORRECTLY_POSITIONED || state2 != State::CORRECTLY_POSITIONED);
	boardA.shiftColumn(Node(0, mainColumn), Node(1, mainColumn));
}


void solveColumns(int fixedRow, Board& boardA, const Board& boardB) {
	auto dims = boardA.getDimensions();
	int NROWS = dims.first, NCOLS = dims.second;

	setBoardDimensions(NROWS, NCOLS);

	std::vector<bool> usedColumns(NCOLS, false);

	for (int j = 0; j < NCOLS; ++j) {
		int best = 1000000009;
		int selectedCol = -1;
		Board selectedBoard;
		for (int mainColumn = 0; mainColumn < NCOLS; ++mainColumn) {
			if (!usedColumns[mainColumn]) {
				Board tempA = boardA;
				for (int i = 1; i < NROWS / 2; ++i) {
					int row = fixedRow - i;
					solveColumn(fixedRow, tempA, boardB, row, mainColumn);
				}
				if (tempA.getTotalMoviments() < best) {
					best = (int)tempA.getTotalMoviments();
					selectedCol = mainColumn;
					selectedBoard = tempA;
				}
			}
		}
		boardA = selectedBoard;
		usedColumns[selectedCol] = true;
	}
}


void solveRow(int fixedRow, Board& boardA, const Board& boardB) {
	auto dims = boardA.getDimensions();
	int NROWS = dims.first, NCOLS = dims.second;

	int num = boardB.getElementAt(Node(fixedRow, 1));
	bool extraColumnShift = false;
	for (int i = 2; i < NCOLS * 2 - 1; ++i) {
		Node node = cellAfterNRowShifts(boardB.getPosition(num), 1);
		Node goal = cellAfterNRowShifts(boardA.getPosition(num), 1);
		num = boardB.getElementAt(node);
		node = boardA.getPosition(num);

		int shiftDist = rowShiftDistance(goal, node);
		if (shiftDist > 3) {
			moveAndSwap(node, cellAfterNRowShifts(goal, shiftDist % 2 == 0 ? 1 : 2), boardA);
			node = boardA.getPosition(num);
			boardA.shiftRow(node, cellAfterNRowShifts(node, 1));
			boardA.shiftColumn(Node(0, node.col), Node(NROWS / 2, node.col));
			shiftDist = shiftDist % 2 + 1;
			node = cellAfterNRowShifts(node, 1);
		}
		while (shiftDist > 0) {
			boardA.shiftRow(node, Node(fixedRow, 0));
			boardA.shiftColumn(Node(0, 0), Node(NROWS / 2, 0));
			--shiftDist;
			extraColumnShift = !extraColumnShift;
			node = boardA.getPosition(num);
		}
	}
	Node firstNode = boardA.getPosition(boardB.getElementAt(Node(fixedRow, 1)));
	boardA.shiftRow(firstNode, Node(fixedRow, 1));
	if (extraColumnShift)
		boardA.shiftColumn(Node(0, 0), Node(NROWS / 2, 0));
}

void solve(Board& boardStart, const Board& boardGoal) {
	int best = 1000000009;
	int row = -1;
	std::string ans;
	for (int fixedRow = 0; fixedRow < 16; ++fixedRow) {
		auto start = boardStart;
		solveColumns(fixedRow, start, boardGoal);
		solveRow(fixedRow, start, boardGoal);

		if (start.euclidianDistance(boardGoal) == 0) {
			best = std::min(best, start.getTotalMoviments());
			ans = start.getAppliedCommands();
			row = fixedRow;
		}
	}
	std::cout << best << std::endl;
	std::cout << ans << std::endl;
}


void validate() {
	Board boardA("resources/A.csv");
	Board boardB("resources/B.csv");

	std::fstream file("commands.csv", std::ios::in);

	std::string commands;
	if (file.is_open())
	{
		std::getline(file, commands);
		std::getline(file, commands);
	}
	boardA.applyCommands(commands);
	assert(boardA.euclidianDistance(boardB) == 0);
}

int main()
{
	Board boardA("resources/A.csv");
	Board boardB("resources/B.csv");
	solve(boardA, boardB);
	//validate();
}
