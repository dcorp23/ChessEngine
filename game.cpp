#include "game.hpp"
#include "moveGen.hpp"
#include "evaluation.hpp"
#include "chessBoard.hpp"
#include "attackTables.hpp"
#include "moveGen.hpp"
#include "search.hpp"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <future>
#include <queue>
#define WIDTH 600
#define HEIGHT 600
#define SQUARE_SIZE (std::min(WIDTH, HEIGHT) / 8)

SDL_Texture* boardTexture;
SDL_Texture* pieceTexture;
SDL_Rect boardRect;

std::vector<std::future<void>> futures;

EvaluationWeights weights = EvaluationWeights();

void Game::init() {
    running = false;
    AttackTables::initAttackTables();
    Evaluation::initEvaluation();
    std::cout << "Pick a side: white = 1, black = 0: ";
    std::cin >> playerSide;
    board = Board(startingFEN);

    if (board.state.whiteToMove == playerSide) engineTurn = false;
    else {engineTurn = true;}
    promotion = false;
    promotionSquare = 0;

    SDL_Init(SDL_INIT_EVERYTHING);

    //create the window
    window = SDL_CreateWindow("Chess Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI );
    if ( NULL == window ) {
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        return;
    }

    //create the renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL) {
        std::cout << "Could not create renderer: " << SDL_GetError() << '\n';
        return;
    }
    SDL_SetRenderDrawColor(renderer, 119, 191, 234, 255);

    //create board texture
    SDL_Surface* tmpSurface = IMG_Load("assets/Chessboard480.svg.png");
    boardTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    //create texture for all pieces
    tmpSurface = IMG_Load("assets/800px-Chess_Pieces_Sprite.svg.png");
    pieceTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    //create rectangle for where board is drawn
    //top left and size is square length min of width height
    boardRect.x = 0;
    boardRect.y = 0;
    boardRect.w = std::min(WIDTH, HEIGHT);
    boardRect.h = std::min(WIDTH, HEIGHT);

    //clear the filtered move list
    filteredMoveList.clear();

    //set all possible next boards for the next move
    possibleNextBoards = MoveGenerator::getAllLegalBoards(board);

    //set game to run
    running = true;
}

//handles clicks and quiting out of the window
void Game::handleEvents() {
    SDL_Event event;
    SDL_WaitEvent(&event);
    //filter moving mouse as an event
    while (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP) {
        SDL_WaitEvent(&event);
    }
    //quit
    if (event.type == SDL_QUIT) {
        running = false;
    }
    //click
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        //left click, click a square and update will check if its valid
        if (event.button.button == SDL_BUTTON_LEFT) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            int file = x/SQUARE_SIZE;
            int rank = y/SQUARE_SIZE;
            //calculate clickedSquare
            clickedSquare = (rank * 8) + file;
        }
        //right click unselect everything
        if (event.button.button == SDL_BUTTON_RIGHT) { 
            clickedSquare = -1;
            selectedPieceSquare = -1;
            promotion = false;
            promotionSquare = -1;
            filteredMoveList.clear();
        }
    }
}

//gets a piece on a given square and board
//if the square is empty then it will give -1
int getPieceOnSquare(int square, Board board) {
    map mask = 1ULL << square;
    int piece = -1;
    for (int i = WPawn; i <= BKing; i++) {
        if (mask & board.bitMaps[i]) piece = i % 6;
    }
    return piece;
}

//gets the move code list for the piece type
std::vector<MoveCode> getMoveList(int piece, Board board) {
    MoveCode moveCode;
    std::vector<MoveCode> moveList;
    if (piece == 0) MoveGenerator::getPawnMoves(board, &moveList);
    if (piece == 1) MoveGenerator::getBishopMoves(board, &moveList);
    if (piece == 2) MoveGenerator::getKnightMoves(board, &moveList);
    if (piece == 3) MoveGenerator::getRookMoves(board, &moveList);
    if (piece == 4) MoveGenerator::getQueenMoves(board, &moveList);
    if (piece == 5) MoveGenerator::getKingMoves(board, &moveList);
    return moveList;
}

