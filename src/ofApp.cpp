#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	board = Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	board.genMoves();
	board.debugMessage(board.genFen());
	botTurn = 1; // 0=black, 1=white

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
	playerTime = (botTime == &whiteTime ? &blackTime : &whiteTime);


	if (botTurn == board.whosTurn) {
		clockThread = thread(&ofApp::clockRun, this);
		threadedBoard = thread(&ofApp::makeBotMove, this);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	drawBoard();
	if (!botThinking && threadedBoard.joinable()) {
		threadedBoard.join();
		board.debugMessage("Bot moved. Eval: " + to_string(board.evaluate()));
		if (preMove[0] != -1) {
			makeMove(preMove[0], preMove[1], preMove[2]);
			board.debugMessage("Premove executed.");
			preMove = { -1, -1, -1 };
		}
		gameStarted = true;
		threadedBoard = thread(&ofApp::makeBotMove, this);
	}
	drawClock();
}

//Draws the clock on the right;
void ofApp::drawClock() {
	ofSetColor(20);
	ofDrawRectangle(640, 0, 260, 320);
	ofSetColor(235);
	ofDrawRectangle(640, 320, 260, 320);
	string btMinutes = to_string(int(blackTime / 60));
	if (int(blackTime / 60) < 10) {
		btMinutes = "0" + btMinutes;
	}
	string btSeconds = to_string((int(blackTime) % 60) + blackTime - int(blackTime));
	if (int(blackTime) % 60 < 10) {
		btSeconds = "0" + btSeconds;
	}

	string wtMinutes = to_string(int(whiteTime / 60));
	if (int(whiteTime / 60) < 10) {
		wtMinutes = "0" + wtMinutes;
	}
	string wtSeconds = to_string((int(whiteTime) % 60) + whiteTime - int(whiteTime));
	if (int(whiteTime) % 60 < 10) {
		wtSeconds = "0" + wtSeconds;
	}

	string bt = btMinutes + ":" + btSeconds.substr(0,5);
	string wt = wtMinutes + ":" + wtSeconds.substr(0,5);
	ofSetColor(255);
	myfont.drawString(bt, 685, 70);
	ofSetColor(0);
	myfont.drawString(wt, 685, 600);
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
		if (gameStarted) {
			if (board.whosTurn != botTurn) {
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
}

//Keep track of bot's panic modes.
void ofApp::timerRun(Chess* b) {
	int time = (*botTime/40 * 1000);
	b->debugMessage("Panic level: " + to_string(b->panicLevel));
	while (time > 0 && !botMoved) {
		this_thread::sleep_for(std::chrono::milliseconds(1));
		time--;
	}
	if (!botMoved) {
		b->panicLevel++;
		b->debugMessage("Panic level: " + to_string(b->panicLevel));
	}
	
	time = (*botTime / 50 * 350);
	while (time > 0 && !botMoved) {
		this_thread::sleep_for(std::chrono::milliseconds(1));
		time--;
	}
	if (!botMoved) {
		b->panicLevel++;
		b->debugMessage("Panic level: " + to_string(b->panicLevel));
	} else {
		b->debugMessage("Exited at panic level " + to_string(b->panicLevel));
	}
}

//Make the bot move.
void ofApp::makeBotMove() {
	board.debugMessage("All moves made: " + board.thisGamesMoves);
	botThinking = true;
	Chess botBoard;
	botBoard = board;
	board.whosTurn = 1 - board.whosTurn; //Inverse turn so player can premove
	botMoved = false;
	timerThread = thread(&ofApp::timerRun, this, &botBoard);
	botBoard.makeBotMove(-DBL_MAX, DBL_MAX);
	botMoved = true;
	timerThread.join();
	botBoard.panicLevel = 0;
	pieceHeld = false;
	promoting = false;
	board = botBoard;
	botThinking = false;
	whosTurn = 1 - whosTurn;
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
					board.debugMessage("Making user move.");
					board.thisGamesMoves += board.posToCoords(move[0]) + board.posToCoords(move[1]) + " ";
					board.makeMove(move);
					whosTurn = 1 - whosTurn;
					threadedBoard = thread(&ofApp::makeBotMove, this);
				} else {
					board.debugMessage("Making user premove.");
					preMove = move;
					preMoveImage = visualChess[move[0]];
				}

				return;
			}

		//Flag-specific move
		} else {
			if (move[0] == from && move[1] == to && move[2] == flag) {
				if (!botThinking) {
					board.debugMessage("Making user move.");
					board.thisGamesMoves += board.posToCoords(move[0]) + board.posToCoords(move[1]) + " ";
					board.makeMove(move);
					whosTurn = 1 - whosTurn;
					threadedBoard = thread(&ofApp::makeBotMove, this);
				} else {
					board.debugMessage("Making user premove.");
					preMove = move;
					preMoveImage = visualChess[move[0]];
				}

				return;
			}
		}
	}
	board.debugMessage("Move not found, from: " + board.posToCoords(from) + " to: " + board.posToCoords(to) + " type: " + to_string(flag));
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
		if (mouseY > 240 && mouseY < 320 && mouseX > 160 && mouseX < 480) {
			if (mouseX > 160 && mouseX < 240) {
				board.debugMessage("PromotingQ");
				makeMove(pieceHeldPos, promotePos, 6);
			} else if (mouseX > 240 && mouseX < 320) {
				board.debugMessage("PromotingR");
				makeMove(pieceHeldPos, promotePos, 7);
			} else if (mouseX > 320 && mouseX < 400) {
				board.debugMessage("PromotingB");
				makeMove(pieceHeldPos, promotePos, 8);
			} else if (mouseX > 400 && mouseX < 480) {
				board.debugMessage("PromotingN");
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
