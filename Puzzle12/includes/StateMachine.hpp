#include "Board.hpp"

enum class State {
	ON_MAIN_COLUMN = 0,
	ON_MAIN_ROWS = 1,
	CORRECTLY_POSITIONED = 2,
	OTHER = 3
};

Node cellAfterNRowShifts(const Node& node, int n);
void moveAndSwap(Node node, Node goal, Board& board);
int rowShiftDistance(Node node1, Node node2);
void setBoardDimensions(int nrows, int ncols);
State getNextState(Node node, bool firstRow, int mainColumn, int mainRow);
void stateAction(State state1, State state2, Node n1, Node n2, int mainColumn, int mainRow, Board& board);