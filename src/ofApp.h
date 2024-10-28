#pragma once

#include "ofMain.h"
#include "../include/Board.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void drawBoard();
		void updateVisualBoard();

		Board board;
		array<ofImage, 64> visualBoard;
		ofImage bP, bR, bN, bB, bQ, bK, wP, wR, wN, wB, wQ, wK, emptySquare;


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
		
};
