#pragma once
#include <array>
using namespace std;

class moveToUnmake {
public:
	moveToUnmake();
	moveToUnmake(array<int, 3> move, int enPassant, array<bool, 4> canCastle, char takenPiece);
	array<int, 3> move;
	int enPassant;
	array<bool, 4> canCastle;
	char takenPiece;
};