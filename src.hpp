#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <memory>

class pylist {
    enum class Kind { INT, LIST };

    struct Node {
        std::vector<pylist> elems;
    };

    struct Heap {
        std::vector<std::unique_ptr<Node>> pool;
        size_t make_list() {
            pool.emplace_back(new Node());
            return pool.size() - 1;
        }
        Node &get(size_t id) { return *pool[id]; }
        static Heap &inst() {
            static Heap h;
            return h;
        }
    };

    Kind kind;
    int ival;
    size_t id; // valid when kind == LIST

    static void print_impl(std::ostream &os, const pylist &x, std::unordered_set<size_t> &vis) {
        if (x.kind == Kind::INT) {
            os << x.ival;
            return;
        }
        size_t nid = x.id;
        if (vis.find(nid) != vis.end()) {
            os << "[...]";
            return;
        }
        vis.insert(nid);
        os << "[";
        auto &vec = Heap::inst().get(nid).elems;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i) os << ", ";
            print_impl(os, vec[i], vis);
        }
        os << "]";
        vis.erase(nid);
    }

    void ensure_list() {
        if (kind != Kind::LIST) {
            id = Heap::inst().make_list();
            kind = Kind::LIST;
        }
    }

public:
    // Constructors
    pylist() : kind(Kind::LIST), ival(0), id(Heap::inst().make_list()) {}
    explicit pylist(int v) : kind(Kind::INT), ival(v), id(0) {}

    // Default copy/move semantics (alias for lists)
    pylist(const pylist &) = default;
    pylist(pylist &&) noexcept = default;
    pylist &operator=(const pylist &) = default;
    pylist &operator=(pylist &&) noexcept = default;

    // Assign from int (turns this into an int value)
    pylist &operator=(int v) {
        kind = Kind::INT;
        ival = v;
        return *this;
    }

    // Append
    void append(const pylist &x) {
        ensure_list();
        Heap::inst().get(id).elems.push_back(x);
    }
    void append(int v) { append(pylist(v)); }

    // Pop
    pylist pop() {
        auto &vec = Heap::inst().get(id).elems;
        pylist ret = vec.back();
        vec.pop_back();
        return ret;
    }

    // Indexing
    pylist &operator[](size_t i) { return Heap::inst().get(id).elems[i]; }
    const pylist &operator[](size_t i) const { return Heap::inst().get(id).elems[i]; }

    // Implicit conversion to int when holding int
    operator int() const { return ival; }

    // Printing
    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        std::unordered_set<size_t> vis;
        print_impl(os, ls, vis);
        return os;
    }
};

#endif // PYLIST_H
