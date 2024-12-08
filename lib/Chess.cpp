#include "../include/Chess.h"
using namespace std;

//Prints debug messages
void Chess::debugMessage(string input) {
	if (showDebugMessages) {
		cout << input << "\n";
	}
}

//Default constructor
Chess::Chess() {};

//FEN constructor
Chess::Chess(string fen, array<int, 2> time) {
	int stage = 0;
	int pos = 0;
	int enPassantXY[2] = { -1, -1 };
	if (time[0] == -1) {
		gameStarted = true;
	}
	halfMoves = 0;
	fullMoves = 1;
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
						kingPos[0] = pos;
					} else if (c == 'K') {
						kingPos[1] = pos;
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
string Chess::genFen() {
	string fen = "";
	int skip = 0;

	//Chess state
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

	genSpiral();

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

//Generate FEN of current board
string Chess::genFenRepitition() {
	string fen = "";
	int skip = 0;

	//Chess state
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
		if ((pos + 1) % 8 == 0 && pos != 0) {
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
	
	return fen;
}

//Generate the spiral coordinate array
void Chess::genSpiral() {
	int dx = 1, dy = 1, signx = -1, signy = -1;
	int pos = 36;
	int index = 0;
	spiralCoords[index] = pos;
	this_thread::sleep_for(std::chrono::milliseconds(1000));
	while (pos != 7) {
		for (int i = 0; i < dx; i++) {
			pos += 1 * signx;
			index++;
			spiralCoords[index] = pos;
			if (pos == 7) {
				return;
			}
		}
		for (int i = 0; i < dy; i++) {
			pos += 1 * signy*8;
			index++;
			spiralCoords[index] = pos;
		}
		dx++, dy++;
		signx*= -1, signy *= -1;
	}
}

//Return the evaluation of the game from white's perspective
double Chess::evaluate() {
	double output = 0;
	int whitePos;
	int blackPos;

	//How important the piece-square tables are.
	int weight = 200;

	//Simplified piece-square tables
	double pawn[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
		5,  5, 10, 25, 25, 10,  5,  5,
		0,  0,  0, 20, 20,  0,  0,  0,
		5, -5,-10,  0,  0,-10, -5,  5,
		5, 10, 10,-20,-20, 10, 10,  5,
		0,  0,  0,  0,  0,  0,  0,  0 };
	double rook[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
		5, 10, 10, 10, 10, 10, 10,  5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		0,  0,  0,  5,  5,  0,  0,  0 };
	double bishop[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-10,-10,-10,-10,-10,-20 };
	double knight[64] = { -50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50 };
	double king[64] = { -30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		20, 20,  0,  0,  0,  0, 20, 20,
		20, 30, 10,  0,  0, 10, 30, 20 };
	double king_eg[64] = { -50,-40,-30,-20,-20,-30,-40,-50,
		-30,-20,-10,  0,  0,-10,-20,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 30, 40, 40, 30,-10,-30,
		-30,-10, 20, 30, 30, 20,-10,-30,
		-30,-30,  0,  0,  0,  0,-30,-30,
		-50,-30,-30,-30,-30,-30,-30,-50 };
	double queen[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-5,  0,  5,  5,  5,  5,  0, -5,
		0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20 };

	if (!endgame) {
		int counter = 0;
		for (int pos = 0; pos < 64; pos++) {
			if (square[pos] != 'p' && square[pos] != 'P' && square[pos] != ' ') {
				counter++;
			}
		}
		if (counter <= 7) {
			endgame = true;
		}
	}

	//Iterate and add values
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			whitePos = (col * 8) + row;
			blackPos = ((7 - col) * 8) + row;
			switch (square[whitePos]) {
			case 'p':
				output += -pawn[blackPos]/weight + -1;
				break;
			case 'r':
				output += -rook[blackPos]/weight + -5;
				break;
			case 'b':
				output += -bishop[blackPos]/weight + -3;
				break;
			case 'n':
				output += -knight[blackPos]/weight + -3;
				break;
			case 'k':
				output += -(endgame ? king[whitePos] : king_eg[whitePos]) /weight + -100000;
				break;
			case 'q':
				output += -queen[blackPos]/weight + -9;
				break;
			case 'P':
				output += pawn[whitePos]/weight + 1;
				break;
			case 'R':
				output += rook[whitePos]/weight + 5;
				break;
			case 'B':
				output += bishop[whitePos]/weight + 3;
				break;
			case 'N':
				output += knight[whitePos]/weight + 3;
				break;
			case 'K':
				
				output += (endgame ? king[whitePos] : king_eg[whitePos])/weight + 100000;
				break;
			case 'Q':
				output += queen[whitePos]/weight + 9;
				break;
			}
		}
	}

	if (inCheck(kingPos[0], 0)) {
		output += 1;
	} else if (inCheck(kingPos[1], 1)) {
		output += -1;
	}
	return output;
}

//Whether the king of the current moving side is safe.
bool Chess::kingSafe(array<int, 3> move) {
	moveToUnmake unmake = moveToUnmake(move, enPassant, canCastle, square[move[1]]);
	bool output = false;
	makeMove(move);
	output = !inCheck(kingPos[1 - whosTurn], 1 - whosTurn);
	unmakeMove(unmake);
	return output;
}

//Whether a specific coordinate is under attack.
bool Chess::inCheck(int pos, int turn) {
	vector<array<int, 3>> moves;
	array<char, 6> enemyPieces = { (turn == 1 ? 'r' : 'R'), (turn == 1 ? 'b' : 'B'), (turn == 1 ? 'q' : 'Q'), (turn == 1 ? 'n' : 'N') , (turn == 1 ? 'k' : 'K') , (turn == 1 ? 'p' : 'P')};
	int dir;
	int amt;
	int prevPos;

	amt = 1;
	dir = -1;
	for (int i = 0; i < 2; i++) {
		while (int((pos + (dir * amt)) / 8) == int(pos / 8) && pos + (dir * amt) >= 0 && pos + (dir * amt) < 64) {
			if (square[pos + (dir * amt)] == ' ') { //Empty space
				amt += 1;
			} else if (square[pos + (dir * amt)] == enemyPieces[0] || square[pos + (dir * amt)] == enemyPieces[2]) { //Enemy piece
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
			} else if (square[pos + (dir * amt)] == enemyPieces[0] || square[pos + (dir * amt)] == enemyPieces[2]) {//Enemy piece
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
				} else if (square[pos + (dir * amt)] == enemyPieces[1] || square[pos + (dir * amt)] == enemyPieces[2]) { //Enemy piece
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
			if (square[pos + to] == enemyPieces[3]) {
				return true;
			}
		}
	}

	//Check for pawns checking
	dir = (turn == 0 ? 9 : -9);
	for (int i = 0; i < 2; i++) {
		if (pos + dir >= 0 && pos + dir < 64) {
			if (abs(((pos + dir) % 8) - (pos % 8)) == 1) {
				if (square[pos + dir] == enemyPieces[5]) {
					return true;
				}
			}
		}
		dir = (turn == 0 ? 7 : -7);
	}
	return false;
}

//Generate all possible moves for whoever's turn it is
void Chess::genMoves() {
	moves.clear();
	vector<array<int, 3>> pieceMoves;
	for (int pos = 63; pos >= 0; pos--) {

		//Ensure only moves for whoever's turn it is get calculated
		if (((square[pos] < 97) ? 1 : 0) != whosTurn || square[pos] == ' ') {
			continue;
		}

		switch (tolower(square[pos])) {
		case 'p':
			pieceMoves = genPawnMoves(pos);
			break;
		case 'r':
			pieceMoves = genSlidingMoves(pos, 0);
			break;
		case 'b':
			pieceMoves = genSlidingMoves(pos, 1);
			break;
		case 'n':
			pieceMoves = genPositionalMoves(pos, {-17, -15, -10, -6, 6, 10, 15, 17});
			break;
		case 'q':
			pieceMoves = genSlidingMoves(pos, 0);
			moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
			pieceMoves = genSlidingMoves(pos, 1);
			break;
		case 'k':
			kingPos[whosTurn] = pos;
			pieceMoves = genPositionalMoves(pos, {-9, -8, -7, -1, 1, 7, 8, 9}, true);
			break;
		}
		moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
	}
}



//Generate a single piece's possible moves
vector<array<int,3>> Chess::getPieceMoves(int pos) {
	vector<array<int, 3>> movess;
	vector<array<int, 3>> pieceMoves;

	//Ensure only moves for whoever's turn it is get calculated
	if (((square[pos] < 97) ? 1 : 0) != whosTurn || square[pos] == ' ') {
		return {};
	}

	switch (tolower(square[pos])) {
	case 'p':
		movess = genPawnMoves(pos);
		break;
	case 'r':
		movess = genSlidingMoves(pos, 0);
		break;
	case 'b':
		movess = genSlidingMoves(pos, 1);
		break;
	case 'n':
		movess = genPositionalMoves(pos, {-17, -15, -10, -6, 6, 10, 15, 17});
		break;
	case 'q':
		//Combine both types of sliding moves
		movess = genSlidingMoves(pos, 0);
		pieceMoves = genSlidingMoves(pos, 1);
		movess.insert(movess.end(), pieceMoves.begin(), pieceMoves.end());
		break;
	case 'k':
		kingPos[whosTurn] = pos;
		movess = genPositionalMoves(pos, {-9, -8, -7, -1, 1, 7, 8, 9}, true);
		break;
	}

	return movess;

}

//Generates pawn moves
vector<array<int, 3>> Chess::genPawnMoves(int pos) {
	vector<array<int, 3>> moves;
	int dir = (whosTurn == 0 ? 1 : -1);
	//Move forward
	if (pos + (8 * dir) < 64 && pos + (8 * dir) >= 0) {
		if (square[pos + (8 * dir)] == ' ') {
			//Check whether is normal move or promotion
			if (int((pos + (8 * dir)) / 8) == ((whosTurn == 0) ? 7 : 0)) {
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
vector<array<int, 3>> Chess::genSlidingMoves(int pos, int type) {
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
vector<array<int, 3>> Chess::genPositionalMoves(int pos, array<int, 8> area, bool king) {
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

	//King-specific castling rights
	if (king) {
		if (canCastle[(whosTurn == 1 ? 0 : 2)]) {
			if (square[kingPos[whosTurn] + 1] == ' ' && square[kingPos[whosTurn] + 2] == ' ' && square[kingPos[whosTurn] + 3] == (whosTurn == 1 ? 'R' : 'r')) {
				if (kingSafe({pos, pos, 0}) && kingSafe({pos, pos+1, 0}) && kingSafe({pos, pos+2, 0})) {
					moves.push_back({ pos, pos + 2, 5 });
				}
			}
		}
		if (canCastle[(whosTurn == 1 ? 1 : 3)]) {
			if (square[kingPos[whosTurn] - 1] == ' ' && square[kingPos[whosTurn] - 2] == ' ' && square[kingPos[whosTurn] - 3] == ' ' && square[kingPos[whosTurn] - 4] == (whosTurn == 1 ? 'R' : 'r')) {
				if (kingSafe({pos, pos, 0}) && kingSafe({pos, pos-1, 0}) && kingSafe({pos, pos-2, 0}) ) {
					moves.push_back({ pos, pos - 2, 4 });
				}
			}
		}
	}
	return moves;
}

//Used in openingBookMove() to get next move in book.
void replace_first(
	std::string& s,
	std::string const& toReplace,
	std::string const& replaceWith
) {
	std::size_t pos = s.find(toReplace);
	if (pos == std::string::npos) return;
	s.replace(pos, toReplace.length(), replaceWith);
}

string Chess::openingBookMove() {
	srand(time(0));
	int tick = false;
	if (openingBookGames.size() == 0) {
		ifstream file("bin/data/openingbooks/all.txt");
		string line;
		retryOpeningBook:
		if (file.is_open()) {
			debugMessage("Opening book found.");
			while (getline(file, line)) {
				openingBookGames.push_back("GAME: " + line);
			}
			file.close();
		} else {
			if (tick == false) {
				debugMessage("Root directory is not immediately outside bin, checking for inside bin.");
				file.open("data/openingbooks/all.txt");
				tick = true;
				goto retryOpeningBook;
			} else {
				debugMessage("Error opening UCI.txt");
				outOfBook = true;
			}
		}
	}
	string::size_type start_pos = 0;
	vector<string> gamesFoundFormatted = {};
	vector<string> gamesFound = {};
	string formatted = "";
	for (auto game : openingBookGames)
	{
		formatted = game;
		if (game.find("GAME: " + thisGamesMoves) != std::string::npos) {
			replace_first(formatted, "GAME: " + thisGamesMoves, "");
			gamesFoundFormatted.push_back(formatted);
			gamesFound.push_back(game);
		}
	}
	openingBookGames = gamesFound;
	if (gamesFound.size() > 0) {
		return gamesFoundFormatted[rand() % gamesFoundFormatted.size()].substr(0, 4);
	} else {
		debugMessage("Exiting opening book.");
		outOfBook = true;
		return "None";
	}
}

//Makes a move using the from square, to square, and move flag.
void Chess::makeMove(array<int, 3> move) {
	int from = move[0];
	int to = move[1];
	int flag = move[2];
	enPassant = -1;
	switch (square[from]) {
	case 'r':
		switch (from) {
		case 0:
			canCastle[3] = false;
			break;
		case 7:
			canCastle[2] = false;
			break;
		default:
			break;
		}
		break;
	case 'R':
		switch (from) {
		case 56:
			canCastle[1] = false;
			break;
		case 63:
			canCastle[0] = false;
			break;
		default:
			break;
		}
		break;
	case 'K':
		kingPos[1] = to;
		canCastle[0] = false;
		canCastle[1] = false;
		break;
	case 'k':
		kingPos[0] = to;
		canCastle[2] = false;
		canCastle[3] = false;
		break;
	default:
		break;
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

//Unmakes a move based on given parameters.
void Chess::unmakeMove(moveToUnmake unmake) {
	canCastle = unmake.canCastle;
	enPassant = unmake.enPassant;
	int from = unmake.move[0];
	int to = unmake.move[1];
	int flag = unmake.move[2];

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
		square[to] = unmake.takenPiece;
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
	//Promotions
	case 6:
	case 7:
	case 8:
	case 9:
		square[from] = (square[to] < 97 ? 'P' : 'p');
		square[to] = unmake.takenPiece;
		break;
	}
	plys -= 1;
	whosTurn = 1 - whosTurn;
}

//Depth-first search of all legal moves, used for perft debugging
int Chess::depthSearch(int depth, int displayAtDepth) {
	int output = 0;
	int showAmount;
	int nonCaptures = 0;
	int captures = 0;
	int enPassants = 0;
	int castles = 0;
	int promotions = 0;
	
	vector<array<int, 3>> moves;
	if (depth == 0) {
		return 1;
	} else {
		moveToUnmake unmake;
		
		genMoves();
		moves = this->moves;
		for (auto move : moves) {
			unmake = moveToUnmake(move, enPassant, canCastle, square[move[1]]);
			makeMove(move);
			showAmount = depthSearch(depth - 1);
			output += showAmount;
			unmakeMove(unmake);
			if (depth == displayAtDepth) {
				cout << posToCoords(move[0]) << posToCoords(move[1]);
				if (move[2] >= 6) {
						switch (move[2]) {
						case 6:
							cout << "q";
							break;
						case 7:
							cout << "r";
							break;
						case 8:
							cout << "b";
							break;
						case 9:
							cout << "n";
							break;
						}
				} 
				cout << ": " << showAmount << "\n";
			}
		}
		return output;
	}
}

//Negated mini-max alpha-beta pruning
double Chess::negaMax(int depth, double alpha, double beta, bool taking) {
	if (depth <= 0 && !taking) {
		
		//Look for checkmate/stalemate
		vector<array<int, 3>> moves;
		int movesFound = 0;
		for (int pos = 0; pos < 64; pos++) {
			moves = getPieceMoves(pos);
			for (auto move : moves) {
				movesFound++;
			}
		}
		if (movesFound == 0) {
			if (!inCheck(kingPos[whosTurn], whosTurn)) {
				return INT_MAX * (whosTurn == 1 ? 1 : -1) * initialDepth;
			} else {
				return INT_MAX * (whosTurn == 1 ? -1 : 1) * initialDepth;
			}
		}
		return evaluate() * (whosTurn == 1 ? 1 : -1) * (panicLevel != 2 ? 1 : 0.7);
	} else {
		moveToUnmake unmake;
		vector<array<int, 3>> moves;
		double value = -DBL_MAX;
		int movesFound = 0;

		for (int i = 0; i < 64; i++) {
			int pos = spiralCoords[i];
			moves = getPieceMoves(pos);
			for (auto move : moves) {
				if (gameover) {
					return 0;
				}
				if (panicLevel > 0 && depth > panicDepth) {
					depth = panicDepth;
				}
				movesFound++;
				unmake = moveToUnmake(move, enPassant, canCastle, square[move[1]]);
				makeMove(move);
				if (panicLevel == 2) {
					value = max(value, -negaMax(depth - 1, -beta, -alpha, false));
				} else {
					value = max(value, -negaMax(depth - 1, -beta, -alpha, (unmake.takenPiece != ' ')));
				}
				unmakeMove(unmake);
				alpha = max(alpha, value);
				if (alpha >= beta) {
					goto outerNegaMax;
				}
			}
		}
		
		if (movesFound == 0) {
			if (!inCheck(kingPos[whosTurn], whosTurn)) {
				return INT_MAX * (whosTurn == 1 ? 1 : -1) * (abs((initialDepth - depth)) != 0 ? abs((initialDepth - depth)) : 1);
			} else {
				return INT_MAX * (whosTurn == 1 ? -1 : 1) * (abs((initialDepth - depth)) != 0 ? abs((initialDepth - depth)) : 1);
			}
		}
		outerNegaMax:
		return value;
	}
}

//Turns the 0-63 coordinate to traditional file-column coordinates (e.g. 0=a8)
string Chess::posToCoords(int pos) {
	int x = (pos % 8) + 1;
	int y = 8 - int(pos / 8);
	char xChar = 96 + x;
	string output = xChar + to_string(y);
	return output;
}

//Turns the traditional file-column coordinates coordinate to 0-63 coordinate
array<int, 2> Chess::coordsToPos(string coords) {
	int fromx, fromy;
	int tox, toy;
	for (int i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			fromx = int(coords[0]) - 97;
			break;
		case 1:
			fromy = 8 - ((coords[1]) - 48);
			break;
		case 2:
			tox = coords[2] - 97;
			break;
		case 3:
			toy = 8 - (coords[3] - 48);
			break;
		}
	}
	int from = (fromy * 8) + fromx;
	int to = (toy * 8) + tox;
	return { from, to };
}


//Makes a move for the bot
void Chess::makeBotMove(double alpha, double beta) {
	
	//Check for book move
	if (!outOfBook) {
		string bookMove = openingBookMove();
		if (bookMove != "None") {
			genMoves();
			array<int, 2> fromto = coordsToPos(bookMove);
			for (auto move : moves) {
				if (move[0] == fromto[0] && move[1] == fromto[1]) {
					thisGamesMoves += bookMove + " ";
					makeMove(move);
					debugMessage("Opening book move made.");
					botMoved = true;
					return;
				}
			}
		} else {

		}
	}
	
	depth = initialDepth;
	botMoved = false;
	vector <Chess> threadedChesss;
	vector <thread> threads;
	moveToUnmake unmake;
	vector<array<int, 3>> moves = {};
	array<int, 3> bestMove = { -1, -1, -1 };
	array<int, 3> nextBestMove = { -1, -1, -1 };
	double init_value = -DBL_MAX;
	double value = init_value;
	double bestValue = value;
	
	depth--; //Decreases by one because at least one depth is always searched outside of negamax().

	for (int i = 0; i < 64; i++) {
		int pos = spiralCoords[i];
		moves = getPieceMoves(pos);

		for (auto move : moves) {
			debugMessage("Looking at move " + posToCoords(move[0]) + " to " + posToCoords(move[1]) + " type " + to_string(move[2]));
			unmake = moveToUnmake(move, enPassant, canCastle, square[move[1]]);

			makeMove(move);
			
			value = max(value, -negaMax(depth, -beta, -alpha, (move[2] == 1 || move[2] == 3)));
			alpha = max(alpha, value);
			unmakeMove(unmake);
			if (value > bestValue) {
				bestValue = value;
				if (bestMove[0] == -1) {
					bestMove = move;
					nextBestMove = bestMove;
				} else {
					nextBestMove = bestMove;
					bestMove = move;
				}
				debugMessage("Best move is " + posToCoords(move[0]) + " to " + posToCoords(move[1]) + " type " + to_string(move[2]));
			}
		}
	}
	outer:
	if (value != init_value && !gameover) {
		debugMessage("Making move " + posToCoords(bestMove[0]) + " " + posToCoords(bestMove[1]) + " " + posToCoords(bestMove[2]) + ", Value: " + to_string(value));
		string moveUCI = posToCoords(bestMove[0]) + posToCoords(bestMove[1]);
		int counter = 0;
		for (auto move : previousMoves) {
			if (moveUCI == move) {
				counter++;
			}
		}
		previousMoves.insert(previousMoves.begin(), moveUCI);
		if (previousMoves.size() > 10) {
			previousMoves.pop_back();
		}
		
		thisGamesMoves += posToCoords(bestMove[0]) + posToCoords(bestMove[1]) + " ";
		if (counter >= 2) {
			debugMessage("Repitition, making next best move.");
			makeMove(nextBestMove);
		} else {
			makeMove(bestMove);
		}
		
	} else {
		debugMessage("Cannot make move.");
	}
	botMoved = true;

}

//Shows the current state of the board in ascii.
void Chess::show() {
	array<array<char, 8>, 8> square;
	int x = -1;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			x += 1;
			square[i][j] = this->square[x];
		}
	}
	cout << "\n";
	string BG = "\033[0;100;";
	string FG = "30m";
	string color;
	string WHITE_BG = "\033[0;107m";
	string BLACK_BG = "\033[40m";
	string letter;
	cout << "     ";
	for (int i = 0; i < 48; i++) {
		cout << WHITE_BG << " " << "\033[0m";
	}
	cout << "\n";
	cout << "     " << WHITE_BG << "  " << "\033[0m";
	for (int i = 0; i < 44; i++) {
		cout << " " << "\033[0m";
	}
	cout << WHITE_BG << "  " << "\033[0m";
	cout << "\n";
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 3; k++) {
			if ((k + 2) % 3 == 0) {
				cout << "  " << to_string(8 - i) << "  ";
			} else {
				cout << "     ";
			}
			cout << WHITE_BG << "  " << BLACK_BG << "  " << "\033[0m";
			for (int j = 0; j < 8; j++) {
				if (BG == "\x1b[48;5;247m") {
					BG = "\x1b[48;5;242m";
				} else {
					BG = "\x1b[48;5;247m";
				}
				if (square[i][j] >= 97) {
					FG = "\x1b[38;5;0m";
				} else {
					FG = "\x1b[38;5;255m";
				}
				color = BG + FG;
				cout << color << "  " << "\033[0m";
				if ((k + 2) % 3 == 0) {
					cout << color << (square[i][j]) << "\033[0m";
				} else {
					cout << color << " " << "\033[0m";
				}
				cout << color << "  " << "\033[0m";
			}
			cout << BLACK_BG << "  " << WHITE_BG << "  " << "\033[0m";
			if (k != 2) {
				cout << "\n";
			}
		}
		if (BG == "\x1b[48;5;247m") {
			BG = "\x1b[48;5;242m";
		} else {
			BG = "\x1b[48;5;247m";
		}
		cout << "\n";
	}
	cout << "     " << WHITE_BG << "  " << "\033[0m";
	for (int i = 0; i < 44; i++) {
		cout << BLACK_BG << " " << "\033[0m";
	}
	cout << WHITE_BG << "  " << "\033[0m";
	cout << "\n";
	cout << "     ";
	for (int i = 0; i < 48; i++) {
		cout << WHITE_BG << " " << "\033[0m";
	}
	cout << "\n\n";
	cout << "         ";
	for (int i = 0; i < 8; i++) {
		cout << "  " << char(64 + i + 1) << "  ";
	}
	cout << "\n\n";
}
