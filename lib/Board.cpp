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
					if (c == 'k') {
						kingPos[0] = true;
					} else if (c == 'K') {
						kingPos[1] = true;
					}
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
	array<int, 3> moveToUnmake = move;
	int unmakeEnPassant = enPassant;
	array<bool, 4> unmakeCanCastle = canCastle;
	char unmakeTakenPiece = square[move[1]];
	makeMove(move);
	//if (square[move[1]] == 'r') {
	//	cout << "kill yourself\n";
	//	cout << "Unmake from: " << moveToUnmake[0] << " to: " << moveToUnmake[1] << " flag: " << moveToUnmake[2] << " enPessant: " << unmakeEnPassant << "\n";
	//}
	bool output = !inCheck(kingPos[1 - whosTurn]);
	
	unmakeMove(moveToUnmake, unmakeEnPassant, unmakeCanCastle, unmakeTakenPiece);
	return output;
}

//Whether a specific coordinate is under attack.
bool Board::inCheck(int pos) {
	vector<array<int, 3>> moves;
	array<char, 6> enemyPieces = { (whosTurn == 0 ? 'r' : 'R'), (whosTurn == 0 ? 'b' : 'B'), (whosTurn == 0 ? 'q' : 'Q'), (whosTurn == 0 ? 'n' : 'N') , (whosTurn == 0 ? 'k' : 'K') , (whosTurn == 0 ? 'p' : 'P')};
	int dir;
	int amt;
	int prevPos;

	amt = 1;
	dir = -1;
	for (int i = 0; i < 2; i++) {
		while (int((pos + (dir * amt)) / 8) == int(pos / 8)) {
			if (square[pos + (dir * amt)] == ' ') { //Empty space
				amt += 1;
			} else if ((square[pos + (dir * amt)] < 97 ? 1 : 0) == whosTurn &&
				(square[pos + (dir * amt)] == enemyPieces[0] || square[pos + (dir * amt)] == enemyPieces[2])) { //Enemy piece
				return true;
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
		while (pos + (dir * amt) >= 0 && pos + (dir * amt) < 64) {
			if (square[pos + (dir * amt)] == ' ') { //Empty space
				amt += 1;
			} else if ((square[pos + (dir * amt)] < 97 ? 1 : 0) == whosTurn &&
				(square[pos + (dir * amt)] == enemyPieces[0] || square[pos + (dir * amt)] == enemyPieces[2])) {//Enemy piece
				return true;
			} else { //Own piece
				break;
			}
		}
		//Lower vertical moves
		amt = 1;
		dir = 8;
	}

	//Diagonal sliding moves

	amt = 1;
	dir = -9;
	prevPos = pos;
	for (int i = 0; i < 2; i++) {
		//Upper diagonal moves
		for (int j = 0; j < 2; j++) {
			while (pos + (dir * amt) >= 0 && pos + (dir * amt) < 64 && abs(((pos + (dir * amt)) % 8) - (prevPos % 8)) == 1) {
				if (square[pos + (dir * amt)] == ' ') { //Empty space
					prevPos = pos + (dir * amt);
					amt += 1;
				} else if ((square[pos + (dir * amt)] < 97 ? 1 : 0) == whosTurn &&
					(square[pos + (dir * amt)] == enemyPieces[1] || square[pos + (dir * amt)] == enemyPieces[2])) { //Enemy piece
					return true;
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

	//Check for king checking
	for (int to : {-9, -8, -7, -1, 1, 7, 8, 9}) {
		//Check bounds
		if (pos + to >= 0 && pos + to < 64 && abs(((pos + to) % 8) - (pos % 8)) <= 2) {
			if (square[pos + to] == enemyPieces[4]) {
				return true;
			}
		}
	}

	//Check for knights checking
	for (int to : {-17, -15, -10, -6, 6, 10, 15, 17}) {
		//Check bounds
		if (pos + to >= 0 && pos + to < 64 && abs(((pos + to) % 8) - (pos % 8)) <= 2) {
			if ((square[pos + to] < 97 ? 1 : 0) == whosTurn && square[pos + to] == enemyPieces[3]) {
				return true;
			}
		}
	}

	//Check for pawns checking
	dir = (whosTurn == 1 ? 9 : -9);
	for (int i = 0; i < 2; i++) {
		if (pos + dir >= 0 && pos + dir < 64) {
			if (abs(((pos + dir) % 8) - (pos % 8)) == 1) {
				if (square[pos + dir] == enemyPieces[5]) {
					return true;
				}
			}
		}
		dir = (whosTurn == 1 ? 7 : -7);
	}

	return false;
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
			kingPos[whosTurn] = pos;
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
		kingPos[whosTurn] = pos;
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
				if (kingSafe({ pos, pos + (8 * dir), 6 })) {
					moves.push_back({ pos, pos + (8 * dir), 6 });
					moves.push_back({ pos, pos + (8 * dir), 7 });
					moves.push_back({ pos, pos + (8 * dir), 8 });
					moves.push_back({ pos, pos + (8 * dir), 9 });
				}
			} else {
				if (kingSafe({ pos, pos + (8 * dir), 0 })) {
					moves.push_back({ pos, pos + (8 * dir), 0 });
				}
				//Double jump forward
				if (pos + (16 * dir) < 64 && pos + (16 * dir) >= 0 && int(pos / 8) == ((whosTurn == 0) ? 1 : 6)) {
					if (square[pos + (16 * dir)] == ' ') {
						//Flag is 2 for en-passantable move
						if (kingSafe({ pos, pos + (16 * dir), 2 })) {
							moves.push_back({ pos, pos + (16 * dir), 2 });
						}
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
			while (int((pos + (dir*amt)) / 8) == int(pos / 8) && pos+(dir*amt) >= 0 && pos+(dir*amt) < 64) {
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

	if (king) {
		if (canCastle[(whosTurn == 1 ? 0 : 2)]) {
			if (square[kingPos[whosTurn] + 1] == ' ' && square[kingPos[whosTurn] + 2] == ' ' && square[kingPos[whosTurn] + 3] == (whosTurn == 1 ? 'R' : 'r')) {
				if (!inCheck(kingPos[whosTurn]) && !inCheck(kingPos[whosTurn] + 1) && !inCheck(kingPos[whosTurn] + 2)) {
					moves.push_back({ pos, pos + 2, 5 });
				}
			}
		}
		if (canCastle[(whosTurn == 1 ? 1 : 3)]) {
			if (square[kingPos[whosTurn] - 1] == ' ' && square[kingPos[whosTurn] - 2] == ' ' && square[kingPos[whosTurn] - 4] == (whosTurn == 1 ? 'R' : 'r')) {
				if (!inCheck(kingPos[whosTurn]) && !inCheck(kingPos[whosTurn] - 1) && !inCheck(kingPos[whosTurn] - 2)) {
					moves.push_back({ pos, pos - 2, 4 });
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
	if ((tolower(square[from]) == 'r' && (from == 63 || from == 7))) {
		canCastle[(whosTurn == 0 ? 0 : 2)] = false;
	} else if ((tolower(square[from]) == 'r' && (from == 0 || from == 56))) {
		canCastle[(whosTurn == 0 ? 1 : 3)] = false;
	} else if (tolower(square[from]) == 'k') {
		kingPos[whosTurn] = to;
		canCastle[(whosTurn == 0 ? 0 : 2)] = false;
		canCastle[(whosTurn == 0 ? 1 : 3)] = false;
	}
	switch (flag) {
	//Non-capturing move
	case 0:
		swap(square[from], square[to]);
		break;
	//Capturing move
	case 1:
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
		square[to] = (square[from] < 97 ? 'Q' : 'q');
		square[from] = ' ';
		break;
	//Rook promotion
	case 7:
		square[to] = (square[from] < 97 ? 'R' : 'r');
		square[from] = ' ';
		break;
	//Bishop promotion
	case 8:
		square[to] = (square[from] < 97 ? 'B' : 'b');
		square[from] = ' ';
		break;
	//Knight promotion
	case 9:
		square[to] = (square[from] < 97 ? 'N' : 'n');
		square[from] = ' ';
		break;
	}
	plys += 1;
	whosTurn = 1 - whosTurn;
}


void Board::unmakeMove(array<int, 3> moveToUnmake, int unmakeEnPassant, array<bool, 4> unmakeCanCastle, char unmakeTakenPiece) {
	canCastle = unmakeCanCastle;
	enPassant = unmakeEnPassant;
	int from = moveToUnmake[0];
	int to = moveToUnmake[1];
	int flag = moveToUnmake[2];

	if (tolower(square[to]) == 'k') {
		kingPos[1 - whosTurn] = from;
	}

	switch (flag) {
	case 0: //Non-capturing move
	case 2: //En-passantable move
		swap(square[from], square[to]);
		break;
		//Capturing move
	case 1:
		square[from] = square[to];
		square[to] = unmakeTakenPiece;
		break;
		//En-passant
	case 3:
		swap(square[from], square[to]);
		square[to + ((square[from] < 97 ? 8 : -8))] = (square[from] < 97 ? 'p' : 'P');
		break;
		//Queen-side castle
	case 4:
		swap(square[to], square[from]);
		swap(square[to - 2], square[from - 1]);
		break;
		//King-side castle
	case 5:
		swap(square[to], square[from]);
		swap(square[to + 1], square[from + 1]);
		break;
	//Promotins
	case 6:
	case 7:
	case 8:
	case 9:
		square[from] = (square[to] < 97 ? 'P' : 'p');
		square[to] = unmakeTakenPiece;
		break;
	}
	plys -= 1;
	whosTurn = 1 - whosTurn;
}