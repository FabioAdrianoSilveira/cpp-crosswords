#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <utility>
#include <random>
#include <chrono>
#include <mysql/mysql.h>

using namespace std;

// Struct com variáveis que serão usadas para conexão com banco
struct ConnectionVariables
{
    // As variáveis são instânciadas como ponteiros de char para não trabalhar com string, visto que a biliboteca mysql aceita apenas char como input
    const char *SERVER, *USER, *PASSWORD, *DATABASE;
};

// Esse struct representa uma célula individual no tabuleiro
struct BoardCell
{
    char letter;  // Representa uma letra na célula
    bool filled;  // true se uma letra já preencheu esta célula ou false se a célula está vazia
    bool blocked; // true se a célula for bloqueada e false se for liberada

    // Método construtor do struct com variáveis inicializadas
    BoardCell() : letter(' '), filled(false), blocked(false) {}
};

// Esse struct receberá uma palavra carregada do banco de dados
struct Word
{
    int id;
    string text; // Armazena a palavra
    string tip;  // Armazena a dica
    // Posição no tabuleiro, uma vez que a palavra é colocada
    int line;
    int column;
    char direction; // 'H' para palavras em horizontal, 'V' para vertical
    bool placed;    // Para controlar se a palavra já foi usada neste tabuleiro

    // Método construtor padrão do struct com variáveis inicializadas
    Word() : id(0), text(""), tip(""), line(-1), column(-1), direction(' '), placed(false) {}

    // Método construtor parametrizado, será utilizado para armazenar as palavras vindas do banco de dados
    Word(int i, const string &txt, const string &tips)
        : id(i), text(txt), tip(tips), line(-1), column(-1), direction(' '), placed(false) {}
};

// Representa um slot pré-definido no tabuleiro onde uma palavra pode ser colocada
struct CrosswordSlot
{
    int internalId; // ID interno para o slot
    // Coordenadas para posição inicial da palavra
    int initialLine;
    int initialColumn;
    char slotDirection; // 'H' para horizontal ou 'V' para vertical
    int lenght;         // Comprimento esperado da palavra para este slot

    // Método construtor parametrizado para facilitar na instância do struct
    CrosswordSlot(int i_int, int r, int c, char d, int len)
        : internalId(i_int), initialLine(r), initialColumn(c), slotDirection(d), lenght(len) {}
};

// Classe que representa o tabuleiro de palavras cruzadas
class Board
{
public:
    // Vetor bidimensional, a primeira camada representa representa uma linha e a segunda cada uma das células dessa linha
    vector<vector<BoardCell>> grid;
    int lines;   // Número de linhas do tabuleiro
    int columns; // Número de colunas no tabuleiro

    // Método construtor do tabuleiro
    Board(int numLines, int numColumns) : lines(numLines), columns(numColumns)
    {
        // Atribui ao vetor os valores de lines e columns, no caso, 8x8
        grid.resize(lines, vector<BoardCell>(columns));
    }

    // Inicializa o layout do tabuleiro com células bloqueadas
    void initializeBoard()
    {
        if (lines != 8 || columns != 8)
        {
            cout << "Erro: Era esperado um tabuleiro de 8x8." << endl; // Interação com terminal para apontar um erro
            return;
        }

        // Marca todas as células como liberadas inicialmente
        for (int i = 0; i < lines; ++i)
        {
            for (int j = 0; j < columns; ++j)
            {
                grid[i][j].blocked = false;
            }
        }

        // Marca as células como bloqueadas

        // Linha 0: todas bloqueadas
        for (int j = 0; j < columns; ++j)
            grid[0][j].blocked = true;

        // Linha 1
        grid[1][0].blocked = true;
        grid[1][1].blocked = true;
        grid[1][3].blocked = true;
        grid[1][4].blocked = true;
        grid[1][5].blocked = true;
        grid[1][6].blocked = true;
        grid[1][7].blocked = true;

        // Linha 2
        grid[2][0].blocked = true;

        // Linha 3
        grid[3][0].blocked = true;
        grid[3][1].blocked = true;
        grid[3][3].blocked = true;
        grid[3][4].blocked = true;
        grid[3][5].blocked = true;
        grid[3][6].blocked = true;
        grid[3][7].blocked = true;

        // Linha 4
        grid[4][0].blocked = true;
        grid[4][1].blocked = true;
        grid[4][3].blocked = true;
        grid[4][4].blocked = true;
        grid[4][5].blocked = true;
        grid[4][6].blocked = true;
        grid[4][7].blocked = true;

        // Linha 5
        grid[5][7].blocked = true;

        // Linha 6
        grid[6][0].blocked = true;
        grid[6][1].blocked = true;
        grid[6][3].blocked = true;
        grid[6][4].blocked = true;
        grid[6][5].blocked = true;
        grid[6][6].blocked = true;
        grid[6][7].blocked = true;

        // Linha 7
        grid[7][0].blocked = true;
        grid[7][1].blocked = true;
    }

