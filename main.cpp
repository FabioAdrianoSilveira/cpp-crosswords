#include <iostream>
#include <filesystem>

using namespace std;

void GenerateBoard()
{
    filesystem::current_path("./server/");
    system("g++ main.cpp -o board `mysql_config --cflags --libs` && ./board");
}

void LaunchInterface()
{
    filesystem::current_path("../GUI/");

    if (!filesystem::is_directory("build"))
    {
        filesystem::create_directory("build");
    }
    filesystem::current_path("build");
    system("cmake ..");
    system("make");
    system("./bin/cruzadinha-pp");
}

int main()
{
    GenerateBoard();
    LaunchInterface();

    return 0;
}