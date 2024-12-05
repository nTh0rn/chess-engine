#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	board = Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//board = Chess("8/1p4k1/8/8/7K/8/6q1/8 w - - 0 1");

	array<int, 2> test = board.coordsToPos("e2e4");
	cout << "\n" << test[0] << ", " << test[1] << "\n";
	//board.setTime(2);
	//board = Chess("1kr5/ppN2ppr/8/3p2n1/3bb3/8/n3PPPQ/4R1K1 w - - 0 1");
	//board = Chess("2kr1b1r/pp1b1ppp/2n5/1B1Q1n2/4RB2/2q2N2/P1P2PPP/3RK3 w k - 0 1");
	//board = Chess("rnb1kbnr/pppp1ppp/4p3/8/6q1/2N2PP1/PPPPP2P/R1BQKBNR b KQkq - 1 4");
	//board = Chess("8/PPPPPPPP/8/4k3/1K6/8/pppppppp/8 w - - 0 1");
	//board = Chess("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
	cout << board.genFen();
	playerTurn = board.whosTurn;
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
	myfont.load("arial.ttf", 32);
	
	//for (int i = 1; i < 9; i++) {
	//	cout << "\nDepth of " << i << " " << board.depthSearch(i);
	//}
	//cout << "\nDepth of " << 5 << " \n" << board.depthSearch(5, 5);
	//threadedBoard = thread(&ofApp::makeBotMove, this);
}


//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	drawBoard();
	if (!botThinking && threadedBoard.joinable()) {
		threadedBoard.join();
		if (preMove[0] != -1) {
			makeMove(preMove[0], preMove[1], preMove[2]);
			preMove = { -1, -1, -1 };
		}
		cout << "\n Bot just moves, Eval: " << board.evaluate() << "\n";
		//threadedBoard = thread(&ofApp::makeBotMove, this);
	}
	if (clockThread.joinable()) {
		//clockThread.join();
	}
	drawClock();

}

//Draws the clock on the right;
void ofApp::drawClock() {
	ofSetColor(20);
	ofDrawRectangle(640, 0, 260, 320);
	ofSetColor(235);
	ofDrawRectangle(640, 320, 260, 320);
	string bt = to_string(int(blackTime / 60)) + ":" + to_string((int(blackTime) % 60) + blackTime - int(blackTime));
	string wt = to_string(int(whiteTime / 60)) + ":" + to_string((int(whiteTime) % 60) + whiteTime - int(whiteTime));
	ofSetColor(255);
	myfont.drawString(bt, 650, 40);
	ofSetColor(0);
	myfont.drawString(wt, 650, 630);
	ofSetColor(255);
	bK.draw(695, 100, 150, 150);
	ofSetColor(255);
	wK.draw(695, 390, 150, 150);
	
	
	ofSetColor(0, 100);
	if (botThinking) {
		ofDrawRectangle(640, 320, 260, 320);
	} else {
		ofDrawRectangle(640, 0, 260, 320);
	}
	//ofDisableAlphaBlending();
}

