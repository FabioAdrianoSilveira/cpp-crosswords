#include <iostream>

struct answer_struct
{
    char c[8][8];
    std::string hint[8][8];
};

const answer_struct ANSWER_SHEET =
{
    .c = {
        { '0', '0', '0', '0', '0', '0', '0', '0' },
        { '0', '0', 's', '0', '0', '0', '0', '0' },
        { '0', 'a', 'u', 'd', 'a', 'c', 'i', 'a' },
        { '0', '0', 'b', '0', '0', '0', '0', '0' },
        { '0', '0', 'l', '0', '0', '0', '0', '0' },
        { 'b', 'r', 'i', 'n', 'c', 'a', 'r', '0' },
        { '0', '0', 'm', '0', '0', '0', '0', '0' },
        { '0', '0', 'e', 'n', 't', 'r', 'a', 'r' }
    },

    .hint = {
        { "0", "0", "0", "0", "0", "0", "0", "0" },
        { "0", "0", "De notável valor (adj.)", "0", "0", "0", "0", "0" },
        { "0", "Inovação insolente (sub.)", "AVISO: Selecione uma célula que não esteja em uma interseção", "Inovação insolente (sub.)", "Inovação insolente (sub.)", "Inovação insolente (sub.)", "Inovação insolente (sub.)", "Inovação insolente (sub.)" },
        { "0", "0", "De notável valor (adj.)", "0", "0", "0", "0", "0" },
        { "0", "0", "De notável valor (adj.)", "0", "0", "0", "0", "0" },
        { "Entreter-se de maneira lúdica (verb. inf.)", "Entreter-se de maneira lúdica (verb. inf.)", "0", "Entreter-se de maneira lúdica (verb. inf.)", "Entreter-se de maneira lúdica (verb. inf.)", "Entreter-se de maneira lúdica (verb. inf.)", "Entreter-se de maneira lúdica (verb. inf.)", "0" },
        { "0", "0", "De notável valor (adj.)", "0", "0", "0", "0", "0" },
        { "0", "0", "e", "Deslocar-se para o outro lado (verb. inf.)", "Deslocar-se para o outro lado (verb. inf.)", "Deslocar-se para o outro lado (verb. inf.)", "Deslocar-se para o outro lado (verb. inf.)", "Deslocar-se para o outro lado (verb. inf.)" }
    }
};