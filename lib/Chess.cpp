
#include "../include/Chess.h"
using namespace std;

//Prints debug messages
void Chess::debugMessage(string input) {
    if (showDebugMessages) {
        std::cout << input << "\n";
    }
}

//Default constructor
Chess::Chess() {};

//Allow colors in external consoles
void Chess::enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

//FEN constructor
Chess::Chess(string fen, array<int, 2> time) {
    if (showDebugMessages) {
        enableANSI();
    }
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
                castlingRights.wK = true;
                break;
            case 'Q':
                castlingRights.wQ = true;
                break;
            case 'k':
                castlingRights.bK = true;
                break;
            case 'q':
                castlingRights.bQ = true;
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

    fen += (whosTurn == 1) ? " w " : " b "; //Whos turn

    //Castling rights
    if (castlingRights.wK) { fen += "K"; }
    if (castlingRights.wQ) { fen += "Q"; }
    if (castlingRights.bK) { fen += "k"; }
    if (castlingRights.bQ) { fen += "q"; }

    //Moves & Half moves made
    fen += " ";
    fen += to_string(halfMoves);
    fen += " ";
    fen += to_string(fullMoves);
    return fen;
}

//Generate FEN of current board without en passant, half moves, or full moves.
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
            skip = 0;
        }
    }
    fen += (whosTurn == 1) ? " w " : " b "; //Whos turn

    //Castling rights
    if (castlingRights.wK) { fen += "K"; }
    if (castlingRights.wQ) { fen += "Q"; }
    if (castlingRights.bK) { fen += "k"; }
    if (castlingRights.bQ) { fen += "q"; }

    return fen;
}

// Evaluate the current position from white's perspective (+ good for white, - good for black)
double Chess::evaluate() {

    double output = 0;
    int whitePos;
    int blackPos;
    int wBCount = 0;
    int bBCount = 0;

    //How important the piece-square tables are.
    double weight = 150;

    //Simplified piece-square tables
    int pawn[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0 };

    int pawn_eg[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
        75, 50, 50, 50, 50, 50, 50, 75,
        35, 35, 35, 35, 35, 35, 35, 35,
        25,  25, 25, 25, 25, 25,  25,  25,
        0,  0,  0, 20, 20,  0,  0,  0,
        -30, -20,-20,  -20,  -20,-20, -20,  -30,
        -50, -50, -50,-50,-50, -50, -50,  -50,
        0,  0,  0,  0,  0,  0,  0,  0 };

    int rook[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0 };

    int bishop[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20 };

    int knight[64] = { -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50 };

    int king[64] = { -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20 };

    int king_eg[64] = { -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50 };

    int queen[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20 };

    //Iterate and add values
    for (int col = 0; col < 8; col++) {
        for (int row = 0; row < 8; row++) {
            whitePos = (col * 8) + row;
            blackPos = ((7 - col) * 8) + row;
            switch (square[whitePos]) {
            case 'p':
                output += -((endgame ? pawn_eg[blackPos] : pawn[blackPos])/weight) + -1;
                break;
            case 'r':
                output += -(rook[blackPos] / weight) + -5;
                break;
            case 'b':
                output += -(bishop[blackPos]/weight) + -3;
                bBCount++;
                break;
            case 'n':
                output += -(knight[blackPos]/weight) + -3;
                break;
            case 'k':
                output += -((endgame ? king_eg[blackPos] : king[blackPos]) /weight) + -100000;
                break;
            case 'q':
                output += -(queen[blackPos]/weight) + -9;
                break;
            case 'P':
                output += ((endgame ? pawn_eg[whitePos] : pawn[whitePos])/weight) + 1;
                break;
            case 'R':
                output += (rook[whitePos]/weight) + 5;
                break;
            case 'B':
                output += (bishop[whitePos]/weight) + 3;
                wBCount++;
                break;
            case 'N':
                output += (knight[whitePos]/weight) + 3;
                break;
            case 'K':
                output += ((endgame ? king_eg[whitePos] : king[whitePos])/weight) + 100000;
                break;
            case 'Q':
                output += (queen[whitePos]/weight) + 9;
                break;
            }
        }
    }

    //Bishop pair detection
    if (wBCount == 2) {
        output += 0.2;
    }
    if (bBCount == 2) {
        output += -0.2;
    }

    //Stacked/passed pawn detection / endgame detection.
    int pieceCount = 0;
    int bPCount = 0;
    int wPCount = 0;
    int pos = 7;
    for (int i = 0; i < 64; i++) {
        if (square[pos] == 'p') {
            bPCount++;
        } else if (square[pos] == 'P') {
            wPCount++;
        } else if (square[pos] != ' ') {
            pieceCount++;
        }
        pos += 8;
        if (pos > 63) {
            output += (wPCount - bPCount) * 0.3;
            if (bPCount >= 2) {
                output += 0.25*(bPCount - 1);
            }
            if (wPCount >= 2) {
                output += -0.25*(wPCount - 1);
            }
            wPCount = 0;
            bPCount = 0;
            pos -= 65;
        }
    }
    if (pieceCount <= 6) {
        endgame = true;
    } else {
        endgame = false;
    }
    return output;
}

