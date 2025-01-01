
<a id="readme-top"></a>
<div align="center">
<h1 align="center">Terconari Chess Engine<h6>("Attempt thrice"->Latin->"Ter conari")</h6></h1>

  <p align="center">
    An entirely custom lichess-compatible chess engine in C++.
  </p>

  <p align="center">
<img src="https://nthorn.com/images/chess-engine/pgn-to-uci.png" width="500">
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

Terconari is a chess-bot with a GUI made in C++ and [OpenFrameworks](https://openframeworks.cc/), compatible with [lichess-bot](https://github.com/lichess-bot-devs/lichess-bot) and often live and playable at [lichess.org/Terconari](https://lichess.org/@/Terconari).

### Technical Specifications
 * Make/Unmake Legal Move Verification
   - Fully tested against Stockfish's perft results across millions of positions.
 * Board Representation using `array<char,64>`
 * Negated Mini-Max with Alpha Beta Pruning
   - Non-pawn captures are always calculated regardless of depth
 * Evaluation via Material/Positional Information
   - Accounts for bishop pairs, passed pawns, and stacked pawns
   - Uses modified [Simplified Piece-Square Tables](https://www.chessprogramming.org/Piece-Square_Tables) with a custom pawn-endgame table
 * Iterative Deepening
   - Attempts a depth of 6 plys
 * Null Move Pruning
   - skips in positions that are in-check, have < 5 legal moves, or have < 7 total non-pawn pieces

A fully technical article about this project is coming soon to my website, [nthorn.com](https://nthorn.com)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- INSTALLATION -->
## Getting started

### Prerequisites

1. Install Chess and Glob
   ```sh
   pip install Chess
   pip install Glob
   ```

### Installation

1. Clone/download the repo
   ```sh
   git clone https://github.com/nTh0rn/opening-book-PGN-to-UCI.git
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE -->
## Usage
1. Place all PGNs inside a folder titled "PGNs" in same directory as PGN-to-UCI.py.
2. Execute PGN-to-UCI.py.
3. The output will be created in a text document UCI.txt in the same directory as PGN-to-UCI.py.

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Nikolas Thornton - [nthorn.com](https://nthorn.com) - contact@nthorn.com

<p align="right">(<a href="#readme-top">back to top</a>)</p>