    // Essa função é exclusiva para testes, mostra o estado da formação do quadro
    void Print() const
    {
        for (int i = 0; i < lines; ++i)
        {
            for (int j = 0; j < columns; ++j)
            {
                if (grid[i][j].blocked)
                {
                    cout << "█ "; // Caractere para célula bloqueada
                }
                else if (grid[i][j].filled)
                {
                    cout << grid[i][j].letter << " ";
                }
                else
                {
                    cout << "_ "; // Vazio, célula liberada
                }
            }
            cout << endl;
        }
    }
};

// --- Funções de Conexão com MySQL ---

MYSQL *ConnectionSetup(ConnectionVariables mysqlVariables)
{
    // Inicia a conexão com a API para manipulação de MySQL (MySQL C API)
    MYSQL *connection = mysql_init(NULL); // mysql_init é iniciada como NULL para dar sinal a API pedindo o retor de um objeto do tipo SQL

    /*
        A função abaixo estabele conexão com o servidor SQL (banco de dados)
        -- Argumentos da função --
        Objeto SQL
        Servidor
        Usuário
        Senha
        Banco de dados
        Porta de conexão (0 para default (3306))
        Tipo de conexão (NULL para TCP/IP)
        Marcação de client específico (0 para nenhum)
    */
    if (!mysql_real_connect(connection, mysqlVariables.SERVER, mysqlVariables.USER, mysqlVariables.PASSWORD, mysqlVariables.DATABASE, 0, NULL, 0))
    {
        cout << "Connection Error: " << mysql_error(connection) << endl; // Interação com terminal para apontar um erro
        exit(1);
    }
    return connection;
}

// Função para execução de query
// Argumentos: Objeto do tipo SQL e uma query válida
MYSQL_RES *executeQuery(MYSQL *connection, const char *query)
{
    // Faz conexão com a MySQL C API, se estiver tudo ok com o onjeto e a query, retorna 0
    if (mysql_query(connection, query))
    {
        cout << "MySQL Query Error: " << mysql_error(connection) << endl; // Interação com terminal para apontar um erro
        exit(1);
    }
    // Se estiver tudo ok, mysql_use_query faz conexão com a MySQL C API, faz uso da query e retorna os resultados obtidos
    return mysql_use_result(connection);
}

// -- Fim das funções de conexão com SQL --

