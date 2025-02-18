#pragma once
#include <vector>
#include <string>
#include <array>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <random>
#include <unordered_map>

using namespace std;

class Chess {
public:
    Chess(); // Default constructor
    Chess(string fen, array<int, 2> time = {-1, -1}); // Overloaded constructor

    enum MoveFlag {
        EMPTY = -1, // Default value
        NONE = 0, // Regular move
        EN_PASSANTABLE = 1, // En-passantable
        Q_CASTLE = 2, // Queenside castle
        K_CASTLE = 3, // Kingside castle
        EN_PASSANT = 4, // En passant
        CAPTURE = 5, // Capture
        N_PROMOTION = 6, // Knight promotion
        B_PROMOTION = 7, // Bishop promotion
        R_PROMOTION = 8, // Rook promotion
        Q_PROMOTION = 9 // Queen promotion
    };

    //Castling rights
    struct CanCastle {
        bool wK; // White king-side castle.
        bool wQ; // White queen-side castle.
        bool bK; // black king-side castle.
        bool bQ; // black queen-side castle.
    };

    // Move, holds from, to, and flag
    struct Move {
        friend bool operator==(const Move& lhs, const Move& rhs)
        {
            return (lhs.from == rhs.from &&
                lhs.to == rhs.to &&
                lhs.flag == rhs.flag);
        }
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
    vector<Move> legalMoves; // Holds all current legal moves.
    bool gameStarted; // Whether or not the game has started
    int fullMoves = 0; // Starts at 1, increases after black moves.
    int halfMoves = 0; // Moves since last pawn move (50 move rule)
    int whosTurn = 0; // whosTurn
    int panicLevel = 0; // How rushed the bot is, managed by driver class since its usually time-dependant.
    int initialDepth = 10; // Initial depth value
    int depth = initialDepth; // Negamax depth value.
    int depthReached = 0;
    int gameStatus = -1; // -1=playing, 0=black won, 1=white won, 2=stalemate
    double evaluation = 0; // Evaluation at-depth reached in bot move.
    string thisGamesMoves; // Moves made this game
    string visualGamesMoves; // Moves made this game but with color codes included.
    vector<string> openingBookGames = {}; // All games in the opening book (narrows down each move)
    bool outOfBook = false; // Whether to continue using the book
    vector<string> previousMoves; // The previous moves made.
    array <int, 2> kingPos; // 0=b, 1=w. Is an array and not struct for easy use alongside whosTurn.
    bool endgame = false; // Whether the game has reached the endgame, used for evaluation.
    bool showDebugMessages = false; // Whether or not to show debug messages
    Move negaMaxResult{ 0,1,EMPTY }; // The result of searching for a best move.
    unordered_map<string, int> positionCount; //Hashmap of how frequently each position has happened.


    
    /**
    * Enables colors in external terminals
    * 
    * @note is ran by default in overloaded Chess() constructor if showDebugMessages==true.
    */
    void enableANSI(); // Allows colors in external terminals

    /**
    * Prints the board in ASCII-form with colors.
    */
    void show(); // Print the board in ASCII in terminal.

    /**
    * Generate FEN of current board
    * 
    * @return A string of the full FEN of the current board
    */
    string genFen();

    /**
    * Generate FEN of the current board for use in three-fold repitition detection.
    * 
    * @return A string of the partial FEN of the current board without en passant, full moves, and half moves.
    */
    string genFenRepitition();

    /**
    * Evaluate the current position from white's perspective
    * 
    * @return A double of the current evaluation (+ good for white, - good for black)
    */
    double evaluate();

    /**
    * Turns the 0-63 coordinate to file-column coordinate (e.g. 0=a8)
    * 
    * @param pos An integer 0-63 coordinate.
    * @return A string of the UCI coordinate.
    */
    string posToCoords(int pos);

    /**
    * Turns the file-column coordinate to 0-63 coordinate (e.g. a8=0)
    * 
    * @param coords A string UCI coordinate.
    * @return An integer 0-63 coordinate.
    */
    array<int, 2> coordsToPos(string coords);

    /**
    * Convert UCI to struct Move (e.g. e2e4={from=52, to=36, flag=NONE})
    * 
    * @param uci A string UCI coordinate
    * @return A Move of the converted UCI coordinate.
    */
    Move UCIToMove(string uci);

    /**
    * Generate all legal moves
    * 
    * @note all legal moves are output to class member vector<Move> legalMoves.
    */
    void genMoves();

