#include <vector>
#include <string>


struct Node {
	int row = 0, col = 0;

	Node() {}
	Node(int row, int col) :row(row), col(col) {}
	bool operator==(const Node& other) {
		return other.row == row && other.col == col;
	}
	bool operator!=(const Node& other) {
		return !(*this == other);
	}
};

class Board {
public:
	Board();
	Board(const std::string& filename);
	Board(const Board& other);
	int shiftColumn(Node start, Node end);
	int shiftRow(Node start, Node end);
	int getElementAt(Node position) const;
	Node getPosition(int num) const;

	int euclidianDistance(const Board& other);
	std::pair<int, int> getDimensions();
	std::string getAppliedCommands();
	int getTotalMoviments();

	void show() const;
	void applyCommands(std::string commands);

private:
	int NCOLS = 0, NROWS = 0, numMoviments = 0;
	std::vector<std::vector<int> > board;
	std::vector<Node> positions;
	std::string appliedCommands;

	void addAppliedCommand(char type, int position, int delta);
	int shiftColumn(int column, int delta);
	int shiftRow(int row, int delta);
};