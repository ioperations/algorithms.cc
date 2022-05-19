#include <map>
#include <string>
#include <type_traits>
#include <vector>

template <typename T = std::string>
class Trie {
   public:
    Trie(){};
    virtual ~Trie(){};

    void Insert(const std::vector<T>& l) {
        Tree* z = &root;
        for (auto& ptr : l) {
            z = &z->map[ptr];
        }
        z->end = true;
    }

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, std::string>, void> Insert(
        std::string s) {
        Tree* z = &root;
        for (auto& ptr : s) {
            z = &z->map[std::to_string(ptr)];
        }
        z->end = true;
    }

    bool Search(const std::vector<T>& l) {
        Tree* z = &root;
        for (auto& ptr : l) {
            if (z->map.find(ptr) != z->map.end()) {
                z = &z->map[ptr];
            } else {
                return false;
            }
        }
        return z->end;
    }

    template <typename U = T>
    auto Search(std::string s) ->
        typename std::enable_if_t<std::is_same<U, std::string>::value, bool> {
        Tree* z = &root;
        for (auto& ptr : s) {
            if (z->map.find(std::to_string(ptr)) != z->map.end()) {
                z = &z->map[std::to_string(ptr)];
            } else {
                return false;
            }
        }
        return z->end;
    }

    template <typename U = T>
    auto StartWith(std::string s) ->
        typename std::enable_if_t<std::is_same_v<U, std::string>, bool> {
        Tree* z = &root;
        for (auto& ptr : s) {
            if (z->map.find(std::to_string(ptr)) != z->map.end()) {
                z = &z->map[std::to_string(ptr)];
            } else {
                return false;
            }
        }
        return true;
    }

    bool StartWith(const std::vector<T>& l) {
        Tree* z = &root;
        for (auto& ptr : l) {
            if (z->map.find(ptr) != z->map.end()) {
                z = &z->map[ptr];
            } else {
                return false;
            }
        }
        return true;
    }

   private:
    struct Tree {
        std::map<T, Tree> map;
        bool end;
        ~Tree() { map.clear(); }
    };
    Tree root;
};
