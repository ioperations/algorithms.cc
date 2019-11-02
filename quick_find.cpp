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
    for (int i = 0; i < connections.size(); ++i)
        connections[i] = i;

    for (auto& pair : pairs) {
        auto f = pair.first_;
        auto s = pair.second_;
        auto t = connections[f];
        bool alread_connected = true;
        Forward_list<int> l;
        if (t == connections[s]) {
            for (int i = 0; i < connections.size(); ++i)
                if (connections[i] == t) 
                    l.push_back(i);
        } else {
            alread_connected = false;
            for (auto& connection : connections)
                if (connection == t)
                    connection = connections[s];
        }
        std::cout << connections << "  " << f << "-" << s;
        if (alread_connected) {
            std::cout << "  ";
            for (auto it = l.cbegin(); it != l.cend(); ++it) {
                if (it != l.cbegin())
                    std::cout << "-";
                std::cout << (*it);
            }

        }
        std::cout << std::endl;
    }

}
