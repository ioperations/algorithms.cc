#pragma once

#include <iostream>
#include <vector>

template<typename T>
class Adjacency_matrix {
    public:
        class Vertex {
            private:
                friend class Adjacency_matrix;
                Adjacency_matrix& adjacency_matrix_;
                size_t index_;
                Vertex(Adjacency_matrix& adjacency_matrix, size_t index) 
                    :adjacency_matrix_(adjacency_matrix), index_(index) 
                {}
                T value() const {
                    return adjacency_matrix_.vertices_[index_];
                }
            public:
                friend std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
                    return stream << v.value(); 
                }
        };
    private:
        friend class Vertex;
        std::vector<T> vertices_;
        std::vector<std::vector<bool>> edges_;
    public:
        Adjacency_matrix() :edges_(100) {
            for (auto& l : edges_)
                l = std::vector<bool>(100);
        }
        Vertex create_vectex(const T& t) {
            vertices_.push_back(t);
            return {*this, vertices_.size() - 1};
        }
        void add_edge(const Vertex& v1, const Vertex& v2) {
            edges_[v1.index_][v2.index_] = true;
            edges_[v2.index_][v1.index_] = true;
        }
};

