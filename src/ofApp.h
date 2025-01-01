#pragma once

#include "ofMain.h"
#include "../include/Chess.h"
#include <chrono>

class ofApp : public ofBaseApp{

    public:
        void setup(); // Ran upon startup
        void update(); // Runs on loop for logic, exits to draw()
        void draw(); // Runs on loop for drawing visuals, exits to setup()

        void drawBoard(); // Draw the board visuals
        void updateVisualChess(); // Update the array that holds visual board information
        void makeMove(Chess::Move move); // Execute a move on board.
        void tryMove(Chess::Move move); // Attempt a move on board.
        void timerRun(Chess* b); // Bot's move timer, controls panic levels.
        void clockRun(); // The game's clock, declares winner if time runs out.
        void drawClock(); // Draw the clock itself.
        void makeBotMove(); // Execute a bot move on board.
        bool startScreen = false; // The start screen, not currently finished.
        bool botInitMove = false; // Initializes the bot's turn
        Chess board; // The main board
        array<ofImage, 64> visualChess; // The visual board
        ofImage bP, bR, bN, bB, bQ, bK, wP, wR, wN, wB, wQ, wK, emptySquare, wK_inverted; // The piece iamges
        bool pieceHeld; // Whether user is holding a piece
        int pieceHeldPos; // Where user is holding a piece
        ofImage pieceHeldImage; // Image of held piece
        ofImage preMoveImage; // Image of premove piece
        bool promoting = false; // Whether promoting
        int promotePos; // Promoting position
        bool botThinking; // Whether the bot is thinking (Used for disallowing user-moves)
        bool botMoved = false; // Whether bot has moved (Used for thread-termination of timerRun())
        Chess::Move preMove { -1, -1, Chess::EMPTY }; // Premove
        double whiteTime; // White's time in seconds
        double blackTime; // Black's time in seconds
        int gamemode = 0; // 0=Bot(w) v Bot(b), 1 = Player(w) v Bot(b), 2 = Bot(w) v Player(b), 3 = Player(w) v Player(b), 4 = UCI Mode (For lichess-bot.py)
        ofTrueTypeFont mainFont; // Default font
        bool gameover = false; // If game has ended
        bool gameStarted = false; // If game has started
        int timeSec = 10*60; // Generic time for both sides in seconds
        int increment = 0; // Increment in seconds
        int whosTurn = 0; // Interface-specific whosTurn
        thread threadedBoard; // Bot's thread
        thread timerThread; // Bot's timer thread for panic-levels
        thread clockThread; // Clock thread
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
