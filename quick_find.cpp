#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "pair.h"

int main() {

    auto input = R"(
3 4
4 9
8 0
2 3
5 6
2 9
5 9
7 3
4 8
5 6
0 2
6 1
q)";
    std::stringstream ss(input);

    Forward_list<Pair<int, int>> pairs;
    int size = 0;

    char cf, cs = '\0';
    do {
        ss >> cf;
        if (cs != '\0') {
            int f = cf - '0';
            int s = cs - '0';
            pairs.emplace_back(f, s);
            ++size;
            cs = '\0';
        } else
            cs = cf;
    } while (cf != 'q');

    std::cout << "connections to add:" << std::endl << pairs << std::endl;

    Array<int> connections(size);
    auto init_connections = [&connections]() {
        for (size_t i = 0; i < connections.size(); ++i)
            connections[i] = i;
    };

    std::cout << "quick find:" << std::endl;
    init_connections();
    std::cout << connections << std::endl;
    for (auto pair = pairs.cbegin(); pair != pairs.cend(); ++pair) {
        auto f = pair->first_;
        auto s = pair->second_;
        Forward_list<int> l;
        auto c = connections[f];
        if (c == connections[s]) {
            for (size_t i = 0; i < connections.size(); ++i)
                if (connections[i] == c) 
                    l.push_back(i);
        } else 
            for (auto& connection : connections)
                if (connection == c)
                    connection = connections[s];
        std::cout << connections << "  " << f << "-" << s;
        if (!l.empty()) {
            std::cout << "  ";
            for (auto it = l.cbegin(); it != l.cend(); ++it) {
                if (it != l.cbegin())
                    std::cout << "-";
                std::cout << (*it);
            }

        }
        std::cout << std::endl;
    }

    std::cout << "quick union:" << std::endl;
    init_connections();
    std::cout << connections << std::endl;
    for (auto pair = pairs.cbegin(); pair != pairs.cend(); ++pair) {
        auto f = pair->first_;
        auto s = pair->second_;
        int i, j;
        Forward_list<int> l;
        l.push_back(f);
        for (i = f; i != connections[i]; i = connections[i])
            l.push_back(connections[i]);
        l.push_back(s);
        for (j = s; j != connections[j]; j = connections[j])
            l.push_back(connections[j]);

        bool already_connected = i == j;
        if (!already_connected)
            connections[i] = j;
        std::cout << connections << "  " << f << "-" << s;
        if (already_connected) // todo remove last item from list
            std::cout << "  " << l;
        std::cout << std::endl;
    }

}
