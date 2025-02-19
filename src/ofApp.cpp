#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    board = Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //board = Chess("r1bqk1nr/pppp1ppp/2n5/2b1p3/2B1P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 0 1");
    board.genMoves();
    board.debugMessage(board.genFen());
    whosTurn = board.whosTurn;

    if (gamemode != 4) {
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

        mainFont.load("fonts/arial.ttf", 32);
        clockThread = thread(&ofApp::clockRun, this);
    }
}

//Only used for gamemode 4, lichess-bot implementation.
void ofApp::update() {
    if (gamemode == 4) {
        std::string input;
        std::string current_fen;

        //Detect input
        while (std::getline(std::cin, input)) {
            if (input.substr(0, 3) == "uci") {
                std::cout << "id name Terconari" << std::endl;
                std::cout << "id author nthorn" << std::endl;
                std::cout << "uciok" << std::endl;

            // Initialize board / run through moves made so far
            } else if (input.substr(0, 8) == "position") {
                board = Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                istringstream iss(input);
                vector<string> parameters;
                string parameter;
                string movesMade = "";

                //Split input
                while (iss >> parameter) {
                    parameters.push_back(parameter);
                }

                //More than just initialization
                if (parameters.size() > 2) {

                    //Disable book moves if more than 8 moves made.
                    if (parameters.size() > 19) {
                        board.outOfBook = true;
                    }

                    if(parameters.size() == 2) {
                        timeSec = -1;
                    }

                    //Iterate through moves
                    for (string uci : parameters) {

                        //Skip if initialization
                        if (uci == "position" || uci == "startpos" || uci == "moves") {
                            continue;
                        }
                        makeMove(board.UCIToMove(uci));
                        
                        movesMade += uci + " ";
                    }
                }
                board.thisGamesMoves = movesMade;

            //Start move generation
            } else if (input.substr(0, 2) == "go") {
                istringstream iss(input);
                vector<string> parameters;
                string parameter;
                string movesMade = "";

                //Split input
                while (iss >> parameter) {
                    parameters.push_back(parameter);
                }

                //Set time control
                if (parameters.size() >= 2) {
                    if (parameters[1] == "wtime") {
                        whiteTime = double(stoi(parameters[2])) / 1000;
                        blackTime = double(stoi(parameters[4])) / 1000;
                        if(timeSec == -1) {
                            timeSec = whiteTime;
                        }
                    }
                }
                if (parameters.size() >= 6) {
                    if (parameters[5] == "winc") {
                        increment = double(stoi(parameters[6])) / 1000;
                    }
                }

                makeBotMove();

                //Account for promotion.
                std::string best_move = board.posToCoords(board.negaMaxResult.from) + board.posToCoords(board.negaMaxResult.to);
                if (board.negaMaxResult.flag >= 6) {
                    switch (board.negaMaxResult.flag) {
                    case Chess::Q_PROMOTION:
                        best_move += "q";
                        break;
                    case Chess::R_PROMOTION:
                        best_move += "r";
                        break;
                    case Chess::B_PROMOTION:
                        best_move += "b";
                        break;
                    case Chess::N_PROMOTION:
                        best_move += "n";
                        break;
                    }
                }
                std::cout << "bestmove " << best_move << std::endl;

            //End of game
            } else if (input == "quit") {
                std::exit(0);

            //Confirm bot's ready
            } else if (input == "isready") {
                std::cout << "readyok" << std::endl;

            //Unaccounted for case
            } else {
                std::cout << "NEED TO ACCOUNT FOR '" << input << "'." <<std::endl;
            }
        }
    }
    
}

