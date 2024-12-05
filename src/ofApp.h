#pragma once

#include "ofMain.h"
#include "../include/Chess.h"
#include <chrono>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void drawBoard();
		void updateVisualChess();
		void makeMove(int from, int to, int flag);
		//void startBoard(string fen, array<int, 2> time);
		int playerTurn;
		Chess board;
		Chess tempBoard;
		array<ofImage, 64> visualChess;
		ofImage bP, bR, bN, bB, bQ, bK, wP, wR, wN, wB, wQ, wK, emptySquare;
		bool pieceHeld;
		int pieceHeldPos;
		ofImage pieceHeldImage;
		ofImage preMoveImage;
		bool promoting = false;
		int promotePos;
		mutex mtx;
		bool botThinking;
		array<int, 3> preMove = { -1, -1, -1 };
		double whiteTime;
		double blackTime;
		ofTrueTypeFont myfont;
		bool gameover = false;
		bool botMoved = false;
		bool gameStarted = false;
		int timeSec = 3 * 60;
		int timeWait = timeSec / 60;
		int timer = 0;
		int increment = 0;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		thread threadedBoard;
		thread timerThread;
		thread clockThread;
		void timerRun(Chess* b);
		void clockRun();
		void drawClock();
		void makeBotMove();
};
