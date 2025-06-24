#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>

using namespace std;

struct gameStruct
{
    char board[8][8];
    string tips[8][8];
};

gameStruct loadStruct(const string &boardFile, const string &tipsFile);

#endif