#pragma once
#include <vector>
#include <string>
#include <array>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <atomic>
#include <limits>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <optional>
#include <cstdint>
#include <random>
#include <unordered_map>

using namespace std;

class Chess {
public:
    Chess(); // Default constructor
    Chess(string fen, array<int, 2> time = {-1, -1}); // Overloaded constructor

    enum MoveFlag {
        EMPTY = -1,
        NONE = 0,                // Regular move
        EN_PASSANTABLE = 1,      // En-passantable
        Q_CASTLE = 2,    // Queenside castle
        K_CASTLE = 3,     // Kingside castle
        EN_PASSANT = 4,          // En passant
        CAPTURE = 5,             // Capture
        N_PROMOTION = 6,    // Knight promotion
        B_PROMOTION = 7,    // Bishop promotion
        R_PROMOTION = 8,      // Rook promotion
        Q_PROMOTION = 9      // Queen promotion
    };

    //Castling rights
    struct CanCastle {
        bool wK;
        bool wQ;
        bool bK;
        bool bQ;
    };

    // Move, holds from, to, and flag
    struct Move {
        int from;
        int to;
        MoveFlag flag;
    };

    // Holds Move, en passant position, castling rights, and the piece taken.
    struct MoveUnmake {
        Move move;
        int enPassant;
        CanCastle castlingRights;
        char takenPiece;
    };
    
    struct CanCastle castlingRights {false, false, false, false}; // Global castling rights
    int enPassant = -1; // En passant coordinate
    array<char, 64> square = { ' ' }; // Board pieces
    array<int, 64> spiralCoords = { 0 }; // Spiral-coordinates, generated in genSpiral().
    vector<Move> legalMoves; // Holds all current legal moves.
    bool gameStarted; // Whether or not the game has started
    int fullMoves = 0; // Starts at 1, increases after black moves.
    int halfMoves = 0; // Moves since last pawn move (50 move rule)
    int whosTurn = 0; // whosTurn
    int panicLevel = 0; // How rushed the bot is, managed by driver class since its usually time-dependant.
    int initialDepth = 6; // Initial depth value
    int depth = initialDepth; // Negamax depth value.
    int depthCeiling = initialDepth;
    int panicDepth = 4; // Panic level 3 depth
    int gameStatus = -1; // -1=playing, 0=black won, 1=white won, 2=stalemate
    double evaluation = 0;
    vector<string> debugMoveColors;
    string thisGamesMoves; // Moves made this game
    string visualGamesMoves;
    vector<string> openingBookGames = {}; // All games in the opening book (narrows down each move)
    bool outOfBook = false; // Whether to continue using the book
    vector<string> previousMoves; // The previous moves made.
    array <int, 2> kingPos; // 0=b, 1=w. Is an array and not struct for easy use alongside whosTurn.
    bool endgame = false; // Whether the game has reached the endgame, used for evaluation.
    bool showDebugMessages = false; // Whether or not to show debug messages
    Move negaMaxResult{ 0,1,EMPTY };
    unordered_map<string, int> positionCount;
    void enableANSI();
    void show();
    string genFen();
    string genFenRepitition();
    double evaluate();
    string posToCoords(int pos);
    array<int, 2> coordsToPos(string coords);
    Move UCIToMove(string uci);
    void genMoves();
    vector<Move> getPieceMoves(int pos);
    bool kingSafe(Move move);
    bool inCheck(int pos, int turn);
    vector<Move> genPawnMoves(int pos);
    vector<Move> genSlidingMoves(int pos, int type);
    vector<Move> genPositionalMoves(int pos, array<int, 8> area, bool king = false);
    void makeMove(Move move);
    void unmakeMove(MoveUnmake move);
    int depthSearch(int depth, int displayAtDepth = -1);
    double negaMax(int depth, double alpha, double beta, bool taking=false, bool root=false);
    void makeBotMove(double alpha, double beta);
    void genSpiral();
    string openingBookMove();
    void debugMessage(string input);
    Move iterativeDeepening();
    double nullMovePruning(int depth, double alpha, double beta, bool taking);
    inline bool compareMoves(const Move& a, const Move& b) {
        return static_cast<int>(a.flag) > static_cast<int>(b.flag); // Sort by flag value (higher priority first)
    }
};