//Draw the board.
void ofApp::drawBoard() {
	tempBoard = board;
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	updateVisualChess();
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
		visualChess[pos].draw(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
	}
	if (promoting) {
		if (tempBoard.square[pieceHeldPos] == 'P') {
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
		
		moves = tempBoard.generateMove(pieceHeldPos);
		for (array<int, 3> move : moves) {
			if (tempBoard.square[move[1]] == ' ' && move[2] != 3) {
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
				visualChess[move[1]].draw(80 * (move[1] % 8), 80 * int(move[1] / 8), 80, 80);
			}
			
		}
		ofSetColor(255);
		pieceHeldImage.draw(80 * (pieceHeldPos % 8), 80 * (int(pieceHeldPos / 8)), 80, 80);
		ofSetColor(255, 0, 0, 50);
		ofDrawRectangle(80 * (pieceHeldPos % 8), 80 * (int(pieceHeldPos / 8)), 80, 80);
		ofSetColor(255, 255, 255, 255);
		pieceHeldImage.draw(mouseX - 40, mouseY - 40, 80, 80);
	} else {
		if (preMove[0] != -1) {
			cout << "Premove";
			ofSetColor(255, 100, 100, 100);
			visualChess[preMove[0]].draw(80 * (preMove[1] % 8), 80 * int(preMove[1] / 8), 80, 80);
			if ((preMove[0] + 1) % 8 != 1) {
				tileColor = (tileColor == darkTile) ? lightTile : darkTile;
			}
			ofSetColor(tileColor);
			ofDrawRectangle(80 * (preMove[0] % 8), 80 * int(preMove[0] / 8), 80, 80);
		}
	}
}

//Updates the visual board array.
void ofApp::updateVisualChess() {
	Chess tempBoard = board;
	for (int pos = 0; pos < 64; pos++) {
		switch (tempBoard.square[pos]) {
		case 'p':
			visualChess[pos] = bP;
			break;
		case 'r':
			visualChess[pos] = bR;
			break;
		case 'n':
			visualChess[pos] = bN;
			break;
		case 'b':
			visualChess[pos] = bB;
			break;
		case 'q':
			visualChess[pos] = bQ;
			break;
		case 'k':
			visualChess[pos] = bK;
			break;
		case 'P':
			visualChess[pos] = wP;
			break;
		case 'R':
			visualChess[pos] = wR;
			break;
		case 'N':
			visualChess[pos] = wN;
			break;
		case 'B':
			visualChess[pos] = wB;
			break;
		case 'Q':
			visualChess[pos] = wQ;
			break;
		case 'K':
			visualChess[pos] = wK;
			break;
		case ' ':
			visualChess[pos] = emptySquare;
			break;
		}
	}
	if (pieceHeld) {
		visualChess[pieceHeldPos] = emptySquare;
	}
}

void ofApp::clockRun() {
	whiteTime = timeSec+500;
	blackTime = timeSec;
	while (!gameover) {
		//cout << "\n\n\n\nBlack Time: " << blackTime << "\nWhite Time: " << whiteTime << "\n";
		if (botThinking) {
			this_thread::sleep_for(std::chrono::milliseconds(10));
			if (blackTime <= 0) {
				gameover = true;
				break;
			}
			blackTime -= 0.01;
			
			if (blackTime < timeWait) {
				board.timerHurry = true;
				board.timerDone = true;
				board.depth = 2;
				board.panicDepth = 0;
			}
		} else {
			this_thread::sleep_for(std::chrono::milliseconds(10));
			if (whiteTime <= 0) {
				gameover = true;
				break;
			}
			whiteTime -= 0.01;
			
		}
	}
}

void ofApp::timerRun(Chess* b) {
	int time = (blackTime/50 * 1000);
	while (time > 0 && !botMoved) {
		this_thread::sleep_for(std::chrono::milliseconds(1));
		time--;
	}
	if (!botMoved) {
		cout << "\bHURRY HURRY HURRY\n";
		b->timerHurry = true;
	} else {
		cout << "Naturally exited.";
	}
	
	time = (blackTime / 50 * 350);
	while (time > 0 && !botMoved) {
		this_thread::sleep_for(std::chrono::milliseconds(1));
		time--;
	}
	if (!botMoved) {
		cout << "\bMOVE NOW DUMBASS!!!!\n";
		b->timerDone = true;
	}
	
}

void ofApp::makeBotMove() {
	cout << "\n" << board.thisGamesMoves << "\n";
	botThinking = true;
	Chess botBoard;
	bool end = false;
	botBoard = board;
	board.whosTurn = 1 - board.whosTurn;
	botMoved = false;
	timerThread = thread(&ofApp::timerRun, this, &botBoard);
	botBoard.makeBotMove(-10000000000, 10000000000);
	botMoved = true;
	timerThread.join();
	botBoard.timerHurry = false;
	botBoard.timerDone = false;
	pieceHeld = false;
	promoting = false;
	board = botBoard;
	botThinking = false;
}

void ofApp::makeMove(int from, int to, int flag) {
	if (gameover) {
		return;
	}
	if (!gameStarted) {
		gameStarted = true;
		clockThread = thread(&ofApp::clockRun, this);
	}
	cout << "\n Eval: " << board.evaluate() << "\n";
	vector<array<int, 3>> moves;

	moves = board.generateMove(from);

	for (array<int, 3> move : moves) {
		if (flag == -1) {
			if (move[1] == to) {
				if (!botThinking) {
					cout << "Makin moves";
					board.thisGamesMoves += board.posToCoords(move[0]) + board.posToCoords(move[1]) + " ";
					board.makeMove(move);
					threadedBoard = thread(&ofApp::makeBotMove, this);
				} else {
					cout << "Making premove";
					preMove = move;
					preMoveImage = visualChess[move[0]];
				}

				return;
			}
		} else {
			if (move[0] == from && move[1] == to && move[2] == flag) {
				if (!botThinking) {
					cout << "Makin moves";
					board.thisGamesMoves += board.posToCoords(move[0]) + board.posToCoords(move[1]) + " ";
					board.makeMove(move);
					threadedBoard = thread(&ofApp::makeBotMove, this);
				} else {
					cout << "Making premove";
					preMove = move;
					preMoveImage = visualChess[move[0]];
				}

				return;
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
	preMove = { -1, -1, -1 };
	Chess tempBoard = board;
	tempBoard.generateMoves();
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	if (!promoting) {
		if (pieceHeld == false && tempBoard.square[mPos] != ' ' && (tempBoard.square[mPos] < 97 ? 1 : 0) == tempBoard.whosTurn && tempBoard.generateMove(mPos).size() > 0) {
			pieceHeld = true;
			pieceHeldPos = mPos;
			pieceHeldImage = visualChess[pieceHeldPos];
		} else if (pieceHeld && mPos != pieceHeldPos) {
			if (tolower(tempBoard.square[pieceHeldPos]) == 'p' && (mPos <= 7 || mPos >= 56)) {
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
	Chess tempBoard = board;
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	if (mPos != pieceHeldPos && pieceHeld && !promoting) {
		if (tolower(tempBoard.square[pieceHeldPos]) == 'p' && (mPos <= 7 || mPos >= 56)) {
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
