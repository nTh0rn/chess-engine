
<a id="readme-top"></a>
<div align="center">
<h1 align="center">Terconari Chess Engine<h6>("Attempt thrice"→Latin→"Ter conari")</h6></h1>

  <p align="center">
    An entirely custom <a href="https://github.com/lichess-bot-devs/lichess-bot">lichess-bot</a> compatible chess engine made in C++
  </p>

  <p align="center">
<img src="/example-game.gif" width="500">
</p>
</div>

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about">About</a>
     <ul>
        <li><a href="#technical-specifications">Technical Specs</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
      </ul>
      <ul>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>



<!-- ABOUT -->
## About

Terconari is a chess-bot with a GUI made in C++ and [openFrameworks](https://openframeworks.cc/), compatible with [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot), and often live and playable at [lichess.org/Terconari](https://lichess.org/@/Terconari).

### Technical Specifications
 * Lichess-style GUI for local-only BotvBot, PlayervBot (premove functionality), or PlayervPlayer usage
 * GUI-less UCI mode compatible with [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot)
 * Opening book functionality
   - Default opening book has 244,117 games up to 16 plys deep, see [Usage→Opening Book](#opening-book) for more details
 * Make/Unmake legal move verification
   - Legal move generation tested against Stockfish's perft results across tens-of-millions of positions
 * Evaluation via material/positional information
   - Accounts for bishop pairs, passed pawns, and stacked pawns
   - Uses modified [Simplified Piece-Square Tables](https://www.chessprogramming.org/Piece-Square_Tables) with a custom pawn-endgame table
 * Negated Mini-Max with Alpha Beta Pruning
   - Non-pawn captures are always calculated an additional depth
 * Time-dependant Iterative Deepening
   - Attempts a depth of 6 plys
   - Escapes after `(timeLeft/50)/increment` seconds
 * Null Move Pruning
   - Does not run in positions that are in-check, have <= 4 legal moves, or have <= 6 total non-pawn pieces

A fully technical article about this project is coming soon to my website, [nthorn.com](https://nthorn.com)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- INSTALLATION -->
## Getting started

### Prerequisites

1. Visit [openFrameworks](https://openframeworks.cc/download/) and download the Windows→Visual Studio version.
2. Extract the downloaded zip to a directory of your choice.

### Installation

1. Clone/download the repository **within your openFrameworks installation directory, `apps/myApps/`**
   ```sh
   cd apps/myApps
   git clone https://github.com/nTh0rn/chess-engine.git
   ```
2. Launch `Chess Engine.sln` and confirm any refactoring prompts.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE -->
## Usage
**A starting screen GUI that allows for the modification of these settings is coming soon.**
1. Customize time control and gamemode by modifying class members within `/src/ofApp.h`.\
  1.1. `int gamemode` values: `0` = Bot(w) v Bot(b), `1` = Player(w) v Bot(b), `2` = Bot(w) v Player(b), `3` = Player(w) v Player(b), `4` = UCI Mode (For lichess-bot).\
  1.2. `int timeSec` The amount of time in seconds either side has.\
  1.3. `int increment` The amount of increment in seconds either side has.
2. Enable/disable debug messages by modifying `bool showDebugMessages` within `/include/Chess.h`.
3. Run in *Release* mode without the local debugger for significant performance increase over *Debug* mode.

### Opening Book
By default, Terconari comes with an opening book containing 244,115 games up to 16 plys deep obtained from [PGNMentor](https://www.pgnmentor.com/files.html) and stripped of duplicates.

Custom opening books compatible with Terconari can be made using [opening-book-PGN-to-UCI](https://github.com/nTh0rn/opening-book-PGN-to-UCI). Whichever opening book is to be used must be renamed to `book.txt` and placed within `/bin/data/openingbooks/`. See the function `openingBookMove()` within `/lib/Chess.cpp` if Terconari is unable to find its opening book.

### Lichess Bot
After compiling, the contents of `/bin/` can easily be moved to the `/engines/` folder of [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) per its documentation.

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Nikolas Thornton - [nthorn.com](https://nthorn.com) - contact@nthorn.com

<p align="right">(<a href="#readme-top">back to top</a>)</p>