//Whether the king of the current moving side is safe.
bool Chess::kingSafe(Move move) {
    MoveUnmake unmake { move, enPassant, castlingRights, square[move.to] };
    bool output = false;
    makeMove(move);
    output = !inCheck(kingPos[1 - whosTurn], 1 - whosTurn);
    unmakeMove(unmake);
    return output;
}

//Whether a specific coordinate is under attack.
bool Chess::inCheck(int pos, int turn) {
    vector<Move> moves;
    array<char, 6> enemyPieces = { 'R', 'B', 'Q', 'N', 'K', 'P'};
    if (turn == 1) {
        enemyPieces = { 'r', 'b', 'q', 'n', 'k', 'p'};
    }
    int amt;
    int prevPos;

    //Non-diagonal sliding moves
    for (int dir : {-1, 1, -8, 8}) {
        amt = 1;
        while (((dir == -1 || dir == 1) ? 
            (int((pos + (dir * amt)) / 8) == int(pos / 8) && pos + (dir * amt) >= 0 && pos + (dir * amt) < 64) : //Horizontal conditions
            (pos + (dir * amt) >= 0 && pos + (dir * amt) < 64))) { //Vertical conditions
            if (square[pos + (dir * amt)] == ' ') { //Empty space
                amt += 1;
            } else if (square[pos + (dir * amt)] == enemyPieces[0] || square[pos + (dir * amt)] == enemyPieces[2]) { //Enemy piece
                return true;
            } else { //Own piece
                break;
            }
        }
    }

    //Diagonal sliding moves
    for (int dir : { -9, 9, -7, 7 }) {
        amt = 1;
        prevPos = pos;
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
    for (int dir : { 9*(turn == 0 ? 1 : -1), 7*(turn == 0 ? 1 : -1) }) {
        if (pos + dir >= 0 && pos + dir < 64) {
            if (abs(((pos + dir) % 8) - (pos % 8)) == 1) {
                if (square[pos + dir] == enemyPieces[5]) {
                    return true;
                }
            }
        }
    }

    return false;
}

//Generate all possible moves for whoever's turn it is
void Chess::genMoves() {
    legalMoves.clear();
    vector<Move> pieceMoves;
    for (int pos = 63; pos >= 0; pos--) {

        //Ensure only moves for whoever's turn it is get calculated
        if (((square[pos] < 97) ? 1 : 0) != whosTurn || square[pos] == ' ') {
            continue;
        }

        switch (tolower(square[pos])) {
        case 'p': //Pawn moves
            pieceMoves = genPawnMoves(pos);
            break;
        case 'r': //Non-diagonal sliding moves
            pieceMoves = genSlidingMoves(pos, 0);
            break;
        case 'b': //Diagonal sliding moves
            pieceMoves = genSlidingMoves(pos, 1);
            break;
        case 'n': //Use knight's relative movement coordinates
            pieceMoves = genPositionalMoves(pos, {-17, -15, -10, -6, 6, 10, 15, 17});
            break;
        case 'q': //Combine both types of sliding moves
            pieceMoves = genSlidingMoves(pos, 0);
            legalMoves.insert(legalMoves.end(), pieceMoves.begin(), pieceMoves.end());
            pieceMoves = genSlidingMoves(pos, 1);
            break;
        case 'k': //Use king's relative movement coordinates
            kingPos[whosTurn] = pos;
            pieceMoves = genPositionalMoves(pos, {-9, -8, -7, -1, 1, 7, 8, 9}, true);
            break;
        }

        //Combine all legal moves.
        legalMoves.insert(legalMoves.end(), pieceMoves.begin(), pieceMoves.end());
    }
}



//Generate a single piece's possible moves
vector<Chess::Move> Chess::getPieceMoves(int pos) {
    vector<Move> movess;
    vector<Move> pieceMoves;

    //Ensure only moves for whoever's turn it is get calculated
    if (((square[pos] < 97) ? 1 : 0) != whosTurn || square[pos] == ' ') {
        return {};
    }

    switch (tolower(square[pos])) {
    case 'p': //Pawn moves
        movess = genPawnMoves(pos);
        break;
    case 'r': //Non-diagonal sliding moves
        movess = genSlidingMoves(pos, 0);
        break;
    case 'b': //Diagonal sliding moves
        movess = genSlidingMoves(pos, 1);
        break;
    case 'n': //Use knight's relative movement coordinates
        movess = genPositionalMoves(pos, {-17, -15, -10, -6, 6, 10, 15, 17});
        break;
    case 'q': //Combine both types of sliding moves
        movess = genSlidingMoves(pos, 0);
        pieceMoves = genSlidingMoves(pos, 1);
        movess.insert(movess.end(), pieceMoves.begin(), pieceMoves.end());
        break;
    case 'k': //Use king's relative movement coordinates
        kingPos[whosTurn] = pos;
        movess = genPositionalMoves(pos, {-9, -8, -7, -1, 1, 7, 8, 9}, true);
        break;
    }

    return movess;

}

//Generates pawn moves
vector<Chess::Move> Chess::genPawnMoves(int pos) {
    vector<Move> moves;
    int dir = (whosTurn == 0 ? 1 : -1);
    //Move forward
    if (pos + (8 * dir) < 64 && pos + (8 * dir) >= 0) {
        if (square[pos + (8 * dir)] == ' ') {
            //Check whether is normal move or promotion
            if (int((pos + (8 * dir)) / 8) == ((whosTurn == 0) ? 7 : 0)) {
                if (kingSafe({ pos, pos + (8 * dir), Q_PROMOTION })) {
                    moves.push_back({ pos, pos + (8 * dir), Q_PROMOTION });
                    moves.push_back({ pos, pos + (8 * dir), R_PROMOTION });
                    moves.push_back({ pos, pos + (8 * dir), B_PROMOTION });
                    moves.push_back({ pos, pos + (8 * dir), N_PROMOTION });
                }
            } else {
                
                if (kingSafe({ pos, pos + (8 * dir), NONE })) {
                    moves.push_back({ pos, pos + (8 * dir), NONE });
                }
                //Double jump forward
                if (pos + (16 * dir) < 64 && pos + (16 * dir) >= 0 && int(pos / 8) == ((whosTurn == 0) ? 1 : 6)) {
                    if (square[pos + (16 * dir)] == ' ') {
                        if (kingSafe({ pos, pos + (16 * dir), EN_PASSANTABLE })) {
                            moves.push_back({ pos, pos + (16 * dir), EN_PASSANTABLE });
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
                    if (kingSafe({pos, cap, CAPTURE})) {
                        moves.push_back({ pos, cap, CAPTURE });
                    }
                } else {
                    if (kingSafe({ pos, cap, Q_PROMOTION })) {
                        moves.push_back({ pos, cap, Q_PROMOTION });
                        moves.push_back({ pos, cap, R_PROMOTION });
                        moves.push_back({ pos, cap, B_PROMOTION });
                        moves.push_back({ pos, cap, N_PROMOTION });
                    }
                }
            } else if(enPassant == cap) {
                if (square[enPassant + 8 * dir] != square[pos] && kingSafe({ pos, cap, EN_PASSANT })) {
                    moves.push_back({ pos, cap, EN_PASSANT });
                }
            }
        }
        //Update to capture to the right
        cap = pos + (8 * dir) + 1;
    }

    return moves;
}

//Generates both types of sliding moves
vector<Chess::Move> Chess::genSlidingMoves(int pos, int type) {
    vector<Move> moves;
    int amt;
    int dir;
    int prevPos;

    //Two different types of sliding moves, non-diagonal and diagonal.
    switch (type) {

    //Horizontal/vertical sliding moves
    case 0:
        for(int dir : {-1, 1, -8, 8}) {
            amt = 1;
            while ((dir == -1 || dir == 1 ?
                (int((pos + (dir*amt)) / 8) == int(pos / 8) && pos+(dir*amt) >= 0 && pos+(dir*amt) < 64) : //Horizontal conditions
                (pos + (dir*amt) >= 0 && pos + (dir*amt) < 64))) { //Vertical conditions
                if (square[pos + (dir*amt)] == ' ') { //Empty space
                    if (kingSafe({ pos, pos + (dir * amt), NONE })) {
                        moves.push_back({ pos, pos + (dir * amt), NONE });
                    }
                    amt += 1;
                } else if ((square[pos+(dir*amt)] < 97 ? 1 : 0) != whosTurn) { //Enemy piece
                    if (kingSafe({ pos, pos + (dir * amt), CAPTURE })) {
                        moves.push_back({ pos, pos + (dir * amt), CAPTURE });
                    }
                    break;
                } else { //Own piece
                    break;
                }
            }
        }
        break;

    //Diagonal sliding moves
    case 1:
        for (int dir : {-9, 9, -7, 7}) {
            amt = 1;
            prevPos = pos;
            while (pos + (dir * amt) >= 0 && pos + (dir * amt) < 64 && abs(((pos + (dir * amt)) % 8) - (prevPos % 8)) == 1) {
                if (square[pos + (dir * amt)] == ' ') { //Empty space
                    if (kingSafe({ pos, pos + (dir * amt), NONE })) {
                        moves.push_back({ pos, pos + (dir * amt), NONE });
                    }
                    prevPos = pos + (dir * amt);
                    amt += 1;
                } else if ((square[pos + (dir * amt)] < 97 ? 1 : 0) != whosTurn) { //Enemy piece
                    if (kingSafe({ pos, pos + (dir * amt), CAPTURE })) {
                        moves.push_back({ pos, pos + (dir * amt), CAPTURE });
                    }
                    break;
                } else { //Own piece
                    break;
                }
            }
        }
        break;
    }

    return moves;
}

//Generates legal moves from a given list of positions.
vector<Chess::Move> Chess::genPositionalMoves(int pos, array<int, 8> area, bool king) {
    vector <Move> moves;
    for (int to : area) {
        //Check bounds
        if (pos + to >= 0 && pos + to < 64 && abs(((pos + to) % 8) - (pos % 8)) <= 2) {
            if (square[pos + to] == ' ') {
                if (kingSafe({ pos, pos + to, NONE })) {
                    moves.push_back({ pos, pos + to, NONE });
                }
            } else if ((square[pos + to] < 97 ? 1 : 0) != whosTurn) {
                if (kingSafe({ pos, pos + to, CAPTURE })) {
                    moves.push_back({ pos, pos + to, CAPTURE });
                }
            }
        }
    }

    //King-specific castling rights
    if (king) {
        if (whosTurn == 1 ? castlingRights.wK : castlingRights.bK) {
            if (square[kingPos[whosTurn] + 1] == ' ' && square[kingPos[whosTurn] + 2] == ' ' && square[kingPos[whosTurn] + 3] == (whosTurn == 1 ? 'R' : 'r')) {
                if (kingSafe({pos, pos, NONE}) && kingSafe({pos, pos+1, NONE}) && kingSafe({pos, pos+2, NONE})) {
                    moves.push_back({ pos, pos + 2, K_CASTLE });
                }
            }
        }
        if (whosTurn == 1 ? castlingRights.wQ : castlingRights.bQ) {
            if (square[kingPos[whosTurn] - 1] == ' ' && square[kingPos[whosTurn] - 2] == ' ' && square[kingPos[whosTurn] - 3] == ' ' && square[kingPos[whosTurn] - 4] == (whosTurn == 1 ? 'R' : 'r')) {
                if (kingSafe({pos, pos, NONE}) && kingSafe({pos, pos-1, NONE}) && kingSafe({pos, pos-2, NONE}) ) {
                    moves.push_back({ pos, pos - 2, Q_CASTLE });
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
        ifstream file("bin/data/openingbooks/book.txt");
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
                file.open("data/openingbooks/book.txt");
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
        outOfBook = true;
        return "None";
    }
}

//Makes a move using the from square, to square, and move flag.
void Chess::makeMove(Move move) {
    enPassant = -1;

    //Castling rights
    switch (square[move.from]) {
    case 'r':
        switch (move.from) {
        case 0:
            castlingRights.bQ = false;
            break;
        case 7:
            castlingRights.bK = false;
            break;
        default:
            break;
        }
        break;
    case 'R':
        switch (move.from) {
        case 56:
            castlingRights.wQ = false;
            break;
        case 63:
            castlingRights.wK = false;
            break;
        default:
            break;
        }
        break;
    case 'K':
        kingPos[1] = move.to;
        castlingRights.wK = false;
        castlingRights.wQ = false;
        break;
    case 'k':
        kingPos[0] = move.to;
        castlingRights.bK = false;
        castlingRights.bQ = false;
        break;
    default:
        break;
    }

    //How to undo depends on type of move
    switch (move.flag) {
    //Non-capturing move
    case NONE:
        swap(square[move.from], square[move.to]);
        break;
    //Capturing move
    case CAPTURE:
        square[move.to] = square[move.from];
        square[move.from] = ' ';
        break;
    //En-passantable move
    case EN_PASSANTABLE:
        enPassant = move.to + ((square[move.from] < 97 ? 8 : -8));
        swap(square[move.from], square[move.to]);
        break;
    //En-passant
    case EN_PASSANT:
        swap(square[move.from], square[move.to]);
        square[move.to + ((square[move.to] < 97 ? 8 : -8))] = ' ';
        break;
    //Queen-side castle
    case Q_CASTLE:
        swap(square[move.to], square[move.from]);
        swap(square[move.to-2], square[move.from-1]);
        break;
    //King-side castle
    case K_CASTLE:
        swap(square[move.to], square[move.from]);
        swap(square[move.to+1], square[move.from+1]);
        break;
    //Queen promotion
    case Q_PROMOTION:
        square[move.to] = (square[move.from] < 97 ? 'Q' : 'q');
        square[move.from] = ' ';
        break;
    //Rook promotion
    case R_PROMOTION:
        square[move.to] = (square[move.from] < 97 ? 'R' : 'r');
        square[move.from] = ' ';
        break;
    //Bishop promotion
    case B_PROMOTION:
        square[move.to] = (square[move.from] < 97 ? 'B' : 'b');
        square[move.from] = ' ';
        break;
    //Knight promotion
    case N_PROMOTION:
        square[move.to] = (square[move.from] < 97 ? 'N' : 'n');
        square[move.from] = ' ';
        break;
    }
    whosTurn = 1 - whosTurn;

}

//Unmakes a move based on given parameters.
void Chess::unmakeMove(MoveUnmake unmake) {
    castlingRights = unmake.castlingRights;
    enPassant = unmake.enPassant;
    if (tolower(square[unmake.move.to]) == 'k') {
        kingPos[1 - whosTurn] = unmake.move.from;
    }

    switch (unmake.move.flag) {
    case NONE: //Non-capturing move
    case EN_PASSANTABLE: //En-passantable move
        swap(square[unmake.move.from], square[unmake.move.to]);
        break;
        //Capturing move
    case CAPTURE:
        square[unmake.move.from] = square[unmake.move.to];
        square[unmake.move.to] = unmake.takenPiece;
        break;
        //En-passant
    case EN_PASSANT:
        swap(square[unmake.move.from], square[unmake.move.to]);
        square[unmake.move.to + ((square[unmake.move.from] < 97 ? 8 : -8))] = (square[unmake.move.from] < 97 ? 'p' : 'P');
        break;
        //Queen-side castle
    case Q_CASTLE:
        swap(square[unmake.move.to], square[unmake.move.from]);
        swap(square[unmake.move.to - 2], square[unmake.move.from - 1]);
        break;
        //King-side castle
    case K_CASTLE:
        swap(square[unmake.move.to], square[unmake.move.from]);
        swap(square[unmake.move.to + 1], square[unmake.move.from + 1]);
        break;
    //Promotions
    case Q_PROMOTION:
    case R_PROMOTION:
    case B_PROMOTION:
    case N_PROMOTION:
        square[unmake.move.from] = (square[unmake.move.to] < 97 ? 'P' : 'p');
        square[unmake.move.to] = unmake.takenPiece;
        break;
    }
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
    
    vector<Move> moves;
    if (depth == 0) {
        return 1;
    } else {
        genMoves();
        moves = legalMoves;
        for (auto move : moves) {
            MoveUnmake unmake { move, enPassant, castlingRights, square[move.to] };
            makeMove(move);
            showAmount = depthSearch(depth - 1);
            output += showAmount;
            unmakeMove(unmake);
            if (depth == displayAtDepth) {
                std::cout << posToCoords(move.from) << posToCoords(move.to);
                if (move.flag >= 6) {
                        switch (move.flag) {
                        case Q_PROMOTION:
                            std::cout << "q";
                            break;
                        case R_PROMOTION:
                            std::cout << "r";
                            break;
                        case B_PROMOTION:
                            std::cout << "b";
                            break;
                        case N_PROMOTION:
                            std::cout << "n";
                            break;
                        }
                } 
                std::cout << ": " << showAmount << "\n";
            }
        }
        return output;
    }
}

//Negated mini-max alpha-beta pruning
double Chess::negaMax(int depth, double alpha, double beta, bool taking) {

    if (panicLevel == 2 && depthReached > 3) {
        return 0;
    }
    
    //Check for checkmate/stalemate
    genMoves();
    if (legalMoves.empty()) {
        if (inCheck(kingPos[whosTurn], whosTurn)) {
            return -10000 - depth;
        } else {
            return 0;
        }
    }

    //Base case, returns relative evaluation
    if (depth <= 0 && !taking) {
        return evaluate() *
            (whosTurn == 1 ? 1 : -1);
    }

    //Sort moves by estimated importance per compareMoves()
    vector<Move> moves = legalMoves;
    std::sort(moves.begin(), moves.end(), [this](const Move& a, const Move& b) {
        return this->compareMoves(a, b);
        });

    //Standard NegaMax
    double value = -INT_MAX;
    double score;
    string fen = "";
    for (const auto& move : moves) {

        //Skip past-depth-0-evaluation if moves isnt non-pawn capturing non-pawn.
        if (taking && depth <= 0 && (tolower(square[move.to]) == 'p' || tolower(square[move.from]) == 'p')) {
            continue;
        }

        //Remember the move and make it
        MoveUnmake unmake = {move, enPassant, castlingRights, square[move.to]};
        makeMove(move);

        //Stalemate by repitition checks
        fen = genFenRepitition();
        positionCount[fen]++;
        if (positionCount[fen] == 3) {
            score = 0;
        } else {

            //Continue recursion to enemy's move
            score = -negaMax(depth - 1, -beta, -alpha, (unmake.takenPiece != ' '));
        }

        //Unmake the move and undo repitition checks
        unmakeMove(unmake);
        if (--positionCount[fen] == 0) {
            positionCount.erase(fen);
        }

        //Maximize score
        value = max(value, score);
        alpha = max(alpha, value);

        //Prune if terrible
        if (alpha >= beta) {
            break;
        }
    }

    return value;
}

//Makes "null moves" to quickly prune bad positions or continue with negaMax().
double Chess::nullMovePruning(int depth, double alpha, double beta, bool taking) {

    //If null moves are too risky, continue with NegaMax.
    genMoves();
    if (endgame || legalMoves.size() < 5 || inCheck(kingPos[whosTurn], whosTurn)) {
        return negaMax(depth, alpha, beta, taking);
    }

    whosTurn = 1 - whosTurn; // Skip the current move
    double nullEval = -negaMax(max(0, depth - 2), -beta, -beta + 1);
    whosTurn = 1 - whosTurn; // Undo skip of move.

    //If terrible, return.
    if (nullEval >= beta) {
        return beta;
    }

    //If not terrible, continue with NegaMax.
    return negaMax(depth, alpha, beta, taking);
}

//Basic iterative deepending, calling nullMovePruning() for first depth.
Chess::Move Chess::iterativeDeepening() {
    int depthReached = 0; //Used for debugging

    //Check for checkmate/stalemate.
    genMoves();
    if (legalMoves.size() == 0) {
        return { 0,0,EMPTY };
    } else if (legalMoves.size() == 1) {
        return legalMoves[0];
    }

    //The best move, initialized to whatever's first in case nullMovePruning() prunes too harshly.
    Move bestMove = legalMoves[0];

    //Generate and sort moves by estimated importance per compareMoves()
    genMoves();
    vector<Move> moves = legalMoves;
    std::sort(moves.begin(), moves.end(), [this](const Move& a, const Move& b) {
        return this->compareMoves(a, b);
        });

    //Iterate through depths.
    for (int depth = 3; depth <= initialDepth; ++depth) {
        depthReached = depth;
        
        //Prioritize the last iteration's best found move if it isn't already first.
        if (depth > 3) {

            //Delete best move from it's current position.
            moves.erase(remove(moves.begin(), moves.end(), bestMove), moves.end());

            //Insert best move to front of vector.
            moves.insert(moves.begin(), bestMove);
        }
        
        
        //Escape if too panicked.
        if (panicLevel != 0) {
            break;
        }

        //Initialize move evaluations.
        double alpha = -INT_MAX;
        double beta = INT_MAX;
        double bestEval = -INT_MAX;
        double eval = 0;
        Move currentBestMove = bestMove;
        string fen = "";

        //Iterate through moves
        for (const auto& move : moves) {

            //Remember and make the move
            MoveUnmake unmake={ move, enPassant, castlingRights, square[move.to] };
            makeMove(move);

            //Stalemate by repitition checks
            fen = genFenRepitition();
            positionCount[fen]++;
            if (positionCount[fen] >= 2) {
                eval = 0;
            } else {

                //Continue via null move pruning.
                eval = -nullMovePruning(depth - 1, -beta, -alpha, (unmake.takenPiece != ' '));
            }

            //Unmake the move and undo repitition checks
            if (--positionCount[fen] == 0) {
                positionCount.erase(fen);
            }
            unmakeMove(unmake);

            //Escape if too panicked.
            if (panicLevel != 0) {
                debugMessage("Depth reached: " + to_string(depthReached));
                return bestMove;
            }

            //Update the best move
            if (eval > bestEval) {
                evaluation = eval*(whosTurn == 1 ? 1 : -1);
                bestEval = eval;
                currentBestMove = move;
            }

            //Maximize and check for possible skips.
            alpha = max(alpha, eval);
            if (alpha >= beta) {
                break;
            }
        }

        //Update best move if not panicked.
        if (panicLevel == 0) {
            depthReached = depth;
            bestMove = currentBestMove;
        }
    }

    debugMessage("Depth reached: " + to_string(depthReached));
    return bestMove;
}

//Turns the 0-63 coordinate to traditional file-column coordinates (e.g. 0=a8)
string Chess::posToCoords(int pos) {
    int x = (pos % 8) + 1;
    int y = 8 - int(pos / 8);
    char xChar = 96 + x;
    string output = xChar + to_string(y);
    return output;
}

//Convert UCI (e.g. e2e4) to struct Move (e.g. {from=52, to=36, flag=NONE})
Chess::Move Chess::UCIToMove(string uci) {
    if (uci.length() == 4) {
        array<int, 2> fromto = coordsToPos(uci);
        genMoves();
        for (auto move : legalMoves) {
            if (fromto[0] == move.from && fromto[1] == move.to) {
                return move;
            }
        }
    } else {
        array<int, 2> fromto = coordsToPos(uci.substr(0,4));
        genMoves();
        for (auto move : legalMoves) {
            if (fromto[0] == move.from && fromto[1] == move.to) {
                if (uci.substr(4, 1) == "q" && move.flag == Q_PROMOTION) {
                    return move;
                } else if (uci.substr(4, 1) == "r" && move.flag == R_PROMOTION) {
                    return move;
                } else if (uci.substr(4, 1) == "b" && move.flag == B_PROMOTION) {
                    return move;
                } else if (uci.substr(4, 1) == "n" && move.flag == N_PROMOTION) {
                    return move;
                }
            }
        }
    }
    return { 0,0,EMPTY };
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

    //For debugging
    if (whosTurn == 0) {
        debugMessage("\nBLACK MOVING");
    } else {
        debugMessage("\n\033[30m\033[47mWHITE MOVING\033[0m");
    }
    
    //Check for book move
    if (!outOfBook) {
        string bookMove = openingBookMove();
        if (bookMove != "None" && bookMove != "") {
            genMoves();
            array<int, 2> fromTo = coordsToPos(bookMove);
            for (Move move : legalMoves) {
                if (move.from == fromTo[0] && move.to == fromTo[1]) {
                    if (whosTurn == 1) {
                        visualGamesMoves += "\033[30m\033[47m";
                    }
                    visualGamesMoves += bookMove + "\033[0m ";
                    thisGamesMoves += bookMove + " ";
                    makeMove(move);
                    debugMessage("Opening book move made");
                    negaMaxResult = move;

                    //Major moves that capture, castle, or promote invalidate all previous possible repititions.
                    if (move.flag != NONE && move.flag != EN_PASSANTABLE) {
                        positionCount.clear();
                    }
                    positionCount[genFenRepitition()]++;

                    return;
                }
            }
        }
    }

    //Initialize values
    negaMaxResult = { 0, 1, EMPTY };
    depth = initialDepth;

    //Get best move
    negaMaxResult = iterativeDeepening();

    //Check if no move is available.
    if (negaMaxResult.flag == EMPTY) {
        debugMessage("Gameover");
        if (inCheck(kingPos[whosTurn], whosTurn)) {
            gameStatus = 1 - whosTurn;
        } else {
            gameStatus = 2;
        }
        return;
    }

    //For debugging visuals
    if (whosTurn == 1) {
        visualGamesMoves += "\033[30m\033[47m";
    }
    visualGamesMoves += posToCoords(negaMaxResult.from) + posToCoords(negaMaxResult.to) + "\033[0m ";

    //For book-move detection
    thisGamesMoves += posToCoords(negaMaxResult.from) + posToCoords(negaMaxResult.to) + " ";

    //Make the best move
    makeMove(negaMaxResult);

    //Major moves that capture, castle, or promote invalidate all previous possible repititions.
    if (negaMaxResult.flag != NONE && negaMaxResult.flag != EN_PASSANTABLE) {
        positionCount.clear();
    }
    positionCount[genFenRepitition()]++;
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
    std::cout << "\n";
    string BG = "\033[0;100;";
    string FG = "30m";
    string color;
    string WHITE_BG = "\033[0;107m";
    string BLACK_BG = "\033[40m";
    string letter;
    std::cout << "     ";
    for (int i = 0; i < 48; i++) {
        std::cout << WHITE_BG << " " << "\033[0m";
    }
    std::cout << "\n";
    std::cout << "     " << WHITE_BG << "  " << "\033[0m";
    for (int i = 0; i < 44; i++) {
        std::cout << " " << "\033[0m";
    }
    std::cout << WHITE_BG << "  " << "\033[0m";
    std::cout << "\n";
    for (int i = 0; i < 8; i++) {
        for (int k = 0; k < 3; k++) {
            if ((k + 2) % 3 == 0) {
                std::cout << "  " << to_string(8 - i) << "  ";
            } else {
                std::cout << "     ";
            }
            std::cout << WHITE_BG << "  " << BLACK_BG << "  " << "\033[0m";
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
                std::cout << color << "  " << "\033[0m";
                if ((k + 2) % 3 == 0) {
                    std::cout << color << (square[i][j]) << "\033[0m";
                } else {
                    std::cout << color << " " << "\033[0m";
                }
                std::cout << color << "  " << "\033[0m";
            }
            std::cout << BLACK_BG << "  " << WHITE_BG << "  " << "\033[0m";
            if (k != 2) {
                std::cout << "\n";
            }
        }
        if (BG == "\x1b[48;5;247m") {
            BG = "\x1b[48;5;242m";
        } else {
            BG = "\x1b[48;5;247m";
        }
        std::cout << "\n";
    }
    std::cout << "     " << WHITE_BG << "  " << "\033[0m";
    for (int i = 0; i < 44; i++) {
        std::cout << BLACK_BG << " " << "\033[0m";
    }
    std::cout << WHITE_BG << "  " << "\033[0m";
    std::cout << "\n";
    std::cout << "     ";
    for (int i = 0; i < 48; i++) {
        std::cout << WHITE_BG << " " << "\033[0m";
    }
    std::cout << "\n\n";
    std::cout << "         ";
    for (int i = 0; i < 8; i++) {
        std::cout << "  " << char(64 + i + 1) << "  ";
    }
    std::cout << "\n\n";
}
