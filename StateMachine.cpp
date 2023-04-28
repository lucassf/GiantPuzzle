#include "StateMachine.hpp"

namespace {
	int NROWS, NCOLS;
}


void setBoardDimensions(int nrows, int ncols) {
	NROWS = nrows;
	NCOLS = ncols;
}


State getNextState(Node node, bool firstRow, int mainColumn, int mainRow) {
	int setRow = firstRow ? mainRow : mainRow + NROWS / 2;
	if (node.row == setRow && node.col == mainColumn)
		return State::CORRECTLY_POSITIONED;
	if (node.col == mainColumn)
		return State::ON_MAIN_COLUMN;
	if (node.row == mainRow || node.row == mainRow + NROWS / 2)
		return State::ON_MAIN_ROWS;
	return State::OTHER;
}

inline int getNearestRow(Node n, int mainRow) {
	return n.row < NROWS / 2 ? mainRow : mainRow + NROWS / 2;
}


Node cellAfterNRowShifts(const Node& node, int n) {
	if (abs(n) > NCOLS)
		n = (n < 0 ? 1 : -1) * (2 * NCOLS - abs(n));
	int col = node.col + n * (node.row >= NROWS / 2 ? -1 : 1);
	int row = node.row;
	if (col < 0 || col >= NCOLS)
		row = (NROWS / 2 + row) % NROWS;
	if (col < 0)
		col = -col - 1;
	if (col >= NCOLS)
		col = 2 * NCOLS - col - 1;

	return Node(row, col);
}

void moveAndSwap(Node node, Node goal, Board& board) {
	Node revGoal = Node((goal.row + NROWS / 2) % NROWS, goal.col);
	while (node != revGoal) {
		board.shiftRow(node, cellAfterNRowShifts(node, 1));
		revGoal = cellAfterNRowShifts(revGoal, -1);
		node = cellAfterNRowShifts(node, 1);
	}
	board.shiftColumn(node, Node((node.row + NROWS / 2) % NROWS, node.col));
}

int rowShiftDistance(Node node1, Node node2) {
	int dist = 0;
	Node node = node1;
	while (node != node2) {
		node = cellAfterNRowShifts(node, 1);
		++dist;
	}
	return dist;
}


void stateAction(State state1, State state2, Node n1, Node n2, int mainColumn, int mainRow, Board& board) {
	int nodeDelta = mainColumn * 2 + 1;
	if (state1 == State::CORRECTLY_POSITIONED) {
		if (state2 != State::CORRECTLY_POSITIONED)
			state1 = State::ON_MAIN_COLUMN;
		else
			return;
	}
	if (state1 == State::OTHER) {
		int nearestRow1 = getNearestRow(n1, mainRow);
		board.shiftColumn(n1, Node(nearestRow1, n1.col));

		return;
	}
	if (state1 == State::ON_MAIN_COLUMN) {
		int deltas = 0;
		if (state2 == State::ON_MAIN_COLUMN || state2 == State::CORRECTLY_POSITIONED) {
			deltas += board.shiftColumn(n1, { mainRow, n1.col });
			board.shiftRow({ mainRow, n1.col }, cellAfterNRowShifts(Node(mainRow, n1.col), mainColumn == NCOLS - 1 ? 2 : 1));
			board.shiftColumn({ 0, mainColumn }, { (NROWS - deltas) % NROWS, mainColumn });

			return;
		}
		if (n1.row == mainRow || n1.row == mainRow + NROWS / 2) {
			deltas += board.shiftColumn(Node(0, n1.col), Node(1, n1.col));
			n1.row = (n1.row + 1) % NROWS;
		}
		if (state2 == State::OTHER) {
			int nearestRow2 = getNearestRow(n2, mainRow);
			board.shiftColumn(n2, Node(nearestRow2, n2.col));
			n2 = Node(nearestRow2, n2.col);
		}
		int nearestRow1 = getNearestRow(n1, mainRow);
		Node goal = Node(nearestRow1, n1.col);
		board.shiftRow(n2, cellAfterNRowShifts(goal, -nodeDelta));
		deltas += board.shiftColumn(n1, goal);
		board.shiftRow(goal, Node(mainRow, mainColumn));

		board.shiftColumn({ 0, mainColumn }, { (NROWS - deltas) % NROWS, mainColumn });

		return;
	}
	if (state2 == State::OTHER) {
		int nearestRow2 = getNearestRow(n2, mainRow);
		Node goal2 = Node(nearestRow2, n2.col);
		Node goal1 = cellAfterNRowShifts(goal2, nodeDelta);

		board.shiftRow(n1, goal1);
		board.shiftColumn(n2, goal2);
		board.shiftRow(goal1, Node(mainRow, mainColumn));

		return;
	}
	if (cellAfterNRowShifts(n2, nodeDelta) == n1) {
		board.shiftRow(n1, Node(mainRow, mainColumn));
		return;
	}
	if (state2 == State::ON_MAIN_ROWS) {
		board.shiftRow(n2, Node(mainRow + NROWS / 2, mainColumn));
		return;
	}
	int deltas = 0;
	if (n2.row == mainRow || n2.row == mainRow + NROWS / 2) {
		deltas += board.shiftColumn(Node(0, mainColumn), Node(1, mainColumn));
		n2.row = (n2.row + 1) % NROWS;
	}
	Node goal2 = Node(mainRow, mainColumn);
	Node goal1 = cellAfterNRowShifts(Node(mainRow, mainColumn), nodeDelta);

	board.shiftRow(n1, goal1);
	deltas += board.shiftColumn(n2, goal2);
	board.shiftRow(goal2, cellAfterNRowShifts(goal2, 1));
	board.shiftColumn(Node(0, mainColumn), Node((NROWS - deltas) % NROWS, mainColumn));
	board.shiftRow(cellAfterNRowShifts(goal1, 1), Node(mainRow, mainColumn));
}