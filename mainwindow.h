#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVector>
#include <QPair>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QVector<QVector<QPushButton*>> boardSquares;
    QVector<QPair<int, int>> player1Pieces;
    QVector<QPair<int, int>> player2Pieces;
    QVector<QPair<int, int>> highlightedSquares;
    QPair<int, int> selectedPiece = {-1, -1};
    bool isPlayer1Turn = true;

    QString getPieceStyle(const QString &pieceText);
    void initializeBoard(QGridLayout *gridLayout);
    void promoteToKing(int row, int col);
    void onSquareClicked(int row, int col);
    void resetSelection();
    void movePiece(QPair<int, int> from, QPair<int, int> to);
    void performJump(QPair<int, int> from, QPair<int, int> to);
    bool isValidMove(QPair<int, int> from, QPair<int, int> to);
    bool canJump(QPair<int, int> from, QPair<int, int> to);
    void highlightLegalMoves(QPair<int, int> piece);
    void clearHighlightedSquares();
    bool checkWinCondition();
    void declareWin(int player);
    void resetGame();
};

#endif
