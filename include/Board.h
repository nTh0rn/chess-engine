#pragma once
#include <vector>
#include <string>
#include <array>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
using namespace std;

class Board {
public:
	Board();
	Board(string fen);
	bool canCastle[4] = {false, false, false, false}; //K Q k q
	bool checks[2] = { false, false }; //white, black
	bool checkmate = false;
	int enPassant = { -1 };
	char square[64] = { ' ' };
	vector<array<int, 3>> moves; //from, to, flag (0=no capture, 1=capture, 2=en-passantable move, 3=en-passant, 4=q castle, 5=k castle, 6,7,8,9=q,r,b,n promotion)
	int fullMoves = 0;
	int halfMoves = 0;
	int plys = 0;
	int whosTurn = 0;


	string genFen();
	void generateMoves();
	bool kingSafe();
	bool inCheck(int pos);
	vector<array<int, 3>> generatePawnMoves(int pos);
	vector<array<int, 3>> generateSlidingMoves(int pos, int type);
	vector<array<int, 3>> generatePositionMoves(int pos, array<int, 8> area);

};