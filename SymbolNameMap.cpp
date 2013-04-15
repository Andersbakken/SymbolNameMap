#include "SymbolNameMap.h"

bool SymbolNameMap::contains(const std::string &string, CaseSensitivity cs) const
{
    return mRoot && findNode(mRoot, string.c_str(), string.size(), cs);
}

std::set<Location> SymbolNameMap::values(const std::string &string, CaseSensitivity cs) const
{
    std::set<Location> locations;
    if (mRoot)
        findNode(mRoot, string.c_str(), string.size(), cs, &locations);
    return locations;
}

bool SymbolNameMap::remove(const std::string &string, const Location &location)
{
    return mRoot && remove(mRoot, string.c_str(), 0, string.size(), &location);
}

bool SymbolNameMap::remove(const std::string &string)
{
    return mRoot && remove(mRoot, string.c_str(), 0, string.size(), 0);
}
bool SymbolNameMap::remove(Node *node, const char *string, int idx, int len, const Location *loc)
{
    Node *child = node->children[index(string[idx])];
    if (!child)
        return false;
    if (idx + 1 == len) {
        if ((loc && child->locations.erase(*loc)) || (!loc && !child->locations.empty())) {
            if (!loc) {
                mSize -= child->locations.size();
                child->locations.clear();
            } else {
                --mSize;
            }
            while (child->locations.empty() && !child->childCount) {
                if (child != mRoot) {
                    Node *p = child->parent;
                    --p->childCount;
                    delete child;
                    p->children[index(string[idx--])] = 0;
                    child = p;
                } else {
                    mRoot = 0;
                    break;
                }
            }

            return true;
        }
        return false;
    } else {
        return remove(child, string, idx + 1, len, loc);
    }
    return false;
}

void SymbolNameMap::insert(const std::string &string, const Location &location)
{
    if (!mRoot)
        mRoot = new Node(0, 0);
    insert(mRoot, string.c_str(), string.size(), location);
}

void SymbolNameMap::insert(Node *node, const char *string, int len, const Location &location)
{
    Node *&child = node->children[index(*string)];
    if (!child) {
        child = new Node(node, *string);
        ++node->childCount;
    }
    if (len == 1) {
        if (child->locations.insert(location).second)
            ++mSize;
    } else {
        insert(child, string + 1, len - 1, location);
    }
}

bool SymbolNameMap::findNode(Node *node, const char *string, int len, CaseSensitivity cs, std::set<Location> *locations) const
{
    bool ret = false;
    Node *childa = node->children[index(*string)];
    Node *childb = 0;
    if (cs == CaseInsensitive) {
        enum { Diff = 'a' - 'A' };
        char ch = 0;
        if (*string >= 'a' && *string <= 'z') {
            ch = *string - Diff;
        } else if (*string >= 'A' && *string <= 'Z') {
            ch = *string + Diff;
        }
        if (ch) {
            childb = node->children[index(ch)];
            if (childb) {
                if (len == 1) {
                    if (locations) {
                        for (std::set<Location>::const_iterator it = childb->locations.begin(); it != childb->locations.end(); ++it) {
                            locations->insert(*it);
                        }
                    }
                    ret |= !childb->locations.empty();
                } else {
                    ret = findNode(childb, string + 1, len - 1, cs, locations);
                }
            }
        }
    }
    if (childa) {
        if (len == 1) {
            if (locations) {
                for (std::set<Location>::const_iterator it = childa->locations.begin(); it != childa->locations.end(); ++it) {
                    locations->insert(*it);
                }
            }
            ret |= !childa->locations.empty();
        } else {
            ret |= findNode(childa, string + 1, len - 1, cs, locations);
        }
    }
    return ret;
}

SymbolNameMap::iterator SymbolNameMap::begin() const
{
    const Node *n =  next(mRoot);
    return iterator(this, n);
}

SymbolNameMap::iterator SymbolNameMap::end() const
{
    return iterator(this, 0);
}

const SymbolNameMap::Node * SymbolNameMap::next(const Node *node) const
{
    const Node *ret = firstChild(node);
    if (ret)
        return ret;
    ret = nextSibling(node);
    if (ret)
        return ret;
    const Node *parent = node->parent;
    while (parent && parent->parent) {
        ret = nextSibling(parent);
        if (ret)
            return ret;
        parent = parent->parent;
    }
    return 0;
}

const SymbolNameMap::Node *SymbolNameMap::firstChild(const Node *node) const
{
    assert(node);
    if (node->childCount) {
        for (int i=0; i<MaxChildrenSize; ++i) {
            if (node->children[i]) {
                if (node->children[i]->locations.empty()) {
                    return next(node->children[i]);
                } else {
                    return node->children[i];
                }
            }
        }
    }
    return 0;
}

const SymbolNameMap::Node *SymbolNameMap::nextSibling(const Node *node) const
{
    assert(node);
    assert(node->parent);
    for (int i=index(node->ch) + 1; i<MaxChildrenSize; ++i) {
        if (node->parent->children[i]) {
            if (node->parent->children[i]->locations.empty()) {
                return next(node->parent->children[i]);
            } else {
                return node->parent->children[i];
            }
        }
    }

    return 0;
}

std::string SymbolNameMap::iterator::key() const
{
    int size = 0;
    const Node *tmp = node;
    while (tmp->parent) {
        ++size;
        tmp = tmp->parent;
    }
    std::string ret(size, '0');
    tmp = node;
    while (tmp->parent) {
        ret[--size] = tmp->ch;
        tmp = tmp->parent;
    }
    return ret;
}

std::set<Location> SymbolNameMap::iterator::values() const
{
    return node->locations;
}

void SymbolNameMap::dump(const Node *node, int indent) const
{
    if (!node) {
        if (mRoot) {
            node = mRoot;
        } else {
            return;
        }
    }
    for (int i=0; i<indent; ++i) {
        printf("  ");
    }
    if (node->ch) {
        printf("%c", node->ch);
        if (!node->locations.empty()) {
            printf(":");
            for (std::set<Location>::const_iterator it = node->locations.begin(); it != node->locations.end(); ++it) {
                printf(" %s:%d:%d", it->path.c_str(), it->line, it->column);
            }
        }
        printf("\n");
    }
    if (node->childCount) {
        for (int i=0; i<MaxChildrenSize; ++i) {
            if (node->children[i]) {
                dump(node->children[i], indent + 1);
            }
        }
    }

}
