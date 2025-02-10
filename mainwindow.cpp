#include "mainwindow.h"
#include <QGridLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Checkers by Shivam Sharma");
    setFixedSize(480, 480);

    QWidget *boardWidget = new QWidget(this);
    setCentralWidget(boardWidget);

    QGridLayout *gridLayout = new QGridLayout(boardWidget);
    gridLayout->setSpacing(0);
    initializeBoard(gridLayout);
    boardWidget->setLayout(gridLayout);
}

QString MainWindow::getPieceStyle(const QString &pieceText) {
    if(pieceText.startsWith("P1") || pieceText.startsWith("K1")) {
        return pieceText.startsWith("K")
        ? "background-color: black; color: blue; font-weight: bold; font-style: italic;"
        : "background-color: black; color: blue; font-weight: bold;";
    }
    if(pieceText.startsWith("P2") || pieceText.startsWith("K2")) {
        return pieceText.startsWith("K")
        ? "background-color: black; color: red; font-weight: bold; font-style: italic;"
        : "background-color: black; color: red; font-weight: bold;";
    }
    return "";
}

void MainWindow::initializeBoard(QGridLayout *gridLayout) {
    player1Pieces.clear();
    player2Pieces.clear();
    boardSquares.clear();

    for (int row = 0; row < 10; ++row) {
        QVector<QPushButton*> rowButtons;
        for (int col = 0; col < 10; ++col) {
            QPushButton *square = new QPushButton(this);
            square->setFixedSize(48, 48);

            if ((row + col) % 2 == 0) {
                square->setStyleSheet("background-color: white;");
            } else {
                square->setStyleSheet("background-color: black;");
                if (row < 4) {
                    square->setText("P2");
                    player2Pieces.append({row, col});
                    square->setStyleSheet(getPieceStyle("P2"));
                } else if (row > 5) {
                    square->setText("P1");
                    player1Pieces.append({row, col});
                    square->setStyleSheet(getPieceStyle("P1"));
                }
            }

            connect(square, &QPushButton::clicked, this, [=]() { onSquareClicked(row, col); });
            gridLayout->addWidget(square, row, col);
            rowButtons.append(square);
        }
        boardSquares.append(rowButtons);
    }
}

void MainWindow::promoteToKing(int row, int col) {
    QString pieceText = boardSquares[row][col]->text();
    if (pieceText == "P1" && row == 0) {
        boardSquares[row][col]->setText("K1");
        boardSquares[row][col]->setStyleSheet(getPieceStyle("K1"));
    } else if (pieceText == "P2" && row == 9) {
        boardSquares[row][col]->setText("K2");
        boardSquares[row][col]->setStyleSheet(getPieceStyle("K2"));
    }
}

void MainWindow::onSquareClicked(int row, int col) {
    if (selectedPiece.first == -1) {
        QString pieceText = boardSquares[row][col]->text();
        if ((isPlayer1Turn && (pieceText == "P1" || pieceText == "K1")) ||
            (!isPlayer1Turn && (pieceText == "P2" || pieceText == "K2"))) {
            selectedPiece = {row, col};
            boardSquares[row][col]->setStyleSheet(
                QString("background-color: yellow; color: %1; font-weight: bold;")
                    .arg(isPlayer1Turn ? "blue" : "red"));
            highlightLegalMoves(selectedPiece);
        }
    } else {
        if (isValidMove(selectedPiece, {row, col})) {
            if (canJump(selectedPiece, {row, col})) {
                performJump(selectedPiece, {row, col});
            } else {
                movePiece(selectedPiece, {row, col});
            }

            if (checkWinCondition()) return;

            isPlayer1Turn = !isPlayer1Turn;
        }
        resetSelection();
    }
}

bool MainWindow::isValidMove(QPair<int, int> from, QPair<int, int> to) {
    int rowDiff = to.first - from.first;
    int colDiff = to.second - from.second;
    QString pieceText = boardSquares[from.first][from.second]->text();
    bool isKing = pieceText.startsWith("K");

    if (to.first < 0 || to.first >= 10 || to.second < 0 || to.second >= 10) return false;
    if (boardSquares[to.first][to.second]->text() != "") return false;

    if (abs(rowDiff) == 2 && abs(colDiff) == 2) {
        return canJump(from, to);
    }

    if (abs(rowDiff) != 1 || abs(colDiff) != 1) return false;

    if (isKing) return true;
    return (pieceText == "P1" && rowDiff == -1) || (pieceText == "P2" && rowDiff == 1);
}

bool MainWindow::canJump(QPair<int, int> from, QPair<int, int> to) {
    int jumpedRow = (from.first + to.first) / 2;
    int jumpedCol = (from.second + to.second) / 2;
    QString pieceText = boardSquares[from.first][from.second]->text();
    QString jumpedPiece = boardSquares[jumpedRow][jumpedCol]->text();
    bool isKing = pieceText.startsWith("K");

    if (jumpedPiece.isEmpty()) return false;

    if (pieceText.startsWith("P1") || pieceText.startsWith("K1")) {
        if (!(jumpedPiece.startsWith("P2") || jumpedPiece.startsWith("K2"))) return false;
        if (!isKing && (to.first - from.first) != -2) return false;
    } else {
        if (!(jumpedPiece.startsWith("P1") || jumpedPiece.startsWith("K1"))) return false;
        if (!isKing && (to.first - from.first) != 2) return false;
    }
    return true;
}

