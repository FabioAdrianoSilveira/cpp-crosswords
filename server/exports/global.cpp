#include <iostream>
#include <string>
#include <fstream>
#include "global.h"

using namespace std;

gameStruct loadStruct(const string &boardFile, const string &tipsFile)
{
    gameStruct result;
    ifstream boardF(boardFile);

    for (int i = 0; i < 8; ++i)
    {
        string line;
        getline(boardF, line);

        for (int j = 0; j < 8; ++j)
        {
            if (j < (int)line.size())
                result.board[i][j] = line[j];
            else
                result.board[i][j] = ' ';
        }
    }
    boardF.close();

    string tips[4];
    ifstream tipsF(tipsFile);
    for (int i = 0; i < 4; ++i)
    {
        getline(tipsF, tips[i]);
    }
    tipsF.close();

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            char cell = result.board[i][j];
            if (cell == '#')
            {
                result.tips[i][j] = "#";
            }
            else
            {
                if (i == 2 && j >= 0 && j <= 7)
                    result.tips[i][j] = tips[0];
                else if (i == 5 && j >= 0 && j <= 6)
                    result.tips[i][j] = tips[1];
                else if (i == 7 && j >= 2 && j <= 7)
                    result.tips[i][j] = tips[2];
                else if ((i >= 1 && i <= 7) && j == 2)
                    result.tips[i][j] = tips[3];
                else
                    result.tips[i][j] = "";
            }
        }
    }

    return result;
}

// Para acessar o tabuleiro manipule result.board[i][j], acessar as dicas, manipule result.tips[i][j]