// Verifica se uma palavra pode ser colocada em um slot específico
bool canInsertIntoSlot(const Board &board, const Word &word, const CrosswordSlot &slot)
{
    // Verifica se o comprimento da palavra corresponde ao comprimento do slot
    if (word.text.length() != slot.lenght)
    {
        return false;
    }

    // Verifica limites do tabuleiro, células bloqueadas e colisões
    for (int i = 0; i < slot.lenght; ++i)
    {
        int r = slot.initialLine;
        int c = slot.initialColumn;

        if (slot.slotDirection == 'H')
        {
            c += i;
        }
        else
        { // 'V'
            r += i;
        }

        // Verifica limites da grade (redundante se slots forem bem definidos, mas é uma boa prática)
        if (r < 0 || r >= board.lines || c < 0 || c >= board.columns)
        {
            return false;
        }

        // Verifica se a célula está bloqueada
        if (board.grid[r][c].blocked)
        {
            return false; // Não pode colocar palavra em célula bloqueada
        }

        // Verifica colisões com letras já existentes
        if (board.grid[r][c].filled)
        {
            // Se já está ocupada, a letra deve ser a mesma (interseção válida)
            if (board.grid[r][c].letter != toupper(word.text[i]))
            {
                return false; // Conflito de letras
            }
        }
    }

    // Verifica células adjacentes para garantir que a palavra não se estenda além do slot
    // Para palavras horizontais:
    if (slot.slotDirection == 'H')
    {
        // Célula imediatamente à esquerda (se existir e não for bloqueada):
        if (slot.initialColumn > 0 && !board.grid[slot.initialLine][slot.initialColumn - 1].blocked)
        {
            if (board.grid[slot.initialLine][slot.initialColumn - 1].filled)
            {
                return false; // Evita que a palavra horizontal "cole" em outra horizontal à esquerda
            }
        }
        // Célula imediatamente à direita (se existir e não for bloqueada):
        if ((slot.initialColumn + slot.lenght) < board.columns &&
            !board.grid[slot.initialLine][slot.initialColumn + slot.lenght].blocked)
        {
            if (board.grid[slot.initialLine][slot.initialColumn + slot.lenght].filled)
            {
                return false; // Evita que a palavra horizontal "cole" em outra horizontal à direita
            }
        }
    }
    // Para palavras verticais:
    else
    {
        // Célula imediatamente acima (se existir e não for bloqueada):
        if (slot.initialLine > 0 && !board.grid[slot.initialLine - 1][slot.initialColumn].blocked)
        {
            if (board.grid[slot.initialLine - 1][slot.initialColumn].filled)
            {
                return false; // Evita que a palavra vertical "cole" em outra vertical acima
            }
        }
        // Célula imediatamente abaixo (se existir e não for bloqueada):
        if ((slot.initialLine + slot.lenght) < board.lines &&
            !board.grid[slot.initialLine + slot.lenght][slot.initialColumn].blocked)
        {
            if (board.grid[slot.initialLine + slot.lenght][slot.initialColumn].filled)
            {
                return false; // Evita que a palavra vertical "cole" em outra vertical abaixo
            }
        }
    }

    return true; // Se todas as verificações passaram, a palavra pode ser colocada
}

// Coloca uma palavra em um slot específico no tabuleiro
void insertIntoSlot(Board &board, Word &word, const CrosswordSlot &slot)
{
    word.line = slot.initialLine;
    word.column = slot.initialColumn;
    word.direction = slot.slotDirection;
    word.placed = true; // Marca a palavra como colocada para não ser usada novamente

    for (int i = 0; i < slot.lenght; ++i)
    {
        int r = slot.initialLine;
        int c = slot.initialColumn;

        if (slot.slotDirection == 'H')
        {
            c += i;
        }
        else
        {
            r += i;
        }
        board.grid[r][c].letter = toupper(word.text[i]);
        board.grid[r][c].filled = true;
    }
}

/*
    A função abaixo determina a ordem em que o tabuleiro deve ser preenchido, comparando os slots de palavra
    A ordem de preenchimento será: palavra vertical, palavras horizontais com 7 caracteres e palavra horizontal com 6 caracteres
*/
bool compareSlots(const CrosswordSlot &a, const CrosswordSlot &b)
{
    if (a.lenght != b.lenght)
    {
        return a.lenght > b.lenght; // Slots maiores tem prioridade
    }
    if (a.initialLine != b.initialLine)
    {
        return a.initialLine < b.initialLine; // Slots mais acima tem prioridade
    }
    if (a.initialColumn != b.initialColumn)
    {
        return a.initialColumn < b.initialColumn; // Slots mais a esquerda têm prioridade
    }
    return a.slotDirection < b.slotDirection; // Slots horizontais têm prioridade (determinado de acordo com a tabela ASCII)
}

