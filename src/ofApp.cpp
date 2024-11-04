#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//board = Board("rnb1kbnr/pppp1ppp/4p3/8/6q1/2N2PP1/PPPPP2P/R1BQKBNR b KQkq - 1 4");
	//board = Board("8/PPPPPPPP/8/4k3/1K6/8/pppppppp/8 w - - 0 1");

	//board = Board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
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
	board.generateMoves();
	cout << "\nMoves size" << board.moves.size() << "\n";
	//for (int i = 1; i < 9; i++) {
	//	cout << "\nDepth of " << i << " " << board.depthSearch(i);
	//}
	//cout << "\nDepth of " << 5 << " \n" << board.depthSearch(5, 5);
	
	
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
	array<int, 4> promotionPiecesHighlights = { 100, 100, 100, 100 };
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
		if (board.square[pieceHeldPos] == 'P') {
			promotionPieces = { wQ, wR, wB, wN };
		}
		if (mouseY > 240 && mouseY < 320 && mouseX > 160 && mouseX < 480) {
			ofSetColor(255);
			if (mouseX > 160 && mouseX < 240) {
				promotionPiecesHighlights[0] = 255;
				promotionPieces[0].draw(80 * (promotePos % 8), 80 * int(promotePos / 8), 80, 80);
			} else if (mouseX > 240 && mouseX < 320) {
				promotionPiecesHighlights[1] = 255;
				promotionPieces[1].draw(80 * (promotePos % 8), 80 * int(promotePos / 8), 80, 80);
			} else if (mouseX > 320 && mouseX < 400) {
				promotionPiecesHighlights[2] = 255;
				promotionPieces[2].draw(80 * (promotePos % 8), 80 * int(promotePos / 8), 80, 80);
			} else if (mouseX > 400 && mouseX < 480) {
				promotionPiecesHighlights[3] = 255;
				promotionPieces[3].draw(80 * (promotePos % 8), 80 * int(promotePos / 8), 80, 80);
			}
		} else {
			promotionPiecesHighlights = { 255, 255, 255, 255 };
			pieceHeldImage.draw(80 * (pieceHeldPos % 8), 80 * int(pieceHeldPos / 8), 80, 80);
		}
		ofSetColor(0, 200);
		ofDrawRectangle(0, 0, 640, 640);
		
		ofSetColor(255, promotionPiecesHighlights[0]);
		promotionPieces[0].draw(160, 240, 80, 80);
		ofSetColor(255, promotionPiecesHighlights[1]);
		promotionPieces[1].draw(240, 240, 80, 80);
		ofSetColor(255, promotionPiecesHighlights[2]);
		promotionPieces[2].draw(320, 240, 80, 80);
		ofSetColor(255, promotionPiecesHighlights[3]);
		promotionPieces[3].draw(400, 240, 80, 80);
		
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
	cout << "\n Eval: " << board.evaluate() << "\n";
	vector<array<int, 3>> moves;

	moves = board.generateMove(from);

	for (array<int, 3> move : moves) {
		if (flag == -1) {
			if (move[1] == to) {
				board.makeMove(move);
				board.makeBotMove();
				break;
			}
		} else {
			if (move[0] == from && move[1] == to && move[2] == flag) {
				board.makeMove(move);
				board.makeBotMove();
				break;
			}
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
			makeMove(pieceHeldPos, mPos, -1);
			pieceHeld = false;
		} else {
			pieceHeld = false;
		}
	} else {
		cout << mouseX << ", " << mouseY << "\n";
		if (mouseY > 240 && mouseY < 320 && mouseX > 160 && mouseX < 480) {
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
		makeMove(pieceHeldPos, mPos, -1);
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
