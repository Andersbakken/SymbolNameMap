#ifndef SymbolNameMap_h
#define SymbolNameMap_h

#include <string>
#include <set>
#include <string.h>
#include <assert.h>
#include <QDebug>

class Location
{
public:
    Location(const std::string &p = std::string(), int l = 0, int c = 0  ) : path(p), line(l), column(c) {}
    bool operator<(const Location &other) const
    {
        if (path < other.path) {
            return true;
        } else if (path > other.path) {
            return false;
        }
        if (line < other.line) {
            return true;
        } else if (line > other.line) {
            return false;
        }
        return column < other.column;
    }
    bool operator==(const Location &other) const
    {
        return path == other.path && line == other.line && column == other.column;
    }
    std::string path;
    int line, column;
};

inline QDebug operator<<(QDebug dbg, const Location &loc)
{
    dbg << QString("%1:%2:%3").arg(loc.path.c_str()).arg(loc.line).arg(loc.column);
    return dbg;
}

template <typename T>
inline QDebug operator<<(QDebug dbg, const std::set<T> &set)
{
    dbg << "<std::set>";
    for (typename std::set<T>::const_iterator it = set.begin(); it != set.end(); ++it) {
        dbg << *it;
    }
    dbg << "</std::set>";
    return dbg;
}

class SymbolNameMap
{
    struct Node;
public:
    SymbolNameMap() : mRoot(0), mSize(0) {}
    ~SymbolNameMap() { delete mRoot; }
    enum CaseSensitivity {
        CaseSensitive,
        CaseInsensitive
    };
    bool contains(const std::string &string, CaseSensitivity cs) const;
    std::set<Location> values(const std::string &string, CaseSensitivity cs) const;
    void insert(const std::string &string, const Location &location);
    bool remove(const std::string &string, const Location &location);
    bool remove(const std::string &string);
    int size() const { return mSize; }
    class iterator
    {
    public:
        std::string key() const;
        std::set<Location> values() const;
        iterator &operator++() { node = map->next(node); return *this; }
        bool operator==(const iterator &other) const
        {
            assert(map && map == other.map);
            return node == other.node;
        }
        bool operator!=(const iterator &other) const
        {
            assert(map && map == other.map);
            return node != other.node;
        }
    private:
        iterator(const SymbolNameMap *m, const Node *n) : map(m), node(n) {}

        const SymbolNameMap *map;
        const Node *node;

        friend class SymbolNameMap;
    };
    iterator begin() const;
    iterator end() const;
    void dump(const Node *node = 0, int indent = 0) const;
private:
    enum { MaxChildrenSize = 61 };
    struct Node;
    const Node *next(const Node *node) const;
    const Node *firstChild(const Node *node) const;
    const Node *nextSibling(const Node *node) const;
    void insert(Node *node, const char *string, int len, const Location &location);
    bool remove(Node *node, const char *string, int idx, int len, const Location *loc);
    bool findNode(Node *node, const char *string, int len, CaseSensitivity cs, std::set<Location> *locations = 0) const;
    struct Node {
        Node(Node *p, char c)
            : parent(p), childCount(0), ch(c)
        {
            memset(children, 0, sizeof(children));
        }
        ~Node()
        {
            for (int i=0; i<MaxChildrenSize; ++i)
                delete children[i];
        }
        Node *parent;
        int childCount;
        const char ch;
        Node *children[MaxChildrenSize];
        std::set<Location> locations;
    } *mRoot;
    int mSize;
    static inline int index(char ch)
    {
        switch (ch) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': return 10;
        case 'b': return 11;
        case 'c': return 12;
        case 'd': return 13;
        case 'e': return 14;
        case 'f': return 15;
        case 'g': return 16;
        case 'h': return 17;
        case 'i': return 18;
        case 'j': return 19;
        case 'k': return 20;
        case 'l': return 21;
        case 'm': return 22;
        case 'n': return 23;
        case 'o': return 24;
        case 'p': return 25;
        case 'q': return 26;
        case 'r': return 27;
        case 's': return 28;
        case 't': return 29;
        case 'u': return 30;
        case 'v': return 31;
        case 'w': return 32;
        case 'y': return 33;
        case 'z': return 34;
        case 'A': return 35;
        case 'B': return 36;
        case 'C': return 37;
        case 'D': return 38;
        case 'E': return 39;
        case 'F': return 40;
        case 'G': return 41;
        case 'H': return 42;
        case 'I': return 43;
        case 'J': return 44;
        case 'K': return 45;
        case 'L': return 46;
        case 'M': return 47;
        case 'N': return 48;
        case 'O': return 49;
        case 'P': return 50;
        case 'Q': return 51;
        case 'R': return 52;
        case 'S': return 53;
        case 'T': return 54;
        case 'U': return 55;
        case 'V': return 56;
        case 'W': return 57;
        case 'Y': return 58;
        case 'Z': return 59;
        case '_': return 60;
        case '(': return 61;
        case ')': return 62;
        case ',': return 63;
        case '<': return 64;
        case '>': return 65;
        case ' ': return 66;
        }
        assert(0);
        return -1;
    }
};

#endif
