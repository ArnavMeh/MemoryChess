#ifndef Board_H
#define Board_H

using namespace std;


class Board { 
  
  public:

    bool turn = true;
    uint64_t pieces[16] = {
      0, 0, 0, 0, 0, 0, //white pieces
      0, 0, 0, 0, 0, 0,  //black pieces
      0, 0, //colors
      0, 0 //combined, empty
    };
    //num of pieces moved from sq, use to determine castling
    uint8_t wKingSqMoves=0; uint8_t wLRookSqMoves=0; uint8_t wRRookSqMoves=0; 
    uint8_t bKingSqMoves=0; uint8_t bLRookSqMoves=0; uint8_t bRRookSqMoves=0;
    int randomness = 0;
    // int eval = 0;

    
    

    // generate all or just capturing moves
    void moveGen (vector<Move> &moves);
    void captureGen (vector<Move> &moves);
    bool canRightCastle();
    bool canLeftCastle();

    //used for legal move generation or castling rights
    bool isCheck();
    bool isCheck (uint8_t currSquare);

    //making moves
    constexpr void toggleMove() { turn = !turn; }
    void move (Move &move);
    void unmove (Move &move);
    
    //evaluating a position, uses positional eval table and material count
    int16_t evaluate();

    //other functions for setting up and printing board
    constexpr void initializeBoard() {
      turn = true;
      pieces[Piece::WHITE] = UINT64_C(18446462598732840960);
      pieces[Piece::BLACK] = UINT64_C(65535);
      //pawns
      pieces[0] = UINT64_C(71776119061282560) & UINT64_C(18446462598732840960);
      pieces[6] = UINT64_C(71776119061282560) & UINT64_C(65535);
      //knights and bishops
      pieces[1] = UINT64_C(4755801206503243842) & UINT64_C(18446462598732840960);
      pieces[7] = UINT64_C(4755801206503243842) & UINT64_C(65535);
      pieces[2] = UINT64_C(2594073385365405732) & UINT64_C(18446462598732840960);
      pieces[8] = UINT64_C(2594073385365405732) & UINT64_C(65535);
      //rooks
      pieces[3] = UINT64_C(9295429630892703873) & UINT64_C(18446462598732840960);
      pieces[9] = UINT64_C(9295429630892703873) & UINT64_C(65535);
      //queen and king
      pieces[4] = UINT64_C(576460752303423496) & UINT64_C(18446462598732840960);
      pieces[10] = UINT64_C(576460752303423496) & UINT64_C(65535);
      pieces[5] = UINT64_C(1152921504606846992) & UINT64_C(18446462598732840960);
      pieces[11] = UINT64_C(1152921504606846992) & UINT64_C(65535);

      pieces[14] = UINT64_C(18446462598732840960) | UINT64_C(65535);
      pieces[15] = ~pieces[14];
    }
    constexpr void setBoard (char boardArr[64]) {
      clear();
      for (int i=0; i<64; i++) {
        uint64_t val = 1ULL << i;
        switch (boardArr[i]) {
          case 'r': pieces[Piece::BLACK]+=val; pieces[Piece::B_ROOK]+=val; break;
          case 'n': pieces[Piece::BLACK]+=val; pieces[Piece::B_KNIGHT]+=val; break;
          case 'b': pieces[Piece::BLACK]+=val; pieces[Piece::B_BISHOP]+=val; break;
          case 'q': pieces[Piece::BLACK]+=val; pieces[Piece::B_QUEEN]+=val; break;
          case 'k': pieces[Piece::BLACK]+=val; pieces[Piece::B_KING]+=val; break;
          case 'p': pieces[Piece::BLACK]+=val; pieces[Piece::B_PAWN]+=val; break;
          
          case 'R': pieces[Piece::WHITE]+=val; pieces[Piece::W_ROOK]+=val; break;
          case 'N': pieces[Piece::WHITE]+=val; pieces[Piece::W_KNIGHT]+=val; break;
          case 'B': pieces[Piece::WHITE]+=val; pieces[Piece::W_BISHOP]+=val; break;
          case 'Q': pieces[Piece::WHITE]+=val; pieces[Piece::W_QUEEN]+=val; break;
          case 'K': pieces[Piece::WHITE]+=val; pieces[Piece::W_KING]+=val; break;
          case 'P': pieces[Piece::WHITE]+=val; pieces[Piece::W_PAWN]+=val; break;
        }
      }
      pieces[Piece::OCCUP] = pieces[Piece::WHITE] ^ pieces[Piece::BLACK];
      pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP];