void Game::engineMove() {
    Board nextBoard;
    int bestIndex = 0;
    float bestEval = board.state.whiteToMove ? -INFINITY : INFINITY;
    float alpha = -INFINITY;
    float beta = INFINITY;
    
    int boardsSize = possibleNextBoards.size();
    for (int i = 0; i < boardsSize; i++) {
        if (futures.size() < NUM_THREADS) {
            futures.push_back(std::async(std::launch::async, Search::threadMinMax, &possibleNextBoards.at(i), i, &bestIndex, &bestEval, MAX_DEPTH, alpha, beta, weights));
        }
        else {
            //waits while the max number of threads are active
            int j = 0;
            while (true) {
                //checks if a thread has finnished then if it has then break out and start a new thread
                if (futures.at(j).wait_for(std::chrono::milliseconds(500)) == std::future_status::ready) {
                    futures.erase(futures.begin() + j);
                    break;
                }
                j++;
                if (j == futures.size()) j = 0;
            }
            i--;
        }
    }

    //wait for threads to finish
    //while threads are still active
    int size = futures.size();
    while (size != 0) {
        for (int i = 0; i < futures.size(); i++) { //loop through threads
            //if a thread is done remove it
            if (futures.at(i).wait_for(std::chrono::milliseconds(500)) == std::future_status::ready) {
                futures.erase(futures.begin() + i);
                size--;
            }
        }
    }
    
    //get the best best board
    nextBoard = possibleNextBoards.at(bestIndex);
    boardHistory.push(board);
    board = nextBoard;
    possibleNextBoards = MoveGenerator::getAllLegalBoards(board);
    engineTurn = false;
}

//updates the state of the game
void Game::update() {
    if (playerSide == board.state.whiteToMove) {
        if (clickedSquare != -1) {
            map clickMask = 1ULL << clickedSquare;
            if (filteredMoveList.empty()) {
                if (clickMask & (playerSide ? board.White : board.Black)) {
                    selectedPieceSquare = clickedSquare;

                    //find which piece was clicked
                    int piece = getPieceOnSquare(clickedSquare, board);

                    //find all moves from that pieced
                    std::vector<MoveCode> moveList = getMoveList(piece, board);

                    filteredMoveList.reserve(moveList.size());
                    int moveListSize = moveList.size();

                    //get all moves that originate from the selected square
                    for (int i = 0; i < moveListSize; i++) {
                        MoveCode tempMoveCode = moveList.at(i);
                        if (tempMoveCode.startSquare == selectedPieceSquare) {
                            filteredMoveList.push_back(tempMoveCode);
                        }
                    }
                    return;
                }
            } 
            if (!filteredMoveList.empty()) {
                if (promotion) {
                    int promotionPiece = 0;
                    if (playerSide) {
                        if (clickedSquare == promotionSquare + (0 * 8)) promotionPiece = 4;
                        if (clickedSquare == promotionSquare + (1 * 8)) promotionPiece = 3;
                        if (clickedSquare == promotionSquare + (2 * 8)) promotionPiece = 2;
                        if (clickedSquare == promotionSquare + (3 * 8)) promotionPiece = 1;
                    } else {
                        if (clickedSquare == promotionSquare - (0 * 8)) promotionPiece = 4;
                        if (clickedSquare == promotionSquare - (1 * 8)) promotionPiece = 3;
                        if (clickedSquare == promotionSquare - (2 * 8)) promotionPiece = 2;
                        if (clickedSquare == promotionSquare - (3 * 8)) promotionPiece = 1;
                    }
                    Board newBoard = board;
                    for (int i = 0; i < filteredMoveList.size(); i++) {
                        MoveCode moveCode = filteredMoveList.at(i);
                        if (moveCode.endSquare == promotionSquare && moveCode.promotion == promotionPiece) {
                            newBoard = board.move(filteredMoveList.at(i));
                        }
                    }
                    int possibleBoardsSize = possibleNextBoards.size();
                    for (int i = 0; i < possibleBoardsSize; i++) {
                        if (newBoard.isEqual(possibleNextBoards.at(i))) {
                            boardHistory.push(possibleNextBoards.at(i));
                            board = possibleNextBoards.at(i);
                            possibleNextBoards = MoveGenerator::getAllLegalBoards(board);
                            engineTurn = true;
                            break;
                        }
                    }
                    if (board.isEqual(newBoard)) {
                        selectedPieceSquare = -1;
                        promotion = false;
                        promotionSquare = -1;
                        filteredMoveList.clear();
                    }
                } else {
                    Board newBoard = board;
                    for (int i = 0; i < filteredMoveList.size(); i++) {
                        if (filteredMoveList.at(i).endSquare == clickedSquare) {
                            if (filteredMoveList.at(i).promotion) {
                                promotion = true;
                                promotionSquare = clickedSquare;
                                clickedSquare == -1;
                                return;
                            }
                            newBoard = board.move(filteredMoveList.at(i));
                        }
                    }
                    int possibleBoardsSize = possibleNextBoards.size();
                    for (int i = 0; i < possibleBoardsSize; i++) {
                        if (newBoard.isEqual(possibleNextBoards.at(i))) {
                            boardHistory.push(possibleNextBoards.at(i));
                            board = possibleNextBoards.at(i);
                            possibleNextBoards = MoveGenerator::getAllLegalBoards(board);
                            engineTurn = true;
                            break;
                        }
                    }
                    if (board.isEqual(newBoard)) {
                        selectedPieceSquare = -1;
                        filteredMoveList.clear();
                    }
                }
            }
        }
        clickedSquare == -1;
    }
}

