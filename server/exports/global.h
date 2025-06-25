// Verifica se GLOBAL_H já foi definido para evitar múltiplas inclusões do mesmo arquivo
#ifndef GLOBAL_H
// Define GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// Estrutura que receberá o quadro e as dicas para montar o jogo
struct gameStruct
{
    char board[8][8];
    string tips[8][8];
};

// Função responsável por carregar o struct com o tabuleiro e as dicas vindas dos arquivos
gameStruct loadStruct(const string &boardFile, const string &tipsFile)
{
    gameStruct result;

    // Abre o arquivo para leitura do tabuleiro
    ifstream boardF(boardFile);

    for (int i = 0; i < 8; ++i)
    {
        // Variável para armazenar linha do tabuleiro
        string line;
        // Faz leitura de uma linha do tabuleiro e passa a mesma para a variável line
        getline(boardF, line);

        for (int j = 0; j < 8; ++j)
        {
            if (j < (int)line.size()) // Se a linha possui caracter na posição j
                result.board[i][j] = line[j]; // Armazena o caracter dentro da estrutura
            // Caso contrário, preencha a posição do struct com um espaço em branco
            else
                result.board[i][j] = ' ';
        }
    }
    // Fecha o arquivo
    boardF.close();

    // Array para armezenar dicas
    string tips[4];
    // Abre o arquivo para leitura das dicas
    ifstream tipsF(tipsFile);
    // Lê o arquivo linha a linha, armazenando cada uma delas em um espaço do array tips
    for (int i = 0; i < 4; ++i)
    {
        getline(tipsF, tips[i]);
    }
    // Fecha o arquivo
    tipsF.close();

    // Cria uma cópia de board e troca as letras pela dica da palavra a qual ela pertence
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            char cell = result.board[i][j];
            if (cell == '0')
            {
                result.tips[i][j] = '0';
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

// Fim da verificação do ifndef GLOBAL_H
#endif