      if ((pieces[Piece::W_KING] & (1ULL << 60)) == 0) { wKingSqMoves++; }
      if ((pieces[Piece::B_KING] & (1ULL << 4)) == 0) { bKingSqMoves++; }
      if ((pieces[Piece::W_ROOK] & (1ULL << 56)) == 0) { wLRookSqMoves++; }
      if ((pieces[Piece::W_ROOK] & (1ULL << 63)) == 0) { wRRookSqMoves++; }
      if ((pieces[Piece::B_ROOK] & (1ULL << 0)) == 0) { bLRookSqMoves++; }
      if ((pieces[Piece::B_ROOK] & (1ULL << 7)) == 0) { bRRookSqMoves++; }
    }
    void setBoardFen (string FENStr) {
      char boardStr[64];
      for (int i=0, sq=0; i<FENStr.size(); i++) {
        char letter = FENStr[i];
        if (letter != '/') {
          if (isdigit(letter)) {
            sq += letter - 48;
          } else {
            boardStr[sq] = FENStr[i];
            sq++;
          }
        }
      }
      setBoard(boardStr);
    }
    void printBoard() {
      cout << '\n';
      bool currTurn = turn;

      for (int i=0; i<64; i++) {
        turn = false;
        int whitePiece = getCapturedPiece(i);
        turn = true;
        int blackPiece = getCapturedPiece(i);
        
        if (whitePiece != 16) {
          cout << Piece::letters[whitePiece];
        } else if (blackPiece != 16) {
          cout << Piece::letters[blackPiece];
        } else {
          cout << '.';
        }

        cout << " ";
        if (i%8 == 7) {
          cout << '\n';
        }
      }

      turn = currTurn;
    }
    constexpr void clear() {
      for (int i=0; i<15; i++) {
        pieces[i] = 0;
      }
      wKingSqMoves = wLRookSqMoves = wRRookSqMoves = 0; 
      bKingSqMoves = bLRookSqMoves = bRRookSqMoves = 0;
    }
    string getBoardStr() {
        string boardStr;
        for (int i = 0; i < 64; i++) {
            int pNum = getPiece(i);
            if (pNum < 12) {
              boardStr += Piece::letters[pNum];
            } else {
              boardStr += ' ';
            }
        }

        return boardStr;
    }

    inline array<uint64_t,7> getBoardKey();
    

    //returns piece captured as piece[] index
    int8_t getCapturedPiece(int8_t currSq) {
      uint64_t currVal = 1ULL << currSq;
      int8_t end = turn? 12 : 6;
      for (int8_t start = turn? 6 : 0; start < end; start++) {
        if ((pieces[start] & currVal)) { return start; }
      }
      return 12;
    }
    int8_t getPiece(int8_t currSq) {
      uint64_t currVal = 1ULL << currSq;
      for (int8_t start = 0; start < 12; start++) {
        if ((pieces[start] & currVal)) { return start; }
      }
      return 12;
    }


  private: 

    //just shorthand methods, might want to make macro
    inline uint64_t getP() { return pieces[0];  }
    inline uint64_t getp() { return pieces[6];  }
    inline uint64_t getN() { return pieces[1];  }
    inline uint64_t getn() { return pieces[7];  }
    inline uint64_t getB() { return pieces[2];  }
    inline uint64_t getb() { return pieces[8];  }
    inline uint64_t getR() { return pieces[3];  }
    inline uint64_t getr() { return pieces[9];  }
    inline uint64_t getQ() { return pieces[4];  }
    inline uint64_t getq() { return pieces[10]; }
    inline uint64_t getK() { return pieces[5];  }
    inline uint64_t getk() { return pieces[11]; }
    inline uint64_t getOccupied() { return pieces[14]; }
    inline uint64_t getUnoccupied() { return pieces[15]; }
    inline uint64_t getTurn() { return pieces[turn? 12 : 13]; }
    inline uint64_t getNotTurn() { return pieces[turn? 13 : 12]; }

    constexpr void setWhite() { pieces[Piece::WHITE] = pieces[Piece::W_PAWN] ^ pieces[Piece::W_KNIGHT] ^ pieces[Piece::W_BISHOP] ^ pieces[Piece::W_ROOK] ^ pieces[Piece::W_QUEEN] ^ pieces[Piece::W_KING]; }
    constexpr void setBlack() { pieces[Piece::BLACK] = pieces[Piece::B_PAWN] ^ pieces[Piece::B_KNIGHT] ^ pieces[Piece::B_BISHOP] ^ pieces[Piece::B_ROOK] ^ pieces[Piece::B_QUEEN] ^ pieces[Piece::B_KING]; }
    constexpr void setOccupied() { pieces[Piece::OCCUP] = pieces[Piece::BLACK] | pieces[Piece::WHITE]; }
    constexpr void setUnoccupied() { pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP]; }
};

#endif