//Main logic for gamemodes 0-3.
void ofApp::draw() {

    //Start screen not yet implemented.
    if (startScreen) {

    } else {

        //Start bot move if botvbot or botvplayer.
        if ((gamemode == 0 || gamemode == 2) && botInitMove == false) {
            threadedBoard = thread(&ofApp::makeBotMove, this);
            botInitMove = true;
        }
    
        drawBoard();

        //End of bot move, execute premoves/start next bot move.
        if (!botThinking && threadedBoard.joinable()) {
            threadedBoard.join();
            if (board.evaluation > 0) {
                board.debugMessage("Eval: \033[30m\033[47m" + to_string(board.evaluation) + "\033[0m");
            } else {
                board.debugMessage("Eval: " + to_string(board.evaluation));
            }
            string movesToShow = board.visualGamesMoves;
            if (board.visualGamesMoves.length() > 56) {
                movesToShow = "... " + board.visualGamesMoves.substr(board.visualGamesMoves.length() - 56, 56);
            }
            board.debugMessage("Moves made: " + movesToShow);
            if (preMove.from != -1) {
                tryMove(preMove);
                preMove = { -1, -1, Chess::EMPTY };
            }
            gameStarted = true;
            if (gamemode == 0 && board.gameStatus == -1) {
                threadedBoard = thread(&ofApp::makeBotMove, this);
            }
        }
        drawClock();
    }
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
    mainFont.drawString(bt, 685, 70);
    ofSetColor(0);
    mainFont.drawString(wt, 685, 600);
    ofSetColor(255);
    wK_inverted.draw(695, 100, 150, 150);
    ofSetColor(255);
    wK.draw(695, 390, 150, 150);
    ofSetColor(0, 100);
    ofDrawRectangle(640, 320 * (1-whosTurn), 260, 320);

    ofSetColor(255);
    ofDrawRectangle(640, 0, 10, 640);
    ofSetColor(0);
    ofDrawRectangle(640, 0, 10, 640 * (1/(1+pow(1.2,board.evaluation))));

    ofSetColor(255);
    ofDrawRectangle(650, 0, 10, 640);
    ofSetColor(0);
    ofDrawRectangle(650, 0, 10, 640 * (1/(1+pow(1.2,board.evaluate()))));

    ofSetColor(0);
    ofDrawRectangle(650, 0, 1, 640);
    ofSetColor(255);
    ofDrawRectangle(650, 0, 1, 640 * (1/(1+pow(1.2,board.evaluation))));

    ofSetColor(0);
    ofDrawRectangle(660, 0, 1, 640);
    ofSetColor(255);
    ofDrawRectangle(660, 0, 1, 640 * (1/(1+pow(1.2,board.evaluate()))));
}

//Draw the board.
void ofApp::drawBoard() {
    int mPos = int(mouseY / 80) * 8 + int(mouseX / 80); //Mouse position
    updateVisualChess(); //Update the visual board
    ofColor darkTile(181, 136, 99);
    ofColor lightTile(240, 217, 181);
    ofColor tileColor = lightTile;
    array<ofImage, 4> promotionPieces = { bQ, bR, bB, bN };
    array<int, 4> promotionPiecesHighlights = { 100, 100, 100, 100 };
    vector<Chess::Move> moves = {};

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

    //Draw legal moves for piece
    } else if (pieceHeld) {
        moves = board.getPieceMoves(pieceHeldPos);
        for (auto move : moves) {
            if (board.square[move.to] == ' ' && move.flag != Chess::EN_PASSANT) {
                ofSetColor(255, 0, 0, 100);
                ofDrawCircle(80 * (move.to % 8)+40, 80 * int(move.to / 8)+40, 10);
            } else {
                ofSetColor(255, 0, 0, 75);
                ofRectangle outerRect;
                outerRect.x = 80 * (move.to % 8);
                outerRect.y = 80 * int(move.to / 8);
                outerRect.width = 80;
                outerRect.height = 80;
                ofDrawRectangle(outerRect);
                if(((move.to % 8) + int(move.to / 8)) % 2 == 0) {
                    ofSetColor(lightTile);
                } else {
                    ofSetColor(darkTile);
                }
                ofDrawRectRounded(outerRect, 30);
                ofSetColor(255);
                visualChess[move.to].draw(80 * (move.to % 8), 80 * int(move.to / 8), 80, 80);
            }
            
        }
        ofSetColor(255);
        pieceHeldImage.draw(80 * (pieceHeldPos % 8), 80 * (int(pieceHeldPos / 8)), 80, 80);
        ofSetColor(255, 0, 0, 50);
        ofDrawRectangle(80 * (pieceHeldPos % 8), 80 * (int(pieceHeldPos / 8)), 80, 80);
        ofSetColor(255, 255, 255, 255);
        pieceHeldImage.draw(mouseX - 40, mouseY - 40, 80, 80);

    //Draw preMove visuals
    } else {
        if (preMove.from != -1) {
            ofSetColor(255, 100, 100, 100);
            visualChess[preMove.from].draw(80 * (preMove.to % 8), 80 * int(preMove.to / 8), 80, 80);
            if (((preMove.from % 8) + int(preMove.from / 8)) % 2 == 0) {
                ofSetColor(lightTile);
            } else {
                ofSetColor(darkTile);
            }
            ofDrawRectangle(80 * (preMove.from % 8), 80 * int(preMove.from / 8), 80, 80);
        }
    }
    
}

