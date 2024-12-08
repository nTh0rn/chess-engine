#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	board = Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//board = Chess("7k/ppn2pp1/8/8/8/7q/1K6/5r2 w - - 0 1");
	//board = Chess("rn4k1/ppp2ppp/8/2r5/8/BP3N1P/P4PP1/4R1K1 w q - 0 1");
	board.genMoves();
	cout << board.genFen() << "\n";
	botTurn = 0; // 0=black, 1=white

	//Initialize images
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
	wK_inverted.load("images/wK_inverted.png");

	myfont.load("fonts/arial.ttf", 32);

	botTime = (botTurn == 0 ? &blackTime : &whiteTime);
	playerTime = (botTurn == 1 ? &blackTime : &whiteTime);

	if (botTurn == board.whosTurn) {
		gameStarted = true;
		clockThread = thread(&ofApp::clockRun, this);
		threadedBoard = thread(&ofApp::makeBotMove, this);
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	drawBoard();
	if (!botThinking && threadedBoard.joinable()) {
		threadedBoard.join();
		cout << "Bot moved. Eval: " << board.evaluate() << "\n\n";
		if (preMove[0] != -1) {
			makeMove(preMove[0], preMove[1], preMove[2]);
			cout << "Premove made.\n";
			preMove = { -1, -1, -1 };
		}
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
	wK_inverted.draw(695, 100, 150, 150);
	ofSetColor(255);
	wK.draw(695, 390, 150, 150);
	ofSetColor(0, 100);
	if (botThinking) {
		ofDrawRectangle(640, 320*(1-botTurn), 260, 320);
	} else {
		ofDrawRectangle(640, 320*botTurn, 260, 320);
	}
}

//Draw the board.
void ofApp::drawBoard() {
	tempBoard = board;
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80); //Mouse position
	updateVisualChess(); //Update the visual board
	ofColor darkTile(181, 136, 99);
	ofColor lightTile(240, 217, 181);
	ofColor tileColor = lightTile;
	array<ofImage, 4> promotionPieces = { bQ, bR, bB, bN };
	array<int, 4> promotionPiecesHighlights = { 100, 100, 100, 100 };
	vector<array<int, 3>> moves = {};

	//Draw the base tiles and pieces
	for (int pos = 0; pos < 64; pos++) {
		if ((pos + 1) % 8 != 1) {
			tileColor = (tileColor==darkTile) ? lightTile : darkTile;
		}
		ofSetColor(tileColor);
		ofDrawRectangle(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
		ofSetColor(255);
		visualChess[pos].draw(80 * (pos % 8), 80 * int(pos / 8), 80, 80);
	}

	//Draw promoting visuals
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

	//Draw legal moves for piece
	} else if (pieceHeld) {
		moves = tempBoard.getPieceMoves(pieceHeldPos);
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

	//Draw premove visuals
	} else {
		if (preMove[0] != -1) {
			ofSetColor(255, 100, 100, 100);
			visualChess[preMove[0]].draw(80 * (preMove[1] % 8), 80 * int(preMove[1] / 8), 80, 80);
			if (((preMove[0] % 8) + int(preMove[0] / 8)) % 2 == 0) {
				ofSetColor(lightTile);
			} else {
				ofSetColor(darkTile);
			}
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

//Keep track of the clock
void ofApp::clockRun() {
	whiteTime = timeSec;
	blackTime = timeSec;
	while (!gameover) {
		if (botThinking) {
			this_thread::sleep_for(std::chrono::milliseconds(10));
			if (*botTime <= 0) {
				gameover = true;
				break;
			}
			*botTime -= 0.01;
		} else {
			this_thread::sleep_for(std::chrono::milliseconds(10));
			if (*playerTime <= 0) {
				gameover = true;
				break;
			}
			*playerTime -= 0.01;
			
		}
	}
}

//Keep track of bot's panic modes.
void ofApp::timerRun(Chess* b) {
	int time = (*botTime/50 * 1000);
	cout << "Panic level: 0\n";
	while (time > 0 && !botMoved) {
		this_thread::sleep_for(std::chrono::milliseconds(1));
		time--;
	}
	if (!botMoved) {
		cout << "Panic level: 1\n";
		b->timerHurry = true;
	}
	
	time = (*botTime / 50 * 350);
	while (time > 0 && !botMoved) {
		this_thread::sleep_for(std::chrono::milliseconds(1));
		time--;
	}
	if (!botMoved) {
		cout << "Panic level: 2\n";
		b->timerDone = true;
	} else {
		cout << "Exited panic.\n";
	}
}

//Make the bot move.
void ofApp::makeBotMove() {
	cout << "\n\nAll moves made: " << board.thisGamesMoves << "\n\n";
	botThinking = true;
	Chess botBoard;
	botBoard = board;
	board.whosTurn = 1 - board.whosTurn; //Inverse turn so player can premove
	botMoved = false;
	timerThread = thread(&ofApp::timerRun, this, &botBoard);
	botBoard.makeBotMove(-DBL_MAX, DBL_MAX);
	botMoved = true;
	timerThread.join();
	botBoard.timerHurry = false;
	botBoard.timerDone = false;
	pieceHeld = false;
	promoting = false;
	board = botBoard;
	botThinking = false;
}

//Make a player move.
void ofApp::makeMove(int from, int to, int flag) {
	if (gameover) {
		return;
	}
	if (!gameStarted && board.whosTurn != botTurn) {
		gameStarted = true;
		clockThread = thread(&ofApp::clockRun, this);
	}
	vector<array<int, 3>> moves;

	moves = board.getPieceMoves(from);

	//Check whether move is legal or is premove
	for (array<int, 3> move : moves) {

		//Non-flag specific move
		if (flag == -1) {
			if (move[1] == to) {
				if (!botThinking) {
					cout << "Making move.\n";
					board.thisGamesMoves += board.posToCoords(move[0]) + board.posToCoords(move[1]) + " ";
					board.makeMove(move);
					threadedBoard = thread(&ofApp::makeBotMove, this);
				} else {
					cout << "Making premove.\n";
					preMove = move;
					preMoveImage = visualChess[move[0]];
				}

				return;
			}

		//Flag-specific move
		} else {
			if (move[0] == from && move[1] == to && move[2] == flag) {
				if (!botThinking) {
					cout << "Making move.\n";
					board.thisGamesMoves += board.posToCoords(move[0]) + board.posToCoords(move[1]) + " ";
					board.makeMove(move);
					threadedBoard = thread(&ofApp::makeBotMove, this);
				} else {
					cout << "Making premove.\n";
					preMove = move;
					preMoveImage = visualChess[move[0]];
				}

				return;
			}
		}
	}
	cout << "Move not found, from: " << from << " to: " << to << " flag: " << flag << "\n";
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
	tempBoard.genMoves();
	int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
	if (!promoting) {
		if (pieceHeld == false && tempBoard.square[mPos] != ' ' && (tempBoard.square[mPos] < 97 ? 1 : 0) == tempBoard.whosTurn && tempBoard.getPieceMoves(mPos).size() > 0) {
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
