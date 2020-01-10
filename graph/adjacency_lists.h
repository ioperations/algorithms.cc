#pragma once

#include <iostream>

#include "vector"
#include "forward_list.h"

template<typename T>
class Adjacency_lists {
    public:
        class Vertex {
            private:
                friend class Adjacency_lists;
                friend class Vector<Vertex>;

                Adjacency_lists* adjacency_lists_;
                T value_;
                size_t index_;
                Forward_list<size_t> links_;

                Vertex(Adjacency_lists* adjacency_lists, T value, size_t index) 
                    :adjacency_lists_(adjacency_lists), value_(value), index_(index) 
                {}
                Vertex() :adjacency_lists_(nullptr) {}

                Vertex(const Vertex&) = delete;
                Vertex& operator=(const Vertex&) = delete;
                Vertex(Vertex&&) = default;
                Vertex& operator=(Vertex&&) = default;

                void add_link(const Vertex& v) {
                    bool found = false;
                    for (auto link = links_.begin(); link != links_.end() && !found; ++link)
                        found = *link == v.index_;
                    if (!found)
                        links_.push_back(v.index_);
                }
            public:
                class Iterator {
                    private:
                        friend class Vertex;
                        const Vertex& vertex_;
                        mutable Forward_list<size_t>::const_iterator it_;
                        Iterator(const Vertex& vertex, const Forward_list<size_t>::const_iterator& it)
                            :vertex_(vertex), it_(it)
                        {}
                    public:
                        const Iterator& operator++() const {
                            ++it_;
                            return *this;
                        }
                        bool operator!=(const Iterator& o) const {
                            return it_ != o.it_;
                        }
                        const Vertex* operator->() const {
                            return &operator*();
                        }
                        const Vertex& operator*() const {
                            return vertex_.adjacency_lists_->vertices_[*it_];
                        }
                };
                size_t index() const {
                    return index_;
                }
                bool operator==(const Vertex& o) const {
                    return index_ == o.index_;
                }
                const Iterator cbegin() const {
                    return Iterator(*this, links_.cbegin());
                }
                const Iterator cend() const {
                    return Iterator(*this, links_.cend());
                }
                friend std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
                    return stream << v.value_;
                }
        };
    private:
        Vector<Vertex> vertices_;
    public:
        Vertex& create_vertex(const T& t) {
            vertices_.push_back(Vertex(this, t, vertices_.size()));
            return vertices_[vertices_.size() - 1];
        }
        void add_edge(Vertex& v1, Vertex& v2) {
            v1.add_link(v2);
            v2.add_link(v1);
        }
        size_t vertices_count() const {
            return vertices_.size();
        }
        void print_internal(std::ostream& stream) {
            for (auto& v : vertices_) {
                stream << v.index_ << ": ";
                for (auto& i : v.links_)
                    stream << i << " ";
                stream << std::endl;
            }
        }
        bool has_simple_path(const Vertex& v1, const Vertex& v2) {
            return false;
        }
};

