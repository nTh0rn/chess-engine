#include "../include/moveToUnmake.h"
using namespace std;

moveToUnmake::moveToUnmake() {};

moveToUnmake::moveToUnmake(array<int, 3> move, int enPassant, array<bool, 4> canCastle, char takenPiece) {
	this->move = move;
	this->enPassant = enPassant;
	this->canCastle = canCastle;
	this->takenPiece = takenPiece;
}