//pieces 0-5 pawn, bishop, knight, rook, queen, king
void renderSinglePiece(int side, int piece, int rank, int file, SDL_Renderer* renderer) {
    SDL_Rect imageRect, placementRect;
    imageRect.h = 133;
    imageRect.w = 133;
    if (side) imageRect.y = 0;
    else imageRect.y = 133; //half of the height of pieces image
    switch (piece)
    {
    case 0:
        imageRect.x = 5 * 133;
        break;
    case 1:
        imageRect.x = 2 * 133;
        break;
    case 2:
        imageRect.x = 3 * 133;
        break;
    case 3:
        imageRect.x = 4 * 133;
        break;
    case 4:
        imageRect.x = 1 * 133;
        break;
    case 5:
        imageRect.x = 0 * 133;
        break;
    default:
        break;
    }
    placementRect.x = file * SQUARE_SIZE;
    placementRect.y = rank * SQUARE_SIZE;
    placementRect.h = SQUARE_SIZE;
    placementRect.w = SQUARE_SIZE;
    SDL_RenderCopy(renderer, pieceTexture, &imageRect, &placementRect);
}

void renderPieces(SDL_Renderer* renderer, Board board) {
    map currentBit;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            currentBit = (1ULL << ((rank * 8) + file));

            if (currentBit & board.All) {
                if (currentBit & board.White) {
                    if (currentBit & board.bitMaps[WPawn]) renderSinglePiece(1, 0, rank, file, renderer);
                    if (currentBit & board.bitMaps[WKnight]) renderSinglePiece(1, 2, rank, file, renderer);
                    if (currentBit & board.bitMaps[WBishop]) renderSinglePiece(1, 1, rank, file, renderer);
                    if (currentBit & board.bitMaps[WRook]) renderSinglePiece(1, 3, rank, file, renderer);
                    if (currentBit & board.bitMaps[WQueen]) renderSinglePiece(1, 4, rank, file, renderer);
                    if (currentBit & board.bitMaps[WKing]) renderSinglePiece(1, 5, rank, file, renderer);
                } else {
                    if (currentBit & board.bitMaps[BPawn]) renderSinglePiece(0, 0, rank, file, renderer);
                    if (currentBit & board.bitMaps[BKnight]) renderSinglePiece(0, 2, rank, file, renderer);
                    if (currentBit & board.bitMaps[BBishop]) renderSinglePiece(0, 1, rank, file, renderer);
                    if (currentBit & board.bitMaps[BRook]) renderSinglePiece(0, 3, rank, file, renderer);
                    if (currentBit & board.bitMaps[BQueen]) renderSinglePiece(0, 4, rank, file, renderer);
                    if (currentBit & board.bitMaps[BKing]) renderSinglePiece(0, 5, rank, file, renderer);
                }
            }
        }
    }
}

void renderPromotionPieces(SDL_Renderer* renderer, int square, int side) {
    //create background over the board to differentiate the promotion pieces
    SDL_Rect backgroundRect;
    backgroundRect.h = 4 * SQUARE_SIZE;
    backgroundRect.w = SQUARE_SIZE;

    //render white rectangle under the pieces
    if (!side) backgroundRect.y = 4 * SQUARE_SIZE;
    else backgroundRect.y = 0;

    backgroundRect.x = (square % 8) * SQUARE_SIZE;

    SDL_RenderFillRect(renderer, &backgroundRect);

    //render the pieces on top of the background
    int rank = square / 8;
    int file = square % 8;

    if (side) {
        renderSinglePiece(1, 4, rank, file, renderer);
        renderSinglePiece(1, 3, rank + 1, file, renderer);
        renderSinglePiece(1, 2, rank + 2, file, renderer);
        renderSinglePiece(1, 1, rank + 3, file, renderer);
    } else {
        renderSinglePiece(0, 4, rank, file, renderer);
        renderSinglePiece(0, 3, rank - 1, file, renderer);
        renderSinglePiece(0, 2, rank - 2, file, renderer);
        renderSinglePiece(0, 1, rank - 3, file, renderer);
    }
}


void renderSelectedSquare(SDL_Renderer* renderer, int square) {
    SDL_Rect selectedSquare = {(square % 8) * SQUARE_SIZE, (square / 8) * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
    SDL_RenderDrawRect(renderer, &selectedSquare);
}

void Game::render() {
    SDL_RenderClear(renderer);
    //render the board
    SDL_RenderCopy(renderer, boardTexture, NULL, &boardRect);
    //render the pieces
    renderPieces(renderer, board);
    //render selected square
    if (selectedPieceSquare) renderSelectedSquare(renderer, selectedPieceSquare);
    if (promotion) renderPromotionPieces(renderer, promotionSquare, playerSide);
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "game exited successfully\n";
}