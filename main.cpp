#include "SymbolNameMap.h"
#include <stdio.h>
#include <QDebug>

int main(int /*argc*/, char ** /*argv*/)
{
    SymbolNameMap map;
    map.insert("aa0", Location("foobar", 1, 1));
    map.insert("ab0", Location("foobar", 1, 1));
    map.dump();
    SymbolNameMap::iterator it = map.begin();
    while (it != map.end()) {
        qDebug() << it.key() << it.values();
        ++it;
    }

    qDebug() << map.values("aa0", SymbolNameMap::CaseSensitive);
    qDebug() << map.values("Aa0", SymbolNameMap::CaseSensitive);
    qDebug() << map.values("Aa0", SymbolNameMap::CaseInsensitive);
    return 0;

    // map.insert("A", Location("foobar", 1, 1));
    // map.insert("a", Location("foobar2", 1, 1));
    // map.insert("A", Location("foobar2", 1, 1));

    // qDebug() << map.size();
    // return 0;

    // printf("A: %ld\n", map.values("A", SymbolNameMap::CaseInsensitive).size());
    // printf("A: %ld\n", map.values("a", SymbolNameMap::CaseInsensitive).size());
    // printf("A: %ld\n", map.values("a", SymbolNameMap::CaseSensitive).size());
    map.insert("foobar", Location("foobar", 1, 1));
    //map.insert("fOobAr0", Location("foobar", 1, 1));
    qDebug() << map.values("foobar", SymbolNameMap::CaseSensitive);
    qDebug() << map.values("foobar", SymbolNameMap::CaseInsensitive);
    return 0;
    printf("foobar before remove %ld\n", map.values("foobar", SymbolNameMap::CaseSensitive).size());
    printf("removing %d\n", map.remove("foobar0"));
    printf("after remove cs %ld\n", map.values("foobar0", SymbolNameMap::CaseSensitive).size());
    printf("after remove cis %ld\n", map.values("foobar0", SymbolNameMap::CaseInsensitive).size());
    return 0;
}
