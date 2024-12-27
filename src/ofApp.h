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
        void makeMove(Chess::Move move);
        void tryMove(Chess::Move move);
        void timerRun(Chess* b);
        void clockRun();
        void drawClock();
        void makeBotMove();
        double timeMultiplier(double timeTotal, double timeLeft);
        bool startScreen = false;
        bool botInitMove = false;
        int playerTurn;
        Chess board;
        array<ofImage, 64> visualChess;
        ofImage bP, bR, bN, bB, bQ, bK, wP, wR, wN, wB, wQ, wK, emptySquare, wK_inverted;
        bool pieceHeld;
        int pieceHeldPos;
        ofImage pieceHeldImage;
        ofImage preMoveImage;
        bool promoting = false;
        int promotePos;
        bool botThinking;
        Chess::Move preMove { -1, -1, Chess::EMPTY };
        double whiteTime;
        double blackTime;
        int gamemode = 0; // 0=Bot(w) v Bot(b), 1 = Player(w) v Bot(b), 2 = Bot(w) v Player(b), 3 = Player(w) v Player(b)
        ofTrueTypeFont myfont;
        bool gameover = false;
        bool botMoved = false;
        bool gameStarted = false;
        int timeSec = 5*60;
        int timeWait = timeSec / 40;
        int timer = 0;
        int increment = 0;
        double* playerTime1;
        double* playerTime2;
        int whosTurn = 0;
        thread threadedBoard;
        thread timerThread;
        thread clockThread;
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