int main()
{
    /*
        -- Geração de números aleatórios --
        mt19937 é uma função de geração de números pseudo-aleatórios
        rng é um objeto de mt19937 que faz a geração de números
        chrono::steady_clock::now().time_since_epoch().count() é a seed para geração de números aleatórios
        -- Seed --
        chrono::steady_clock é um tipo de relógio da biblioteca chrono
        now() é uma função que retorna o ponto exato atual de tempo do relógio
        time_since_epoch() retorna a diferença de horário entre o momento que a função foi chamada para o ponto atual de tempo
        count() faz a extração da unidade de tempo exata como um inteiro
    */
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    // Configuração do Banco de Dados e Conexão
    MYSQL *connect;
    MYSQL_RES *results;
    MYSQL_ROW row;

    ConnectionVariables mysqlDatabase;
    mysqlDatabase.SERVER = ""; // Para rodar localmente, o valor de SERVER deve ser "localhost" | Para rodar em nuvem, o valor de SERVER deve ser "switchyard.proxy.rlwy.net"
    mysqlDatabase.USER = ""; // Para rodar localmente, o valor de USER deve ser "root" (ou seu usuário de preferência) | Para rodar em nuvem, o valor de USER deve ser "root"
    mysqlDatabase.PASSWORD = ""; // Para rodar localmente, o valor de PASSWORD deve ser {sua_senha} | Para rodar em nuvem, o valor de PASSWORD deve ser "XnvkSWZdYVocercOkmtfeAeXcPXtzdGV"
    mysqlDatabase.DATABASE = ""; // Para rodar localmente, o valor de DATABASE deve ser "palavras_cruzadas" | Para rodar em nuvem, o valor de DATABASE deve ser "railway"

    connect = ConnectionSetup(mysqlDatabase);
    results = executeQuery(connect, "SELECT id, palavra, dica FROM palavras_cruzadas");

    // Vetor para armazenar as palavras vindas do banco de dados
    vector<Word> allWords;
    cout << "Carregando palavras do banco de dados..." << endl; // Interação com terminal para indicar início de concexão com banco

    // Faz o comando de SELECT no banco de dados de ids, palavras e dicas até que o retorno seja NULL (não hajam mais registros no banco)
    while ((row = mysql_fetch_row(results)) != NULL)
    {
        int id = stoi(row[0]); // stoi -> String TO Int
        string text = row[1];
        string tip = row[2];
        allWords.emplace_back(id, text, tip); // Adiciona os registros obtidos no SELECT na posição após a última registrada
    }

    mysql_free_result(results); // Limpa a memória das requisições feitas a MySQL C API para evitar vazamentos e bricks no sistema
    mysql_close(connect);       // Fecha a conexão com o banco de dados

    cout << "Palavras carregadas: " << allWords.size() << endl; // Interação com terminal para indicar conexão bem sucedida

    // Preparação das listas de palavras para randomização
    vector<Word> verticalWords;
    vector<Word> horizontalWords;

    for (const auto &word : allWords) // auto é uma tipagem dinâmica de variável
    {
        // Palavras com id de 1 a 5 são exclusivas da linha vertical
        if (word.id >= 1 && word.id <= 5)
        {
            verticalWords.push_back(word);
        }
        else
        {
            horizontalWords.push_back(word);
        }
    }

    /*
        As funções abaixo tem a função de embaralhar a lista de palavras
        shuffle é uma função que embaralha elementos dado um determinado range, ela é proveniente da biblioteca algorithm
        begin() é responsǘale por determinar o início do range
        end() é responsável por determinar o fim do range
        rng é um objeto instânciado de mt19937 que gerará a nova ordem de acordo com os ranges definidos
    */
    shuffle(verticalWords.begin(), verticalWords.end(), rng);
    shuffle(horizontalWords.begin(), horizontalWords.end(), rng);

    cout << "Listas de palavras embaralhadas para randomização." << endl; // Interação com terminal para indicar que as palavras foram randomizadas

    // Definição do tabuleiro
    vector<CrosswordSlot> boardSlots;
    int slotId = 1; // ID internos dos slots

    // Slots Horizontais (IDs internos: 1-7)
    // Argumentos: (ID do slot, Linha inicial, Coluna incial, Direção, Tamanho esperado da palavra na linha)
    boardSlots.emplace_back(slotId++, 1, 2, 'H', 1);
    boardSlots.emplace_back(slotId++, 2, 1, 'H', 7);
    boardSlots.emplace_back(slotId++, 3, 2, 'H', 1);
    boardSlots.emplace_back(slotId++, 4, 2, 'H', 1);
    boardSlots.emplace_back(slotId++, 5, 0, 'H', 7);
    boardSlots.emplace_back(slotId++, 6, 2, 'H', 1);
    boardSlots.emplace_back(slotId++, 7, 2, 'H', 6);

    // Slots Verticais (IDs internos: 8-16)
    boardSlots.emplace_back(slotId++, 2, 1, 'V', 3);
    boardSlots.emplace_back(slotId++, 1, 2, 'V', 7);
    boardSlots.emplace_back(slotId++, 2, 3, 'V', 4);
    boardSlots.emplace_back(slotId++, 2, 4, 'V', 4);
    boardSlots.emplace_back(slotId++, 2, 5, 'V', 4);
    boardSlots.emplace_back(slotId++, 2, 6, 'V', 4);
    boardSlots.emplace_back(slotId++, 2, 7, 'V', 3);
    boardSlots.emplace_back(slotId++, 5, 0, 'V', 1);
    boardSlots.emplace_back(slotId++, 7, 2, 'V', 1);

    cout << "Slots do tabuleiro definidos: " << boardSlots.size() << endl; // Interação com terminal para indicar slots vagos para inserção de caracteres

    /*
        -- A função abaixo ordena os slots para preencher os mais "importantes" primeiro --
        Sort é uma função da biblioteca algorithm que ordena elementos em ordem crescente de acordo com um range fornecido
        begin() indica o começo do range, end() indica o fim do range e compareSlots será o método usado para definir a prioridade
    */
    sort(boardSlots.begin(), boardSlots.end(), compareSlots);

    // Inicialização do Tabuleiro
    Board gameBoard(8, 8);
    gameBoard.initializeBoard();

    // Interação com terminal para indicar que o quadro foi gerado e mostrar sua estrutura
    cout << "\nTabuleiro Vazio (com layout):\n";
    gameBoard.Print();

    /*
        A estrutura abaixo (map) é uma estrutura proveniente da biblioteca map que armazena elementos formados por uma combinação de chave(um id) e um valor guardado
        -- Explicação da estrutura --
        O elementos são guardados em ordem crescente de acordo com sua chave
        pair<int, char> representa o valor chave usado para mapear os elementos
        pair é uma estrutura da biblioteca utility que permite combinar dois valores heterogêneos em um objeto
        Sintaxe: map<chave, valorMapeado>
    */
    map<pair<int, char>, Word> placedWords; // Chave: {ID Interno do Slot, Direção}

    // Preenchimento do tabuleiro
    for (const auto &slot : boardSlots)
    {
        bool filledSlot = false;

        // Define quais palavras usar com base na direção do slot
        vector<Word> *wordsToBeUsed;
        if (slot.slotDirection == 'V')
        {
            wordsToBeUsed = &verticalWords;
        }
        else
        {
            wordsToBeUsed = &horizontalWords;
        }

        // Percorre as palavras disponíveis
        for (auto &words : *wordsToBeUsed)
        {
            // Verifica se a palavra já foi usada
            if (!words.placed && canInsertIntoSlot(gameBoard, words, slot))
            {
                insertIntoSlot(gameBoard, words, slot);
                placedWords[{slot.internalId, slot.slotDirection}] = words;
                filledSlot = true;
                break; // Slot preenchido, vá para o próximo slot
            }
        }
    }

    // Exibição do quadro preenchido, usado para testes e visualização via terminal apenas
    cout << "\nTabuleiro Preenchido:\n";
    gameBoard.Print();

    // Exibição das palavras usadas e dicas associadas a essas palavras
    cout << "\n--- Palavras e Dicas ---" << endl;
    for (const auto &slot : boardSlots)
    {
        /*
            placedWords é o resultado da estrutura map<pair<int, char>, Word>
            O método find() busca dentro do elemento placedWords palavras colocadas nos slots
            Se nada for encontrado, o método retornará end(), um equivalemente para NULL nesse caso, se uma palavra for encontrada, um ponteiro para ela será retornado
        */
        auto it = placedWords.find({slot.internalId, slot.slotDirection});
        if (it != placedWords.end())
        {
            /*
                No contexto de objetos do tipo map, trabalhamos com "->" ao invés de "." pr estarmos acessando o ponteiro de um objeto
                No contexto de objetos do tipo map, variable->first te da acesso ao id do elemento, já variable->second te da acesso ao elemento mapeado
            */
            const Word &word = it->second;
            cout << word.tip << " (" << word.text << ")" << endl;
        }
    }

    return 0;
}