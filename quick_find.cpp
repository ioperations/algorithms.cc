#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "pair.h"

template<typename F>
void search_connections(const Forward_list<Pair<int, int>>& pairs, Array<int>& connections, F connections_searcher) {
    for (size_t i = 0; i < connections.size(); ++i)
        connections[i] = i;
    std::cout << connections << std::endl;
    for (auto pair = pairs.cbegin(); pair != pairs.cend(); ++pair) {
        auto already_existing = connections_searcher(pair->first_, pair->second_, connections);
        std::cout << connections << "  " << pair->first_ << "-" << pair->second_;
        if (!already_existing.empty()) {
            std::cout << "  ";
            for (auto it = already_existing.cbegin(); it != already_existing.cend(); ++it) {
                if (it != already_existing.cbegin())
                    std::cout << "-";
                std::cout << (*it);
            }

        }
        std::cout << std::endl;
    }
}

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

    std::cout << "quick find:" << std::endl;
    search_connections(pairs, connections, [](int f, int s, Array<int>& connections) {
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
        return l;
    });

    std::cout << "quick union:" << std::endl;
    search_connections(pairs, connections, [](int f, int s, Array<int>& connections) {
        Forward_list<int> l;
        l.push_back(f);
        int i, j;
        for (i = f; i != connections[i]; i = connections[i])
            l.push_back(connections[i]);
        l.push_back(s);
        for (j = s; j != connections[j]; j = connections[j])
            l.push_back(connections[j]);

        if (i != j) {
            connections[i] = j;
            l.clear();
        } // todo else remove last item from list, double linked list required
        return l;
    });

}
