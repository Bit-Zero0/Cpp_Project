#include "searcher.hpp"
#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>

const std::string input = "data/raw_html/raw.txt";

int main()
{
    NS_search::Searcher *search = new NS_search::Searcher();
    search->InitSearcher(input);

    std::string query;
    std::string json_string;

    char buffer[1024];
    while (true)
    {
        std::cout << "Please Enter You Search Query#";
        fgets(buffer, sizeof(buffer) - 1, stdin);

        buffer[strlen(buffer) - 1] = 0;
        query = buffer;
        search->Search(query, &json_string);
        std::cout << json_string << std::endl;
    }
    return 0;
}