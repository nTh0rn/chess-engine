#include "../include/Board.h"
using namespace std;

//Default constructor
Board::Board() {};

//FEN constructor
Board::Board(string fen) {
	int stage = 0;
	int pos = 0;
	int enPassantXY[2] = { -1, -1 };

	//FEN decoding
	for (char c : fen) {
		if (c == ' ') {
			stage++;
		}
		switch (stage) {
		//Pieces on the board
		case 0:
			if (!isdigit(c)) {
				if (c == '/') {
					break;
				}
				if (pos < 64) {
					square[pos] = c;
					pos++;
				}
			} else {
				int emptySquares = c - '0';
				for (int i = 0; i < emptySquares; i++) {
					square[pos] = ' ';
					pos++;
				}
			}
			break;
			//Who is to move
		case 1:
			whosTurn = (c == 'w' ? 1 : 0);
			break;
			//Permanent castling rights
		case 2:
			switch (c) {
			case 'K':
				canCastle[0] = true;
				break;
			case 'Q':
				canCastle[1] = true;
				break;
			case 'k':
				canCastle[2] = true;
				break;
			case 'q':
				canCastle[3] = true;
				break;
			default:
				break;
			}
			break;
			//En passant x position
		case 3:
			if (c == '-') {
				stage++;
				break;
			}
			enPassantXY[0] = c - 'a';
			break;
			//En passant y position
		case 4:
			enPassantXY[1] = c - '1';
			this->enPassant = enPassantXY[0] + (enPassantXY[1] * 8);
			break;
			//Half moves
		case 5:
			if (isdigit(c)) {
				halfMoves = c - '0';
			}
			break;
			//Full moves
		case 6:
			if (isdigit(c)) {
				fullMoves = c - '0';
				plys = fullMoves * 2 + (1-whosTurn);
			}
			break;
		default:
			break;
		}
	}
}

//Generate FEN of current board
string Board::genFen() {
	string fen = "";
	int skip = 0;
	for (int pos = 0; pos < 64; pos++) {
		if (square[pos] == ' ') {
			skip++;
		} else {

			if (skip > 0) {
				fen += to_string(skip);
			}
			fen += square[pos];
			skip = 0;
		}
		if ((pos+1) % 8 == 0 && pos != 0) {
			if (skip > 0) {
				fen += to_string(skip);
			}
			if (pos != 63) {
				fen += "/";
			}
			skip = 0;
		}
	}
	fen += (whosTurn == 1) ? " w " : " b ";
	if (canCastle[0]) { fen += "K"; }
	if (canCastle[1]) { fen += "Q"; }
	if (canCastle[2]) { fen += "k"; }
	if (canCastle[3]) { fen += "q"; }
	fen += " ";
	fen += to_string(halfMoves);
	fen += " ";
	fen += to_string(fullMoves);
	return fen;
}

//Whether the king of the current moving side is safe.
bool Board::kingSafe() {
	return true;
}

//Whether a specific coordinate is under attack.
bool Board::inCheck(int pos) {
	return true;
}

