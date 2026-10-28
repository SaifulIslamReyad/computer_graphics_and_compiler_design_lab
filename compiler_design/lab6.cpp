
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;

map<char, vector<string>> grammar;
map<char, set<char>> first, follow;
set<char> nonTerminals;
char startSymbol;

bool isnonterminal(char c)
{
    return nonTerminals.count(c);
}

set<char> FIRST(char c)
{
    if (!isnonterminal(c))
        return {c};

    if (!first[c].empty())
        return first[c];

    for (string p : grammar[c])
    {
        if (p == "#")
        {
            first[c].insert('#');
            continue;
        }

        bool emptystringprod = true;

        for (int i = 0; i < p.size(); i++)
        {
            set<char> temp = FIRST(p[i]);

            for (char x : temp)
            {
                if (x != '#')
                    first[c].insert(x);
            }

            if (!temp.count('#'))
            {
                emptystringprod = false;
                break;
            }
        }

        if (emptystringprod)
            first[c].insert('#');
    }

    return first[c];
}

void calcFIRST()
{
    for (char c : nonTerminals)
        FIRST(c);
}

void calcFOLLOW()
{
    follow[startSymbol].insert('$');

    bool changed = true;

    while (changed)
    {
        changed = false;

        for (char A : nonTerminals)
        {
            for (string p : grammar[A])
            {
                for (int i = 0; i < p.size(); i++)
                {
                    char B = p[i];

                    if (!isnonterminal(B))
                        continue;

                    set<char> old = follow[B];
                    bool emptystringprod = true;

                    for (int j = i + 1; j < p.size(); j++)
                    {
                        set<char> temp = FIRST(p[j]);

                        for (char x : temp)
                        {
                            if (x != '#')
                                follow[B].insert(x);
                        }

                        if (!temp.count('#'))
                        {
                            emptystringprod = false;
                            break;
                        }
                    }

                    if (emptystringprod)
                    {
                        follow[B].insert(
                            follow[A].begin(),
                            follow[A].end()
                        );
                    }

                    if (old != follow[B])
                        changed = true;
                }
            }
        }
    }
}

void ps(map<char, set<char>>& sets)
{
    for (char c : nonTerminals)
    {
        cout << c << " = { ";

        bool firstElement = true;

        for (char x : sets[c])
        {
            if (!firstElement)
                cout << ", ";

            cout << x;
            firstElement = false;
        }

        cout << " }" << endl;
    }
}

int main()
{
    int n;

    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++)
    {
        string line;

        getline(cin, line);

        char left = line[0];
        string right = line.substr(line.find("->") + 2);

        nonTerminals.insert(left);
        grammar[left].push_back(right);
    }

    startSymbol = grammar.begin()->first;

    calcFIRST();
    calcFOLLOW();

    cout << "\nFIRST Sets:\n";
    ps(first);

    cout << "\nFOLLOW Sets:\n";
    ps(follow);

    return 0;
}