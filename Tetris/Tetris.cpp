#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>

using namespace std;

const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 20;
int speed = 500; // デフォルトの落下速度（ミリ秒）

vector<vector<int>> tetrominoes[7] = {
    {{1, 1, 1, 1}},         // I
    {{1, 1, 1}, {0, 1, 0}}, // T
    {{1, 1, 0}, {0, 1, 1}}, // Z
    {{0, 1, 1}, {1, 1, 0}}, // S
    {{1, 1}, {1, 1}},       // O
    {{1, 1, 1}, {1, 0, 0}}, // L
    {{1, 1, 1}, {0, 0, 1}}  // J
};

int field[FIELD_HEIGHT][FIELD_WIDTH] = { 0 };
int currentBlock, nextBlock;
int blockX, blockY;
int score = 0;
bool gameOver = false;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, coord);
}

vector<vector<int>> rotateBlock(vector<vector<int>>& block) {
    int h = block.size(), w = block[0].size();
    vector<vector<int>> rotated(w, vector<int>(h, 0));
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            rotated[j][h - 1 - i] = block[i][j];
    return rotated;
}

bool isCollision(int x, int y, vector<vector<int>>& block) {
    for (int i = 0; i < block.size(); i++)
        for (int j = 0; j < block[i].size(); j++)
            if (block[i][j] && (x + j < 0 || x + j >= FIELD_WIDTH || y + i >= FIELD_HEIGHT || field[y + i][x + j]))
                return true;
    return false;
}

void placeBlock() {
    vector<vector<int>>& block = tetrominoes[currentBlock];
    for (int i = 0; i < block.size(); i++)
        for (int j = 0; j < block[i].size(); j++)
            if (block[i][j]) field[blockY + i][blockX + j] = 1;

    int linesCleared = 0;
    for (int i = 0; i < FIELD_HEIGHT; i++) {
        bool full = true;
        for (int j = 0; j < FIELD_WIDTH; j++)
            if (!field[i][j]) full = false;

        if (full) {
            linesCleared++;
            for (int k = i; k > 0; k--)
                for (int j = 0; j < FIELD_WIDTH; j++)
                    field[k][j] = field[k - 1][j];
        }
    }
    score += linesCleared * 100;

    currentBlock = nextBlock;
    nextBlock = rand() % 7;
    blockX = FIELD_WIDTH / 2 - 1;
    blockY = 0;

    if (isCollision(blockX, blockY, tetrominoes[currentBlock])) gameOver = true;
}

void drawField() {
    gotoxy(0, 0);

    // 上枠
    cout << "+";
    for (int j = 0; j < FIELD_WIDTH; j++) cout << "-";
    cout << "+" << endl;

    for (int i = 0; i < FIELD_HEIGHT; i++) {
        cout << "|";
        for (int j = 0; j < FIELD_WIDTH; j++) {
            if (field[i][j]) cout << "#";
            else if (i >= blockY && i < blockY + tetrominoes[currentBlock].size() &&
                j >= blockX && j < blockX + tetrominoes[currentBlock][0].size() &&
                tetrominoes[currentBlock][i - blockY][j - blockX])
                cout << "#";
            else cout << " ";
        }
        cout << "|" << endl;
    }

    // 下枠
    cout << "+";
    for (int j = 0; j < FIELD_WIDTH; j++) cout << "-";
    cout << "+" << endl;

    cout << "Score: " << score << endl;
    cout << "Next Block:" << endl;

    for (const auto& row : tetrominoes[nextBlock]) {
        for (int cell : row) cout << (cell ? "#" : " ");
        cout << endl;
    }

    if (gameOver) {
        cout << "GAME OVER! Press 'c' to Continue, 'q' to Quit." << endl;
        while (true) {
            char key = _getch();
            if (key == 'c') {
                memset(field, 0, sizeof(field));
                score = 0;
                gameOver = false;
                return;
            }
            if (key == 'q') exit(0);
        }
    }
}

void gameLoop() {
    while (true) {
        if (_kbhit()) {
            char key = _getch();
            int newX = blockX, newY = blockY;
            vector<vector<int>> newBlock = tetrominoes[currentBlock];

            if (key == 'a') newX--;
            if (key == 'd') newX++;
            if (key == 's') newY++;
            if (key == 'w') while (!isCollision(newX, newY + 1, newBlock)) newY++;
            if (key == 'r') newBlock = rotateBlock(newBlock);

            if (!isCollision(newX, newY, newBlock)) {
                blockX = newX;
                blockY = newY;
                tetrominoes[currentBlock] = newBlock;
            }
        }

        if (!isCollision(blockX, blockY + 1, tetrominoes[currentBlock])) blockY++;
        else placeBlock();

        drawField();
        Sleep(speed);
    }
}

void showTitleScreen() {
    system("cls");
    cout << "=====================\n";
    cout << "   SIMPLE TETRIS\n";
    cout << "=====================\n";
    cout << "Select Difficulty:\n";
    cout << "1. Easy\n";
    cout << "2. Normal\n";
    cout << "3. Hard\n";
    cout << "Choice: ";

    char choice;
    while (true) {
        choice = _getch();
        if (choice == '1') { speed = 700; break; }
        if (choice == '2') { speed = 500; break; }
        if (choice == '3') { speed = 300; break; }
    }
}

int main() {
    srand(time(0));
    showTitleScreen();
    currentBlock = rand() % 7;
    nextBlock = rand() % 7;
    gameLoop();
    return 0;
}
