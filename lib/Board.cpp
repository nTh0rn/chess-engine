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

	//Board state
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
	fen += (whosTurn == 1) ? " w " : " b "; // Whos turn

	//Castling rights
	if (canCastle[0]) { fen += "K"; }
	if (canCastle[1]) { fen += "Q"; }
	if (canCastle[2]) { fen += "k"; }
	if (canCastle[3]) { fen += "q"; }

	//Moves & Half moves made
	fen += " ";
	fen += to_string(halfMoves);
	fen += " ";
	fen += to_string(fullMoves);
	return fen;
}

//Whether the king of the current moving side is safe.
bool Board::kingSafe(array<int, 3> move) {
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
}

//Generate a single piece's possible moves
vector<array<int,3>> Board::generateMove(int pos) {
	vector<array<int, 3>> movess = {};
	vector<array<int, 3>> pieceMoves;

	//Ensure only moves for whoever's turn it is get calculated
	if (((square[pos] < 97) ? 1 : 0) != whosTurn || square[pos] == ' ') {
		return {};
	}

	switch (tolower(square[pos])) {
	case 'p':
		movess = generatePawnMoves(pos);
		break;
	case 'r':
		movess = generateSlidingMoves(pos, 0);
		break;
	case 'b':
		movess = generateSlidingMoves(pos, 1);
		break;
	case 'n':
		movess = generatePositionMoves(pos, {-17, -15, -10, -6, 6, 10, 15, 17});
		break;
	case 'q':
		//Combine both types of sliding moves
		movess = generateSlidingMoves(pos, 0);
		pieceMoves = generateSlidingMoves(pos, 1);
		movess.insert(movess.end(), pieceMoves.begin(), pieceMoves.end());
		break;
	case 'k':
		movess = generatePositionMoves(pos, {-9, -8, -7, -1, 1, 7, 8, 9}, true);
		break;
	}

	return movess;

}

//Generates pawn moves
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
				moves.push_back({ pos, pos + (8 * dir), 0 });
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

	//Initialized to capture to the left
	int cap = pos + (8 * dir) - 1;
	for (int i = 0; i < 2; i++) {
		//Check bounds
		if (cap < 64 && cap >= 0 && abs((cap % 8) - (pos % 8)) == 1) {
			//Check for en-passant
			if (square[cap] != ' ' && (square[cap] < 97 ? 1 : 0) != whosTurn) {
				//Check for promotion
				if (int(cap / 8) != ((whosTurn == 0) ? 7 : 0)) {
					if (kingSafe({pos, cap, 1})) {
						moves.push_back({ pos, cap, 1 });
					}
				} else {
					if (kingSafe({ pos, cap, 6 })) {
						moves.push_back({ pos, cap, 6 });
						moves.push_back({ pos, cap, 7 });
						moves.push_back({ pos, cap, 8 });
						moves.push_back({ pos, cap, 9 });
					}
				}
			} else if(enPassant == cap) {
				if (kingSafe({ pos, cap, 3 })) {
					moves.push_back({ pos, cap, 3 });
				}
			}
		}
		//Update to capture to the right
		cap = pos + (8 * dir) + 1;
	}

	return moves;
}

