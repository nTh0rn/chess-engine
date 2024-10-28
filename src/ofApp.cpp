#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	board = Board("rnbqkbnr/pppp1ppp/8/8/8/7p/PPPPPPPP/RNBQKBNR b KQkq - 1 4");
	cout << board.genFen();
	
	bP.load("images/bP.png");
	bR.load("images/bR.png");
	bN.load("images/bN.png");
	bB.load("images/bB.png");
	bQ.load("images/bQ.png");
	bK.load("images/bK.png");
	wP.load("images/wP.png");
	wR.load("images/wR.png");
	wN.load("images/wN.png");
	wB.load("images/wB.png");
	wQ.load("images/wQ.png");
	wK.load("images/wK.png");
	emptySquare.load("images/Empty.png");
	board.generateMoves();
	
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	drawBoard();
}

//Draw the board.
void ofApp::drawBoard() {
	updateVisualBoard();
	ofColor darkTile(181, 136, 99);
	ofColor lightTile(240, 217, 181);
	ofColor tileColor = lightTile;
	for (int pos = 0; pos < 64; pos++) {
		if ((pos + 1) % 8 != 1) {
			tileColor = (tileColor==darkTile) ? lightTile : darkTile;
		}
		ofSetColor(tileColor);
		ofDrawRectangle(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
		ofSetColor(255);
		visualBoard[pos].draw(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
	}
	for (array<int, 3> move : board.moves) {
		ofSetColor(255,0,0, 100);
		ofDrawRectangle(80 * (move[1] % 8), 80 * int(move[1] / 8), 80, 80);
	}
}

//Updates the visual board array.
void ofApp::updateVisualBoard() {
	for (int pos = 0; pos < 64; pos++) {
		switch (board.square[pos]) {
		case 'p':
			visualBoard[pos] = bP;
			break;
		case 'r':
			visualBoard[pos] = bR;
			break;
		case 'n':
			visualBoard[pos] = bN;
			break;
		case 'b':
			visualBoard[pos] = bB;
			break;
		case 'q':
			visualBoard[pos] = bQ;
			break;
		case 'k':
			visualBoard[pos] = bK;
			break;
		case 'P':
			visualBoard[pos] = wP;
			break;
		case 'R':
			visualBoard[pos] = wR;
			break;
		case 'N':
			visualBoard[pos] = wN;
			break;
		case 'B':
			visualBoard[pos] = wB;
			break;
		case 'Q':
			visualBoard[pos] = wQ;
			break;
		case 'K':
			visualBoard[pos] = wK;
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
