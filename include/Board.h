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
	array<bool, 4> canCastle = {false, false, false, false}; //K Q k q
	int enPassant = { -1 };
	char square[64] = { ' ' };
	vector<array<int, 3>> moves; //from, to, flag (0=no capture, 1=capture, 2=en-passantable move, 3=en-passant, 4=q castle, 5=k castle, 6,7,8,9=q,r,b,n promotion)
	int fullMoves = 0;
	int halfMoves = 0;
	int plys = 0;
	int whosTurn = 0;
	array<int, 2> kingPos;

	
	string genFen();
	void generateMoves();
	vector<array<int, 3>> generateMove(int pos);
	bool kingSafe(array<int, 3> move);
	bool inCheck(int pos);
	vector<array<int, 3>> generatePawnMoves(int pos);
	vector<array<int, 3>> generateSlidingMoves(int pos, int type);
	vector<array<int, 3>> generatePositionMoves(int pos, array<int, 8> area, bool king = false);
	void makeMove(array<int, 3> move);
	void unmakeMove(array<int, 3> moveToUnmake, int unmakeEnPassant, array<bool, 4> unmakeCanCastle, char unmakeTakenPiece);
	

};