    /**
    * Get legal moves for a specific coordinate
    * 
    * @param uci An integer 0-63 coordinate to get legal moves of
    * @return A vector<Move> of legal moves.
    */
    vector<Move> getPieceMoves(int pos);

    /**
    * Check if a certain side's king is safe after a move.
    * 
    * @param move A Move to test for whether the king is safe after.
    * @return A boolean whether or not the king is safe.
    */
    bool kingSafe(Move move);

    /**
    * Check if a certain position is safe depending on who's turn it is.
    * 
    * @param pos An integer 0-63 coordinate to test for whether in check.
    * #param turn An integer for whos turn it is (0=black, 1=white).
    * @return A boolean of whether pos is in check.
    */
    bool inCheck(int pos, int turn);

    /**
    * Generate pawn moves
    * 
    * @param pos An integer 0-63 coordinate position of piece to generate legal moves for.
    * @return A vector<Move> of legal pawn moves.
    */
    vector<Move> genPawnMoves(int pos);

    /**
    * Generate sliding moves
    * 
    * @param pos An integer 0-63 coordinate position of piece to generate legal moves for.
    * @param type An integer for what type of sliding move (0=up/down/left/right, 1=diagonally)
    * @return A vector<Move> of legal sliding moves.
    */
    vector<Move> genSlidingMoves(int pos, int type);

    /**
    * Generate positional moves
    * 
    * @param pos An integer 0-63 coordinate position of piece to generate legal moves for.
    * @param area An array<int, 8> of 0-63 coordinates to check the legality of moving to.
    * @param king A boolean of whether or not moves are being generated for a king
    * @return A vector<Move> of legal positional moves.
    */
    vector<Move> genPositionalMoves(int pos, array<int, 8> area, bool king = false);

    /**
    * Make a move on the board for whoever's turn it is.
    * 
    * @param move A Move to make.
    */
    void makeMove(Move move); // Make a move

    /**
    * Unmake a move.
    * 
    * @param move A MoveUnmake holding a Move to unmake.
    */
    void unmakeMove(MoveUnmake move); // Unmake a move

    /**
    * Depth search all legal moves, used for perft debugging.
    * 
    * @param depth An integer of what depth to search to.
    * @param displayAtDepth An integer of what depth to display legal moves at, initialized at -1
    * @return An integer of how many legal moves for given depth.
    */
    int depthSearch(int depth, int displayAtDepth = -1);

    /**
    * Negated mini-max alpha-beta pruning
    * 
    * @param depth An integer of what depth to search to.
    * @param alpha A double of what the best-case evaluation is.
    * @param beta A double of what the worst-case evaluation is.
    * @param taking A boolean of whether the last move was a taking move.
    * @return A double of what the position's evaluation is.
    */
    double negaMax(int depth, double alpha, double beta, bool taking=false);

    /**
    * Make a move for the bot
    * 
    * @param alpha A double of what the best-case evaluation is.
    * @param beta A double of what the worst-case evaluation is.
    */
    void makeBotMove(double alpha, double beta); // Make a move for the bot

    /**
    * Make an opening-book move.
    * 
    * @return A string of what the opening book move is, "None" if no moves found.
    */
    string openingBookMove();

    /**
    * Show debug message
    * 
    * @param input A string of the message to show
    * @note messages won't be shown if class member showDebugMessages==false.
    */
    void debugMessage(string input);

    /**
    * First layer of searching for a bot move
    * 
    * @return A Move of the best move to make.
    */
    Move iterativeDeepening();

    /**
    * Makes a "null move" for the opponent after being called by iterativeDeepening(). This
    * is effective because it is assumed that passing a turn is always a bad move, so if the
    * position is still bad after the enemy passes the turn, then it is pruned.
    * 
    * @param depth An integer of what depth to search to.
    * @param alpha A double of what the best-case evaluation is.
    * @param beta A double of what the worst-case evaluation is.
    * @param taking A boolean of whether the last move was a taking move.
    * @return A double of what the position's evaluation is.
    */
    double nullMovePruning(int depth, double alpha, double beta, bool taking);

    /**
    * Sorts Moves by their priority
    * 
    * @note sorts by enum values established in MoveFlag.
    * @param a A Move to compare
    * @param b A Move to compare
    * @return whether A is more important than B
    */
    inline bool compareMoves(const Move& a, const Move& b) {
        return static_cast<int>(a.flag) > static_cast<int>(b.flag); // Sort by flag value (higher priority first)
    }
};