void MainWindow::performJump(QPair<int, int> from, QPair<int, int> to) {
    int jumpedRow = (from.first + to.first) / 2;
    int jumpedCol = (from.second + to.second) / 2;
    QString jumpedPiece = boardSquares[jumpedRow][jumpedCol]->text();

    if (!jumpedPiece.isEmpty()) {
        if (jumpedPiece.startsWith("P1") || jumpedPiece.startsWith("K1")) {
            for (int i = 0; i < player1Pieces.size(); ++i) {
                if (player1Pieces[i] == QPair<int, int>(jumpedRow, jumpedCol)) {
                    player1Pieces.removeAt(i);
                    break;
                }
            }
        } else {
            for (int i = 0; i < player2Pieces.size(); ++i) {
                if (player2Pieces[i] == QPair<int, int>(jumpedRow, jumpedCol)) {
                    player2Pieces.removeAt(i);
                    break;
                }
            }
        }
    }

    boardSquares[jumpedRow][jumpedCol]->setText("");
    boardSquares[jumpedRow][jumpedCol]->setStyleSheet("background-color: black;");
    movePiece(from, to);
}

void MainWindow::movePiece(QPair<int, int> from, QPair<int, int> to) {
    QString pieceText = boardSquares[from.first][from.second]->text();

    if (pieceText.startsWith("P1") || pieceText.startsWith("K1")) {
        for (int i = 0; i < player1Pieces.size(); ++i) {
            if (player1Pieces[i] == from) {
                player1Pieces[i] = to;
                break;
            }
        }
    } else {
        for (int i = 0; i < player2Pieces.size(); ++i) {
            if (player2Pieces[i] == from) {
                player2Pieces[i] = to;
                break;
            }
        }
    }

    boardSquares[from.first][from.second]->setText("");
    boardSquares[from.first][from.second]->setStyleSheet("background-color: black;");
    boardSquares[to.first][to.second]->setText(pieceText);
    boardSquares[to.first][to.second]->setStyleSheet(getPieceStyle(pieceText));
    promoteToKing(to.first, to.second);
}

void MainWindow::highlightLegalMoves(QPair<int, int> piece) {
    clearHighlightedSquares();
    int row = piece.first;
    int col = piece.second;
    QString pieceText = boardSquares[row][col]->text();

    QVector<QPair<int, int>> directions;
    if (pieceText.startsWith("P1") || pieceText.startsWith("K1")) {
        directions.append({-1, -1});
        directions.append({-1, 1});
        if (pieceText.startsWith("K1")) {
            directions.append({1, -1});
            directions.append({1, 1});
        }
    }
    if (pieceText.startsWith("P2") || pieceText.startsWith("K2")) {
        directions.append({1, -1});
        directions.append({1, 1});
        if (pieceText.startsWith("K2")) {
            directions.append({-1, -1});
            directions.append({-1, 1});
        }
    }

    for (const auto& dir : directions) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (isValidMove(piece, {newRow, newCol})) {
            highlightedSquares.append({newRow, newCol});
            boardSquares[newRow][newCol]->setStyleSheet("background-color: green;");
        }

        int jumpRow = row + 2 * dir.first;
        int jumpCol = col + 2 * dir.second;
        if (isValidMove(piece, {jumpRow, jumpCol})) {
            highlightedSquares.append({jumpRow, jumpCol});
            boardSquares[jumpRow][jumpCol]->setStyleSheet("background-color: pink;");
        }
    }
}

// void MainWindow::resetSelection() {
//     if (selectedPiece.first != -1) {
//         QString pieceText = boardSquares[selectedPiece.first][selectedPiece.second]->text();
//         boardSquares[selectedPiece.first][selectedPiece.second]->setStyleSheet(getPieceStyle(pieceText));
//     }
//     clearHighlightedSquares();
//     selectedPiece = {-1, -1};
// }

void MainWindow::resetSelection() {
    if (selectedPiece.first != -1) {
        int row = selectedPiece.first;
        int col = selectedPiece.second;
        QPushButton* square = boardSquares[row][col];
        QString pieceText = square->text();

        if (pieceText.isEmpty()) {
            // Reset to original board color
            if ((row + col) % 2 == 0) {
                square->setStyleSheet("background-color: white;");
            } else {
                square->setStyleSheet("background-color: black;");
            }
        } else {
            square->setStyleSheet(getPieceStyle(pieceText));
        }
    }
    clearHighlightedSquares();
    selectedPiece = {-1, -1};
}

void MainWindow::clearHighlightedSquares() {
    for (const auto& square : highlightedSquares) {
        QString pieceText = boardSquares[square.first][square.second]->text();
        boardSquares[square.first][square.second]->setStyleSheet(
            pieceText.isEmpty() ? "background-color: black;" : getPieceStyle(pieceText));
    }
    highlightedSquares.clear();
}

bool MainWindow::checkWinCondition() {
    if (player1Pieces.isEmpty()) {
        declareWin(2);
        return true;
    } else if (player2Pieces.isEmpty()) {
        declareWin(1);
        return true;
    }
    return false;
}

void MainWindow::declareWin(int player) {
    QMessageBox::information(this, "Game Over", QString("Player %1 Wins!").arg(player));
    resetGame();
}

void MainWindow::resetGame() {
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            QPushButton* square = boardSquares[row][col];
            square->setText("");
            if ((row + col) % 2 == 0) {
                square->setStyleSheet("background-color: white;");
            } else {
                square->setStyleSheet("background-color: black;");
            }
        }
    }

    player1Pieces.clear();
    player2Pieces.clear();
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            if ((row + col) % 2 == 1) {
                if (row < 4) {
                    boardSquares[row][col]->setText("P2");
                    player2Pieces.append({row, col});
                    boardSquares[row][col]->setStyleSheet(getPieceStyle("P2"));
                } else if (row > 5) {
                    boardSquares[row][col]->setText("P1");
                    player1Pieces.append({row, col});
                    boardSquares[row][col]->setStyleSheet(getPieceStyle("P1"));
                }
            }
        }
    }

    isPlayer1Turn = true;
    selectedPiece = {-1, -1};
}
