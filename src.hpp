#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_set>

class pylist {
    struct Inner {
        enum class Type { INT, LIST };
        Type type;
        int ival;
        std::vector<pylist> lst;

        Inner() : type(Type::LIST), ival(0), lst() {}
        explicit Inner(int v) : type(Type::INT), ival(v), lst() {}
    };

    std::shared_ptr<Inner> p;

    // Helper for printing with cycle detection
    static void print_impl(std::ostream &os, const pylist &node,
                           std::unordered_set<const Inner*> &visiting) {
        if (!node.p) {
            os << "[]";
            return;
        }
        if (node.p->type == Inner::Type::INT) {
            os << node.p->ival;
            return;
        }
        const Inner *addr = node.p.get();
        if (visiting.find(addr) != visiting.end()) {
            os << "[...]";
            return;
        }
        visiting.insert(addr);
        os << "[";
        for (size_t i = 0; i < node.p->lst.size(); ++i) {
            if (i) os << ", ";
            print_impl(os, node.p->lst[i], visiting);
        }
        os << "]";
        visiting.erase(addr);
    }

public:
    // Constructors
    pylist() : p(std::make_shared<Inner>()) {}
    pylist(const pylist &) = default;
    pylist(pylist &&) noexcept = default;
    explicit pylist(int v) : p(std::make_shared<Inner>(v)) {}

    // Assignment
    pylist &operator=(const pylist &) = default;
    pylist &operator=(pylist &&) noexcept = default;
    pylist &operator=(int v) {
        p = std::make_shared<Inner>(v);
        return *this;
    }

    // Append operations (amortized O(1))
    void append(const pylist &x) {
        // Ensure we are a list; if currently an int, turn into empty list then append
        if (p->type != Inner::Type::LIST) {
            p = std::make_shared<Inner>();
        }
        p->lst.push_back(x);
    }

    void append(int x) { append(pylist(x)); }

    // Pop last element (amortized O(1))
    pylist pop() {
        // Assume valid usage (non-empty list) as per problem statement
        pylist ret = p->lst.back();
        p->lst.pop_back();
        return ret;
    }

    // Indexing (O(1))
    pylist &operator[](size_t i) {
        return p->lst[i];
    }

    const pylist &operator[](size_t i) const {
        return p->lst[i];
    }

    // Implicit conversion to int for arithmetic and comparisons when holding int
    operator int() const {
        return p->ival;
    }

    // Stream output
    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        std::unordered_set<const Inner*> visiting;
        print_impl(os, ls, visiting);
        return os;
    }
};

#endif // PYLIST_H

