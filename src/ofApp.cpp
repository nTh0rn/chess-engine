#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//board = Board("rnbqkbnr/pppp1ppp/8/8/8/7p/PPPPPPPP/RNBQKBNR w KQkq - 1 4");
	//board = Board("rnb1kbnr/pppp1ppp/4p3/8/6q1/2N2PP1/PPPPP2P/R1BQKBNR b KQkq - 1 4");
	board = Board("8/PPPPPPPP/8/4k3/1K6/8/pppppppp/8 w - - 0 1");
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
	//board.generateMoves();
	
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
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	updateVisualBoard();
	ofColor darkTile(181, 136, 99);
	ofColor lightTile(240, 217, 181);
	ofColor tileColor = lightTile;
	array<ofImage, 4> promotionPieces = { bQ, bR, bB, bN };
	vector<array<int, 3>> moves = {};
	for (int pos = 0; pos < 64; pos++) {
		if ((pos + 1) % 8 != 1) {
			tileColor = (tileColor==darkTile) ? lightTile : darkTile;
		}
		ofSetColor(tileColor);
		ofDrawRectangle(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
		ofSetColor(255);
		visualBoard[pos].draw(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
	}
	if (promoting) {
		ofSetColor(0, 0, 0, 100);
		ofDrawRectangle(0, 0, 640, 640);
		if (tolower(board.square[pieceHeldPos]) != 'p') {
			promotionPieces = { wQ, wR, wB, wN };
		}

		promotionPieces[0].draw(160, 140, 80, 80);
		promotionPieces[1].draw(240, 140, 80, 80);
		promotionPieces[2].draw(320, 140, 80, 80);
		promotionPieces[3].draw(400, 140, 80, 80);
		
	} else if (pieceHeld) {
		
		moves = board.generateMove(pieceHeldPos);
		for (array<int, 3> move : moves) {
			if (board.square[move[1]] == ' ' && move[2] != 3) {
				ofSetColor(255, 0, 0, 100);
				ofDrawCircle(80 * (move[1] % 8)+40, 80 * int(move[1] / 8)+40, 10);
			} else {
				ofSetColor(255, 0, 0, 75);
				ofRectangle outerRect;
				outerRect.x = 80 * (move[1] % 8);
				outerRect.y = 80 * int(move[1] / 8);
				outerRect.width = 80;
				outerRect.height = 80;
				ofDrawRectangle(outerRect);
				if(((move[1] % 8) + int(move[1] / 8)) % 2 == 0) {
					ofSetColor(lightTile);
				} else {
					ofSetColor(darkTile);
				}
				ofDrawRectRounded(outerRect, 30);
				ofSetColor(255);
				visualBoard[move[1]].draw(80 * (move[1] % 8), 80 * int(move[1] / 8), 80, 80);
			}
			
			
		}
		
		ofSetColor(255);
		pieceHeldImage.draw(80 * (pieceHeldPos % 8), 80 * (int(pieceHeldPos / 8)), 80, 80);
		ofSetColor(255, 0, 0, 50);
		ofDrawRectangle(80 * (pieceHeldPos % 8), 80 * (int(pieceHeldPos / 8)), 80, 80);
		ofSetColor(255, 255, 255, 255);
		pieceHeldImage.draw(mouseX - 40, mouseY - 40, 80, 80);
		
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
		case ' ':
			visualBoard[pos] = emptySquare;
			break;
		}
	}
	if (pieceHeld) {
		visualBoard[pieceHeldPos] = emptySquare;
	}
}

void ofApp::makeMove(int from, int to, int flag) {
	vector<array<int, 3>> moves;

	moves = board.generateMove(from);

	for (array<int, 3> move : moves) {
		if (move[1] == to) {
			board.makeMove(move);
			return;
		}
	}
	cout << "Move not found from: " << from << " to: " << to << " flag: " << flag << "\n";
	for (array<int, 3> move : moves) {
		if (move[0] == from) {
			cout << "from: " << move[0] << " to: " << move[1] << " flag: " << move[2] << "\n";
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
	board.generateMoves();
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	if (!promoting) {
		if (pieceHeld == false && board.square[mPos] != ' ' && (board.square[mPos] < 97 ? 1 : 0) == board.whosTurn && board.generateMove(mPos).size() > 0) {
			pieceHeld = true;
			pieceHeldPos = mPos;
			pieceHeldImage = visualBoard[pieceHeldPos];
		} else if (pieceHeld && mPos != pieceHeldPos) {
			if (tolower(board.square[pieceHeldPos]) == 'p' && (mPos <= 7 || mPos >= 56)) {
				promoting = true;
				promotePos = mPos;
				return;
			}
			if (board.square[mPos] != ' ') {
				makeMove(pieceHeldPos, mPos, 1);
			} else {
				makeMove(pieceHeldPos, mPos, 0);
			}
			pieceHeld = false;
		} else {
			pieceHeld = false;
		}
	} else {
		cout << mouseX << ", " << mouseY << "\n";
		if (mouseX > 160 && mouseX < 240) {
			cout << "PromotingQ\n";
			makeMove(pieceHeldPos, promotePos, 6);
		} else if (mouseX > 240 && mouseX < 320) {
			cout << "PromotingR\n";
			makeMove(pieceHeldPos, promotePos, 7);
		} else if (mouseX > 320 && mouseX < 400) {
			cout << "PromotingB\n";
			makeMove(pieceHeldPos, promotePos, 8);
		} else if (mouseX > 400 && mouseX < 480) {
			cout << "PromotingN\n";
			makeMove(pieceHeldPos, promotePos, 9);
		}
		promoting = false;
		pieceHeld = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	if (mPos != pieceHeldPos && pieceHeld && !promoting) {
		if (tolower(board.square[pieceHeldPos]) == 'p' && (mPos <= 7 || mPos >= 56)) {
			promoting = true;
			promotePos = mPos;
			return;
		}
		makeMove(pieceHeldPos, mPos, 0);
		pieceHeld = false;
	}
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
