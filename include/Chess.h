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

using namespace std;

class Chess {
public:
	Chess();
	Chess(string fen, array<int, 2> time = {-1, -1});
	array<bool, 4> canCastle = {false, false, false, false}; //K Q k q
	int enPassant = { -1 };
	array<char, 64> square = { ' ' };
	vector<array<int, 3>> moves; //from, to, flag (0=no capture, 1=capture, 2=en-passantable move, 3=en-passant, 4=q castle, 5=k castle, 6,7,8,9=q,r,b,n promotion)
	vector <int> moveValues;
	bool gameStarted;
	int fullMoves = 0;
	int halfMoves = 0;
	int plys = 0;
	int whosTurn = 0;
	bool botMoved = true;
	bool timerHurry = false;
	bool timerDone = false;
	int initialDepth = 5;
	int depth = initialDepth;
	int panicDepth = 3;
	bool gameover = false;
	string thisGamesMoves;
	vector<string> openingBookGames = {};
	bool outOfBook = false;
	vector<string> previousMoves;
	array <int, 2> kingPos;
	void show();
	string genFen();
	string genFenRepitition();
	double evaluate();
	string posToCoords(int pos);
	array<int, 2> coordsToPos(string coords);
	void generateMoves();
	vector<array<int, 3>> generateMove(int pos);
	bool kingSafe(array<int, 3> move);
	bool inCheck(int pos, int turn);
	vector<array<int, 3>> generatePawnMoves(int pos);
	vector<array<int, 3>> generateSlidingMoves(int pos, int type);
	vector<array<int, 3>> generatePositionalMoves(int pos, array<int, 8> area, bool king = false);
	void makeMove(array<int, 3> move);
	void unmakeMove(array<int, 3> moveToUnmake, int unmakeEnPassant, array<bool, 4> unmakeCanCastle, char unmakeTakenPiece);
	int depthSearch(int depth, int displayAtDepth = -1);
	double negaMax(int depth, double alpha, double beta, bool taking=false);
	void makeBotMove(double alpha, double beta);
	string openingBookMove();

};