//Updates the visual board array.
void ofApp::updateVisualChess() {
    for (int pos = 0; pos < 64; pos++) {
        switch (board.square[pos]) {
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
    while (board.gameStatus == -1) {
        if (gameStarted) {
            if (whosTurn == 0) {
                this_thread::sleep_for(std::chrono::milliseconds(10));
                if (blackTime <= 0) {
                    board.gameStatus = 1;
                    break;
                }
                blackTime -= 0.01;
            } else {
                this_thread::sleep_for(std::chrono::milliseconds(10));
                if (whiteTime <= 0) {
                    board.gameStatus = 0;
                    break;
                }
                whiteTime -= 0.01;
            }
        }
    }
}

double ofApp::timeFactor(double x) {
    double term1 = 0.3 + (1 - 0.3) * (1 / (1 + exp(-7 * (x - 15.7))));
    double term2 = 1.5 / (pow(x - 17.7, 1.6) + exp(1));
    return std::max(term1 + term2, 0.4);
}



//Keep track of bot's panic modes.
void ofApp::timerRun(Chess* b) {
    for (int i = 0; i < 40; i++) {
        cout << i << ":" << timeFactor((double)i) << ", ";
    }
    int initTime = int((((timeSec) / 40) + increment)*timeFactor(b->halfMoves));
    b->debugMessage("Time to think: " + to_string(initTime));
    b->debugMessage("Extra time: " + to_string(initTime / 3.0));
    b->debugMessage("Time factor: " + to_string(timeFactor(b->halfMoves)));
    int time = initTime*10;
    if ((whosTurn == 0 ? blackTime : whiteTime) <= increment){
        b->panicLevel = 2;
        time = 0;
    }
    //cout << "\nTimer " << initTime << "\n";
    b->debugMessage("Panic level: " + to_string(b->panicLevel));
    while (time > 0 && !botMoved) {
        this_thread::sleep_for(std::chrono::milliseconds(100));
        time--;
    }
    if (!botMoved) {
        b->panicLevel++;
        b->debugMessage("Panic level: " + to_string(b->panicLevel));
    }
    time = int(initTime)*3;
    if ((whosTurn == 0 ? blackTime : whiteTime) <= increment){
        b->panicLevel = 2;
        time = 0;
    }
    //cout << "\nTimer2 " << initTime/3 << "\n";
    while (time > 0 && !botMoved) {
        this_thread::sleep_for(std::chrono::milliseconds(100));
        time--;
    }
    if (!botMoved) {
        b->panicLevel++;
        b->debugMessage("Panic level: " + to_string(b->panicLevel));
    }
    b->debugMessage("\nExited at panic level " + to_string(b->panicLevel));
}

//Make the bot move.
void ofApp::makeBotMove() {
    botThinking = true;
    Chess botBoard;
    botBoard = board;
    if (gamemode != 0) {
        board.whosTurn = 1 - board.whosTurn; //Inverse turn so player can preMove
    }
    botMoved = false;
    timerThread = thread(&ofApp::timerRun, this, &botBoard);
    botBoard.makeBotMove(-INT_MAX, INT_MAX);
    botMoved = true;
    timerThread.join();
    botBoard.panicLevel = 0;
    pieceHeld = false;
    promoting = false;
    board = botBoard;
    botThinking = false;
    if (whosTurn == 0) {
        blackTime += increment;
    } else {
        whiteTime += increment;
    }
    whosTurn = 1 - whosTurn;
}

void ofApp::makeMove(Chess::Move move) {
    gameStarted = true;
    board.makeMove(move);
    if (whosTurn == 0) {
        blackTime += increment;
    } else {
        whiteTime += increment;
    }
    whosTurn = 1 - whosTurn;
    if (board.evaluation > 0) {
        board.debugMessage("Eval: \033[30m\033[47m" + to_string(board.evaluation) + "\033[0m");
    } else {
        board.debugMessage("Eval: " + to_string(board.evaluation));
    }
    string movesToShow = board.visualGamesMoves;
    if (board.visualGamesMoves.length() > 56) {
        movesToShow = "... " + board.visualGamesMoves.substr(board.visualGamesMoves.length() - 56, 56);
    }
    board.debugMessage("Moves made: " + movesToShow);
    if (gamemode == 1 || gamemode == 2) {
        threadedBoard = thread(&ofApp::makeBotMove, this);
    }

}

//Make a player move.
void ofApp::tryMove(Chess::Move moveToTry) {
    if (board.gameStatus != -1 || gamemode == 0) {
        return;
    }
    
    vector<Chess::Move> moves = board.getPieceMoves(moveToTry.from); //Generate moves for selected piece

    //Check whether move is legal or is preMove
    for (Chess::Move move : moves) {

        //Non-flag specific move
        if (moveToTry.flag == Chess::EMPTY) {
            if (move.to == moveToTry.to) {
                if (!botThinking) {
                    board.debugMessage("Making user move.");
                    board.thisGamesMoves += board.posToCoords(move.from) + board.posToCoords(move.to) + " ";
                    if (whosTurn == 1) {
                        board.visualGamesMoves += "\033[30m\033[47m";
                    }
                    board.visualGamesMoves += board.posToCoords(move.from) + board.posToCoords(move.to) + "\033[0m ";
                    makeMove(move);
                    
                } else {
                    board.debugMessage("Making user preMove.");
                    preMove = move;
                    preMoveImage = visualChess[move.from];
                }

                return;
            }

        //Flag-specific move
        } else {
            if (move.from == moveToTry.from && move.to == moveToTry.to && move.flag == moveToTry.flag) {
                if (!botThinking) {
                    board.debugMessage("Making user move.");
                    board.thisGamesMoves += board.posToCoords(move.from) + board.posToCoords(move.to) + " ";
                    if (whosTurn == 1) {
                        board.visualGamesMoves += "\033[30m\033[47m";
                    }
                    board.visualGamesMoves += board.posToCoords(move.from) + board.posToCoords(move.to) + "\033[0m ";
                    makeMove(move);
                    
                } else {
                    board.debugMessage("Making user preMove.");
                    preMove = move;
                    preMoveImage = visualChess[move.from];
                }
                return;
            }
        }
    }
    board.debugMessage("Move not found, from: " + board.posToCoords(moveToTry.from) + " to: " + board.posToCoords(moveToTry.to) + " type: " + to_string(moveToTry.flag));
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
    preMove = { -1, -1, Chess::EMPTY };
    board.genMoves();
    int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
    if (startScreen) {

    } else if(mouseX < 640) {
        if (!promoting) {
            if (pieceHeld == false && board.square[mPos] != ' ' && (board.square[mPos] < 97 ? 1 : 0) == board.whosTurn && board.getPieceMoves(mPos).size() > 0) {
                pieceHeld = true;
                pieceHeldPos = mPos;
                pieceHeldImage = visualChess[pieceHeldPos];
            } else if (pieceHeld && mPos != pieceHeldPos) {
                if (tolower(board.square[pieceHeldPos]) == 'p' && (mPos <= 7 || mPos >= 56)) {
                    promoting = true;
                    promotePos = mPos;
                    return;
                }
                tryMove({pieceHeldPos, mPos, Chess::EMPTY});
                pieceHeld = false;
            } else {
                pieceHeld = false;
            }
        } else {
            if (mouseY > 240 && mouseY < 320 && mouseX > 160 && mouseX < 480) {
                if (mouseX > 160 && mouseX < 240) {
                    board.debugMessage("PromotingQ");
                    tryMove({ pieceHeldPos, promotePos, Chess::Q_PROMOTION });
                } else if (mouseX > 240 && mouseX < 320) {
                    board.debugMessage("PromotingR");
                    tryMove({ pieceHeldPos, promotePos, Chess::R_PROMOTION });
                } else if (mouseX > 320 && mouseX < 400) {
                    board.debugMessage("PromotingB");
                    tryMove({ pieceHeldPos, promotePos, Chess::B_PROMOTION });
                } else if (mouseX > 400 && mouseX < 480) {
                    board.debugMessage("PromotingN");
                    tryMove({pieceHeldPos, promotePos, Chess::N_PROMOTION});
                }
            }
            promoting = false;
            pieceHeld = false;
        }
    } else {
        //Alarm clicked.
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    int mPos = int(mouseY / 80) * 8 + int(mouseX / 80);
    if (startScreen) {

    } else if(mouseX < 640) {
        if (mPos != pieceHeldPos && pieceHeld && !promoting) {
            if (tolower(board.square[pieceHeldPos]) == 'p' && (mPos <= 7 || mPos >= 56)) {
                promoting = true;
                promotePos = mPos;
                return;
            }
            tryMove({ pieceHeldPos, mPos, Chess::EMPTY });
            pieceHeld = false;
        }
    } else {

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
