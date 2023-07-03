#include <iostream>
#include <random>
#include <vector>
#include "moveGen.hpp"
#include "chessBoard.hpp"
#include "search.hpp"
#include "evaluation.hpp"

#define NUM_WEIGHTS 20
#define NUM_BOARDS 5

std::random_device rd;
std::mt19937 gen(rd());

struct WeightBounds
{
    std::uniform_real_distribution<> rfg;

    WeightBounds(float lower, float upper) {
        this->rfg = std::uniform_real_distribution<> (lower, upper);
    }
};

//set the bounds to generate a random weight for each evaluation area
WeightBounds kingSafetyBounds = WeightBounds(.4, 1.5);
WeightBounds activityBounds = WeightBounds(.6, 2);
WeightBounds pawnBounds = WeightBounds(.3, 1);
WeightBounds bishopBounds = WeightBounds(.3, 1);
WeightBounds knightBounds = WeightBounds(.3, 1);
WeightBounds rookBounds = WeightBounds(.3, 1);
WeightBounds queenBounds = WeightBounds(.3, 1);

std::string startingBoardsFEN[NUM_BOARDS] {
    startingFEN, 
    "rnbqkb1r/1p2pppp/p2p1n2/8/3NP3/2N5/PPP2PPP/R1BQKB1R w KQkq - 0 6", 
    "rnbqkb1r/pp2pppp/2p2n2/8/P1pP4/2N2N2/1P2PPPP/R1BQKB1R b KQkq - 2 5", 
    "rnbq1rk1/ppppppbp/5np1/8/8/5NP1/PPPPPPBP/RNBQ1RK1 w - - 4 5",
    "rnbqkbnr/ppp2ppp/4p3/3p4/3PP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 3"
};

int main (void) {
    AttackTables::initAttackTables();
    Evaluation::initEvaluation();
    //randomize evaluation weights
    //in some length array
    //loop through that array and start a game from them
    //with a 4 set positions and the starting position
    //remove each of the losers and if there is a draw then we 
    //save the second set of weights and keep going with the original
    //if the original loses then we add all the other weights it drew with back
    //keep going till there is only one set of weights left if there are some left
    //over that it drew with then it will pick one at random and say that that is the
    //best set of weights that we generated
    std::vector<EvaluationWeights> randomWeights;
    std::vector<EvaluationWeights> drawnWeights;
    for (int i = 0; i < NUM_WEIGHTS; i++) {
        EvaluationWeights weights;
        weights.kingSafety = kingSafetyBounds.rfg(gen);
        weights.activity = activityBounds.rfg(gen);
        weights.pawn = pawnBounds.rfg(gen);
        weights.bishop = bishopBounds.rfg(gen);
        weights.knight = knightBounds.rfg(gen);
        weights.rook = rookBounds.rfg(gen);
        weights.queen = queenBounds.rfg(gen);
        randomWeights.push_back(weights);
    }

    while (randomWeights.size() > 1) {
        EvaluationWeights firstWeight = randomWeights.at(0);
        EvaluationWeights secondWeight = randomWeights.at(1);
        int firstWins = 0;
        int secondWins = 0;

        std::cout << "Starting games first as white\n";

        //all these games will be played with first weight as white
        for (int i = 0; i < NUM_BOARDS; i++) {
            Board currentBoard = startingBoardsFEN[i];
            while (true) {
                if (currentBoard.state.checkMate) { //add for checkmate
                    if (currentBoard.state.whiteToMove) secondWins++;
                    if (!currentBoard.state.whiteToMove) firstWins++;
                    break;
                }
                std::vector<Board> boards = MoveGenerator::getAllLegalBoards(currentBoard);
                if (boards.empty()) break; //there is a draw

                EvaluationWeights weightsToMove; //the weights that will be used for the evaluation based on whose move
                if (currentBoard.state.whiteToMove) weightsToMove = firstWeight;
                else weightsToMove = secondWeight;

                int bestIndex = 0;
                float bestEval = currentBoard.state.whiteToMove ? -INFINITY : INFINITY;
                float eval;
                float alpha = -INFINITY;
                float beta = INFINITY;
                int boardsSize = boards.size();
                for (int j = 0; j < boardsSize; j++) {
                    eval = Search::minimax(&boards.at(j), MAX_DEPTH, !currentBoard.state.whiteToMove, alpha, beta, weightsToMove);
                    if (currentBoard.state.whiteToMove ? eval > bestEval : eval < bestEval) {
                        bestEval = eval;
                        bestIndex = j;
                    }
                }
                //get the best best board
                currentBoard = boards.at(bestIndex);
            }
            std::cout << "Game " << i << " done\n";
        }

        std::cout << "Starting games first as black\n";

        //all these games will be played with first weight as black
        for (int i = 0; i < NUM_BOARDS; i++) {
            Board currentBoard = startingBoardsFEN[i];
            while (true) {
                if (currentBoard.state.checkMate) { //add for checkmate
                    if (currentBoard.state.whiteToMove) firstWins++;
                    if (!currentBoard.state.whiteToMove) secondWins++;
                    break;
                }
                std::vector<Board> boards = MoveGenerator::getAllLegalBoards(currentBoard);
                if (boards.empty()) break; //there is a draw

                EvaluationWeights weightsToMove; //the weights that will be used for the evaluation based on whose move
                if (currentBoard.state.whiteToMove) weightsToMove = secondWeight;
                else weightsToMove = firstWeight;

                int bestIndex = 0;
                float bestEval = currentBoard.state.whiteToMove ? -INFINITY : INFINITY;
                float eval;
                float alpha = -INFINITY;
                float beta = INFINITY;
                int boardsSize = boards.size();
                for (int j = 0; j < boardsSize; j++) {
                    eval = Search::minimax(&boards.at(j), MAX_DEPTH, !currentBoard.state.whiteToMove, alpha, beta, weightsToMove);
                    if (currentBoard.state.whiteToMove ? eval > bestEval : eval < bestEval) {
                        bestEval = eval;
                        bestIndex = j;
                    }
                }
                //get the best best board
                currentBoard = boards.at(bestIndex);
            }
            std::cout << "Game " << i << " done\n";
        }

        if (firstWins == secondWins) { //draw in total wins
            drawnWeights.push_back(secondWeight);
            randomWeights.erase(randomWeights.begin() + 1);
            std::cout << "There Was A Draw\n\n";
            continue;
        }
        if (firstWins > secondWins) { //first weights wins
            randomWeights.erase(randomWeights.begin() + 1);
            std::cout << "First Weights Wins\n\n";
        }
        if (firstWins < secondWins) { //second weights wins
            randomWeights.erase(randomWeights.begin());
            std::cout << "Second Weights Wins\n\n";
        }
        int drawnWeightsSize = drawnWeights.size();
        for (int i = drawnWeightsSize; i > 0; i--) {
            randomWeights.push_back(drawnWeights.at(i - 1));
            drawnWeights.erase(drawnWeights.end());
        }
    }

    randomWeights.at(0).printWeights();
    
    return 0;
}