#include <bits/stdc++.h>

using namespace std;

struct st
{
    string name;
    vector<string> a, b, eps;
    bool start = false, final = false;
};

vector<st> nfa;
vector<vector<string>> dfa;
vector<vector<int>> table;
vector<string> finals;

string tr(string s)
{
    while (!s.empty() && s[0] == ' ')
        s.erase(0, 1);

    while (!s.empty() && s[s.size() - 1] == ' ')
        s.pop_back();

    return s;
}

vector<string> split(string s)
{
    vector<string> result;

    s = tr(s);

    if (s == "{}" || s == "∅" || s.empty())
        return result;

    if (s[0] == '{')
        s.erase(0, 1);

    if (s[s.size() - 1] == '}')
        s.pop_back();

    string word = "";

    for (char c : s)
    {
        if (c == ',')
        {
            word = tr(word);
            if (word != "" && word != "∅")
                result.push_back(word);
            word = "";
        }
        else
        {
            word += c;
        }
    }

    word = tr(word);
    if (word != "" && word != "∅")
        result.push_back(word);

    return result;
}

int findst(string name)
{
    for (int i = 0; i < nfa.size(); i++)
    {
        if (nfa[i].name == name)
            return i;
    }

    return -1;
}

void addd(vector<string> &v, string s)
{
    if (find(v.begin(), v.end(), s) == v.end())
        v.push_back(s);
}

vector<string> closure(vector<string> states)
{
    vector<string> result = states;

    for (int i = 0; i < result.size(); i++)
    {
        int k = findst(result[i]);

        for (string next : nfa[k].eps)
            addd(result, next);
    }

    sort(result.begin(), result.end());
    return result;
}

vector<string> khujo(vector<string> states, char symbol)
{
    vector<string> result;

    for (string s : states)
    {
        int k = findst(s);

        if (k == -1)
            continue;

        if (symbol == 'a')
        {
            for (string next : nfa[k].a)
                addd(result, next);
        }
        else
        {
            for (string next : nfa[k].b)
                addd(result, next);
        }
    }

    return closure(result);
}

int findDFA(vector<string> states)
{
    for (int i = 0; i < dfa.size(); i++)
    {
        if (dfa[i] == states)
            return i;
    }

    return -1;
}

int main()
{
    ifstream file("input.txt");
    string line;

    if (!file.is_open())
    {
        cout << "Cannot open input.txt";
        return 0;
    }

    getline(file, line);

    while (getline(file, line))
    {
        if (tr(line) == "")
            continue;

        vector<string> parts;
        string word = "";
        int brace = 0;

        for (char c : line)
        {
            if (c == '{')
                brace++;

            if (c == '}')
                brace--;

            if (c == ',' && brace == 0)
            {
                parts.push_back(tr(word));
                word = "";
            }
            else
            {
                word += c;
            }
        }

        parts.push_back(tr(word));

        if (parts.size() != 4)
            continue;

        st s;
        s.name = parts[0];

        if (s.name.substr(0, 2) == "->")
        {
            s.start = true;
            s.name = s.name.substr(2);
        }

        if (s.name[0] == '*')
        {
            s.final = true;
            s.name = s.name.substr(1);
        }

        s.name = tr(s.name);
        s.a = split(parts[1]);
        s.b = split(parts[2]);
        s.eps = split(parts[3]);

        nfa.push_back(s);
    }

    string start;

    for (st s : nfa)
    {
        if (s.start)
            start = s.name;

        if (s.final)
            finals.push_back(s.name);
    }

    vector<string> initial;
    initial.push_back(start);
    initial = closure(initial);

    dfa.push_back(initial);
    table.push_back({-1, -1});

    for (int i = 0; i < dfa.size(); i++)
    {
        for (int j = 0; j < 2; j++)
        {
            char symbol = (j == 0) ? 'a' : 'b';

            vector<string> next = khujo(dfa[i], symbol);

            int index = findDFA(next);

            if (index == -1)
            {
                dfa.push_back(next);
                table.push_back({-1, -1});
                index = dfa.size() - 1;
            }

            table[i][j] = index;
        }
    }

    cout << "st\t\ta\t\tb\n";

    for (int i = 0; i < dfa.size(); i++)
    {
        cout << "S" << i;
        // for (auto s: dfa[i]){cout << " "<< s;}

        if (i == 0)
            cout << "(s)";

        bool isFinal = false;

        for (string s : dfa[i])
        {

            if (find(finals.begin(), finals.end(), s) != finals.end())
                isFinal = true;
        }

        if (isFinal)
            cout << "(f)";

        cout << "\t\tS" << table[i][0];
        cout << "\t\tS" << table[i][1] << "\n";
    }
    // for (int i = 0; i < dfa.size(); i++)
    // {
    //     cout << "S" << i << "= {";
    //     for (auto s : dfa[i])
    //     {
    //         cout << " " << s;
    //     }
    //     cout << " }" << endl;
    // }

    return 0;
}