//Generate all possible moves for whoever's turn it is
void Board::generateMoves() {
	moves.clear();
	vector<array<int, 3>> pieceMoves;
	for (int pos = 0; pos < 64; pos++) {

		//Ensure only moves for whoever's turn it is get calculated
		if (((square[pos] < 97) ? 1 : 0) != whosTurn || square[pos] == ' ') {
			cout << square[pos] << " - " << whosTurn << "\n";
			continue;
		}

		switch (tolower(square[pos])) {
		case 'p':
			pieceMoves = generatePawnMoves(pos);
			break;
		case 'r':
			pieceMoves = generateSlidingMoves(pos, 0);
			break;
		case 'b':
			pieceMoves = generateSlidingMoves(pos, 1);
			break;
		case 'n':
			pieceMoves = generatePositionMoves(pos, {-17, -15, -10, -6, 6, 10, 15, 17});
			break;
		case 'q':
			pieceMoves = generateSlidingMoves(pos, 0);
			moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
			pieceMoves = generateSlidingMoves(pos, 1);
			break;
		case 'k':
			pieceMoves = generatePositionMoves(pos, {-9, -8, -7, -1, 1, 7, 8, 9});
			break;
		}
		moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
	}
	for (array<int, 3> move : moves) {
		cout << "Highlighting " << move[2] << "\n";
	}
}
vector<array<int, 3>> Board::generatePawnMoves(int pos) {
	vector<array<int, 3>> moves;
	int dir = (whosTurn == 0 ? 1 : -1);

	//Move forward
	if (pos + (8 * dir) < 64 && pos + (8 * dir) >= 0) {
		if (square[pos + (8 * dir)] == ' ') {
			//Check whether is normal move or promotion
			if (pos + (8 * dir) == ((whosTurn == 0) ? 0 : 7)) {
				moves.push_back({ pos, pos + (8 * dir), 5 });
				moves.push_back({ pos, pos + (8 * dir), 6 });
				moves.push_back({ pos, pos + (8 * dir), 7 });
				moves.push_back({ pos, pos + (8 * dir), 8 });
			} else {
				cout << "Adding " << (pos + (8 * dir)) << "\n";
				moves.push_back({ pos, pos + (8 * dir), 0 });
				for (array<int, 3> move : moves) {
					cout << "Piece moves in pawn gen " << move[2] << "\n";
				}
				//Double jump forward
				if (pos + (16 * dir) < 64 && pos + (16 * dir) >= 0 && int(pos / 8) == ((whosTurn == 0) ? 1 : 6)) {
					if (square[pos + (16 * dir)] == ' ') {
						//Flag is 2 for en-passantable move
						moves.push_back({ pos, pos + (16 * dir), 2 });
					}
				}
			}
		}
	}

	//Capture to the left
	if (pos + (8 * dir) - 1 < 64 && pos + (8 * dir) - 1 >= 0 && abs(((pos + (8 * dir) - 1) % 8) - (pos % 8)) == 1) {
		if (((square[pos + (8 * dir) - 1] < 97) ? 1 : 0) != whosTurn && square[pos + (8 * dir) - 1] != ' ') {
			//Check whether is normal capture, promotion, or en-passant
			if (pos + (8 * dir) - 1 == ((whosTurn == 0) ? 0 : 7)) {
				moves.push_back({ pos, pos + (8 * dir) - 1, 6 });
				moves.push_back({ pos, pos + (8 * dir) - 1, 7 });
				moves.push_back({ pos, pos + (8 * dir) - 1, 8 });
				moves.push_back({ pos, pos + (8 * dir) - 1, 9 });
			} else if(enPassant != pos + (8 * dir) - 1) {
				moves.push_back({ pos, pos + (8 * dir) - 1, 1 });
			} else {
				moves.push_back({ pos, pos + (8 * dir) - 1, 3 });
			}
		}
	}

	//Capture to the right
	if (pos + (8 * dir) + 1 < 64 && pos + (8 * dir) + 1 >= 0 && abs(((pos + (8 * dir) + 1) % 8) - (pos % 8)) == 1) {
		if (((square[pos + (8 * dir) + 1] < 97) ? 1 : 0) != whosTurn && square[pos + (8 * dir) - 1] != ' ') {
			//Check whether is normal capture, promotion, or en-passant
			if (pos + (8 * dir) + 1 == ((whosTurn == 0) ? 0 : 7)) {
				moves.push_back({ pos, pos + (8 * dir) + 1, 5 });
				moves.push_back({ pos, pos + (8 * dir) + 1, 6 });
				moves.push_back({ pos, pos + (8 * dir) + 1, 7 });
				moves.push_back({ pos, pos + (8 * dir) + 1, 8 });
			} else if(enPassant != pos + (8 * dir) + 1) {
				moves.push_back({ pos, pos + (8 * dir) + 1, 1 });
			} else {
				moves.push_back({ pos, pos + (8 * dir) + 1, 3 });
			}
		}
	}
	cout << "pawn moves size " << moves.size() << "\n";

	return moves;
}
vector<array<int, 3>> Board::generateSlidingMoves(int pos, int type) {
	return { {1,1,1} };
}
vector<array<int, 3>> Board::generatePositionMoves(int pos, array<int, 8> area) {
	return { {1,1,1} };
}