//Generates both types of sliding moves
vector<array<int, 3>> Board::generateSlidingMoves(int pos, int type) {
	vector<array<int, 3>> moves;
	int dir;
	int amt;
	int prevPos;
	switch (type) {
	//Horizontal/vertical sliding moves
	case 0:
		//Left-side horizontal moves
		amt = 1;
		dir = -1;
		for (int i = 0; i < 2; i++) {
			while (int((pos + (dir*amt)) / 8) == int(pos / 8)) {
				if (square[pos + (dir*amt)] == ' ') { //Empty space
					if (kingSafe({ pos, pos + (dir * amt), 0 })) {
						moves.push_back({ pos, pos + (dir * amt), 0 });
					}
					amt += 1;
				} else if ((square[pos+(dir*amt)] < 97 ? 1 : 0) != whosTurn) { //Enemy piece
					if (kingSafe({ pos, pos + (dir * amt), 1 })) {
						moves.push_back({ pos, pos + (dir * amt), 1 });
					}
					break;
				} else { //Own piece
					break;
				}
			}
			//Right-side horizontal moves
			amt = 1;
			dir = 1;
		}
		//Upper vertical moves
		amt = 1;
		dir = -8;
		for (int i = 0; i < 2; i++) {
			while (pos + (dir*amt) >= 0 && pos + (dir*amt) < 64) {
				if (square[pos + (dir*amt)] == ' ') { //Empty space
					if (kingSafe({ pos, pos + (dir * amt), 0 })) {
						moves.push_back({ pos, pos + (dir * amt), 0 });
					}
					amt += 1;
				} else if ((square[pos+(dir*amt)] < 97 ? 1 : 0) != whosTurn) { //Enemy piece
					if (kingSafe({ pos, pos + (dir * amt), 1 })) {
						moves.push_back({ pos, pos + (dir * amt), 1 });
					}
					break;
				} else { //Own piece
					break;
				}
			}
			//Lower vertical moves
			amt = 1;
			dir = 8;
		}
		break;
	//Diagonal sliding moves
	case 1:
		amt = 1;
		dir = -9;
		prevPos = pos;
		for (int i = 0; i < 2; i++) {
			//Upper diagonal moves
			for (int j = 0; j < 2; j++) {
				while (pos + (dir * amt) >= 0 && pos + (dir * amt) < 64 && abs(((pos+(dir*amt)) % 8) - (prevPos % 8)) == 1) {
					if (square[pos + (dir*amt)] == ' ') { //Empty space
						if (kingSafe({ pos, pos + (dir * amt), 0 })) {
							moves.push_back({ pos, pos + (dir * amt), 0 });
						}
						prevPos = pos + (dir * amt);
						amt += 1;
					} else if ((square[pos+(dir*amt)] < 97 ? 1 : 0) != whosTurn) { //Enemy piece
						if (kingSafe({ pos, pos + (dir * amt), 1 })) {
							moves.push_back({ pos, pos + (dir * amt), 1 });
						}
						break;
					} else { //Own piece
						break;
					}
				}
				//Lower diagonal moves
				amt = 1;
				dir = (dir == -9) ? 9 : 7;
				prevPos = pos;
			}
			//Swap diagonal
			amt = 1;
			dir = -7;
			prevPos = pos;
		}
		break;
	}
	return moves;
}

//Generates legal moves from a given list of positions.
vector<array<int, 3>> Board::generatePositionMoves(int pos, array<int, 8> area, bool king) {
	vector <array<int, 3>> moves;
	for (int to : area) {
		//Check bounds
		if (pos + to >= 0 && pos + to < 64 && abs(((pos + to) % 8) - (pos % 8)) <= 2) {
			if (square[pos + to] == ' ') {
				if (kingSafe({ pos, pos + to, 0 })) {
					moves.push_back({ pos, pos + to, 0 });
				}
			} else if ((square[pos + to] < 97 ? 1 : 0) != whosTurn) {
				if (kingSafe({ pos, pos + to, 1 })) {
					moves.push_back({ pos, pos + to, 1 });
				}
			}
		}
	}
	return moves;
}

//Makes a move using the from square, to square, and move flag.
void Board::makeMove(array<int, 3> move) {
	int from = move[0];
	int to = move[1];
	int flag = move[2];
	enPassant = -1;
	switch (flag) {
	//Non-capturing move
	case 0:
		swap(square[from], square[to]);
		break;
	//Capturing move
	case 1:
		cout << "Regular capture\n";
		square[to] = square[from];
		square[from] = ' ';
		break;
	//En-passantable move
	case 2:
		enPassant = to + ((square[from] < 97 ? 8 : -8));
		swap(square[from], square[to]);
		break;
	//En-passant
	case 3:
		swap(square[from], square[to]);
		square[to + ((square[to] < 97 ? 8 : -8))] = ' ';
		break;
	//Queen-side castle
	case 4:
		swap(square[to], square[from]);
		swap(square[to-2], square[from-1]);
		break;
	//King-side castle
	case 5:
		swap(square[to], square[from]);
		swap(square[to+1], square[from+1]);
		break;
	//Queen promotion
	case 6:
		cout << "Queen promotion\n";
		square[to] = (square[from] < 97 ? 'Q' : 'q');
		square[from] = ' ';
		break;
	//Rook promotion
	case 7:
		cout << "Rook promotion\n";
		square[to] = (square[from] < 97 ? 'R' : 'r');
		square[from] = ' ';
		break;
	//Bishop promotion
	case 8:
		cout << "Bishop promotion\n";
		square[to] = (square[from] < 97 ? 'B' : 'b');
		square[from] = ' ';
		break;
	//Knight promotion
	case 9:
		cout << "Knight promotion\n";
		square[to] = (square[from] < 97 ? 'N' : 'n');
		square[from] = ' ';
		break;
	}
	plys += 1;
	whosTurn = 1 - whosTurn;
}