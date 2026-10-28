#include <iostream>   // for cin, cout, cerr
#include <fstream>    // for reading the file
#include <map>        // for transition tables
#include <set>        // for sets of NFA states
#include <queue>      // for the work-list of DFA states
#include <vector>     // for dynamic arrays
#include <string>     // for string handling
using namespace std;  // avoid writing std:: everywhere

int main(int argc, char* argv[]) {
    string fileName = (argc > 1) ? argv[1] : "nfa.txt"; // file name from command line, default nfa.txt
    ifstream in(fileName);                               // open the file
    if (!in) {                                           // EDGE CASE: file missing or unreadable
        cerr << "Error: cannot open file " << fileName << endl; // tell the user
        return 1;                                        // stop with error
    }

    int n, m;                                            // n = number of NFA states, m = number of symbols
    if (!(in >> n >> m) || n <= 0 || m <= 0) {           // EDGE CASE: bad or missing header values
        cerr << "Error: invalid state/symbol count\n";   // report problem
        return 1;                                        // stop
    }

    vector<char> symbols(m);                             // the input alphabet
    for (int i = 0; i < m; i++) in >> symbols[i];        // read each symbol

    int start;                                           // NFA start state
    in >> start;                                         // read it
    if (start < 0 || start >= n) {                       // EDGE CASE: start state out of range
        cerr << "Error: invalid start state\n";          // report problem
        return 1;                                        // stop
    }

    int k;                                               // number of accepting NFA states
    in >> k;                                             // read the count
    set<int> nfaAccept;                                  // set of accepting NFA states
    for (int i = 0; i < k; i++) {                        // read each accepting state
        int x; in >> x;                                  // read one state
        if (x < 0 || x >= n) {                           // EDGE CASE: accepting state out of range
            cerr << "Error: invalid accepting state " << x << endl; // report problem
            return 1;                                    // stop
        }
        nfaAccept.insert(x);                             // store it
    }

    map<pair<int, char>, set<int>> nfa;                  // NFA table: (state, symbol) -> set of next states
    int from, to; char sym;                              // variables for one transition line
    while (in >> from >> sym >> to) {                    // read until the file ends
        bool okSym = false;                              // check if the symbol is in the alphabet
        for (char c : symbols) if (c == sym) okSym = true; // search the alphabet
        if (from < 0 || from >= n || to < 0 || to >= n || !okSym) { // EDGE CASE: invalid transition line
            cerr << "Error: bad transition " << from << " " << sym << " " << to << endl; // report
            return 1;                                    // stop
        }
        nfa[{from, sym}].insert(to);                     // add the transition (NFA allows many targets)
    }

    // ---------- SUBSET CONSTRUCTION ----------
    map<set<int>, int> id;                               // maps a set of NFA states -> DFA state number
    vector<set<int>> dfaStates;                          // DFA state number -> its set of NFA states
    vector<map<char, int>> dfa;                          // DFA table: dfa[state][symbol] = next state (-1 = dead)
    queue<int> work;                                     // DFA states still to be processed

    set<int> s0 = {start};                               // the DFA start state = {NFA start}
    id[s0] = 0;                                          // give it number 0
    dfaStates.push_back(s0);                             // store its NFA-state set
    dfa.push_back({});                                   // make an empty row for it
    work.push(0);                                        // add to work-list

    while (!work.empty()) {                              // repeat until no unprocessed DFA state remains
        int cur = work.front(); work.pop();              // take the next DFA state
        set<int> curSet = dfaStates[cur];                // copy its NFA set (vector may grow later)
        for (char c : symbols) {                         // try every input symbol
            set<int> next;                               // the set reached on symbol c
            for (int s : curSet) {                       // for each NFA state inside the current set
                auto it = nfa.find({s, c});              // look for transitions on c
                if (it != nfa.end())                     // if there are any
                    next.insert(it->second.begin(), it->second.end()); // add all targets (union)
            }
            if (next.empty()) {                          // EDGE CASE: no move possible
                dfa[cur][c] = -1;                        // -1 means dead state (reject)
            } else {                                     // otherwise there is a real target set
                if (id.find(next) == id.end()) {         // if this set is new
                    int newId = dfaStates.size();        // assign next free number
                    id[next] = newId;                    // remember it
                    dfaStates.push_back(next);           // store its NFA set
                    dfa.push_back({});                   // add an empty row for it
                    work.push(newId);                    // process it later
                }
                dfa[cur][c] = id[next];                  // record the DFA transition
            }
        }
    }

    // A DFA state is accepting if its set contains ANY accepting NFA state
    vector<bool> dfaAccept(dfaStates.size(), false);     // accepting flag for each DFA state
    for (size_t i = 0; i < dfaStates.size(); i++)        // go through each DFA state
        for (int s : dfaStates[i])                       // go through its NFA states
            if (nfaAccept.count(s)) dfaAccept[i] = true; // mark accepting if any member accepts

    // ---------- PRINT THE DFA ----------
    cout << "\nDFA Transition Table\n";                  // heading
    cout << "State\tNFA set\t\t";                         // column titles
    for (char c : symbols) cout << c << "\t";            // one column per symbol
    cout << "\n";                                        // end of header line
    for (size_t i = 0; i < dfaStates.size(); i++) {      // each DFA state
        cout << (i == 0 ? "->" : "  ")                   // mark the start state
             << (dfaAccept[i] ? "*" : " ")               // mark accepting states with *
             << "D" << i << "\t{";                       // DFA state name
        bool first = true;                               // for comma printing
        for (int s : dfaStates[i]) {                     // print NFA set members
            if (!first) cout << ",";                     // comma between items
            cout << s; first = false;                    // print the state number
        }
        cout << "}\t\t";                                 // close the set
        for (char c : symbols) {                         // print each transition
            int t = dfa[i][c];                           // get the target
            if (t == -1) cout << "-\t";                  // dead state shown as -
            else cout << "D" << t << "\t";               // otherwise show target name
        }
        cout << "\n";                                    // next row
    }

    // ---------- TEST STRINGS ----------
    cout << "\nEnter a string to test (empty line = empty string, 'quit' to stop):\n"; // instructions
    string line;                                         // holds user input
    getline(cin, line);                                  // clear any leftover newline safely
    while (true) {                                       // loop until the user quits
        cout << "> ";                                    // prompt
        if (!getline(cin, line)) break;                  // EDGE CASE: end of input (Ctrl+D)
        if (line == "quit") break;                       // stop on 'quit'

        int cur = 0;                                     // start at DFA start state
        bool valid = true;                               // becomes false on a bad symbol or dead move
        for (char ch : line) {                           // read the string one char at a time
            if (dfa[cur].find(ch) == dfa[cur].end()) {   // EDGE CASE: symbol not in alphabet
                cout << "Invalid symbol '" << ch << "' -> REJECTED\n"; // tell the user
                valid = false; break;                    // stop checking
            }
            cur = dfa[cur][ch];                          // move to the next state
            if (cur == -1) { valid = false; break; }     // dead state: reject immediately
        }
        if (!valid && cur == -1)                         // rejected through a dead move
            cout << "REJECTED\n";                        // print result
        else if (valid)                                  // string fully consumed
            cout << (dfaAccept[cur] ? "ACCEPTED" : "REJECTED") << "\n"; // accept only in accepting state
    }
    return 0;                                            // finished successfully
}