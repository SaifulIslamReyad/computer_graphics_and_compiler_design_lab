#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;


bool isOperand(char c)
{
    return isalnum(c);
}


bool isUnaryPostfixOp(char c)
{
    return c == '*' || c == '+' || c == '?';
}

int precedence(char c)
{
    if (isUnaryPostfixOp(c))
        return 3;

    if (c == '.')
        return 2;

    if (c == '|')
        return 1;

    return 0;
}

string removeSpaces(const string& re)
{
    string result;

    for (char c : re)
    {
        if (!isspace(c))
            result += c;
    }

    return result;
}

bool isLeftConcat(char c)
{
  
    return isOperand(c) || c == ')' || isUnaryPostfixOp(c);
}

bool isRightConcat(char c)
{
    return isOperand(c) || c == '(';
}


string addConcatenation(const string& re)
{
    string result;

    for (int i = 0; i < (int)re.length(); i++)
    {
        char current = re[i];

        result += current;

        if (i + 1 < (int)re.length())
        {
            char next = re[i + 1];

            if (isLeftConcat(current) && isRightConcat(next))
            {
                result += '.';
            }
        }
    }

    return result;
}


string infixToPostfix(const string& infix)
{
    string postfix;
    stack<char> operators;

    for (char c : infix)
    {
        // Operand
        if (isOperand(c))
        {
            postfix += c;
        }

        // Opening parenthesis
        else if (c == '(')
        {
            operators.push(c);
        }

        // Closing parenthesis
        else if (c == ')')
        {
            while (!operators.empty() && operators.top() != '(')
            {
                postfix += operators.top();
                operators.pop();
            }

            if (!operators.empty() && operators.top() == '(')
            {
                operators.pop();
            }
        }

        // Operator
        else
        {
            /*
                '*', '+', '?' are postfix unary operators.

                Example:
                    a*  a+  a?
                We can immediately append them right after their operand
                has been processed, no need to touch the operator stack.
            */

            if (isUnaryPostfixOp(c))
            {
                postfix += c;
            }
            else
            {
                while (!operators.empty() &&
                       operators.top() != '(' &&
                       precedence(operators.top()) >= precedence(c))
                {
                    postfix += operators.top();
                    operators.pop();
                }

                operators.push(c);
            }
        }
    }

    while (!operators.empty())
    {
        postfix += operators.top();
        operators.pop();
    }

    return postfix;
}


// ============================================================
// 7. INFIX → PREFIX
//
// We use:
//     Reverse infix
//     Swap ( and )
//     Convert to postfix
//     Reverse postfix
//
// NOTE: this reverse-trick is exact for binary operators (., |)
// and operands, but for the postfix-unary operators (*, +, ?) it
// keeps the operator glued to the SAME operand it was already
// attached to (this matches the original code's behaviour for '*'
// and is kept consistent here for '+' and '?' as well). It is a
// readable "prefix-like" rendering rather than a strict formal
// prefix notation for the unary operators.
// ============================================================

string infixToPrefix(string infix)
{
    reverse(infix.begin(), infix.end());

    for (char& c : infix)
    {
        if (c == '(')
            c = ')';

        else if (c == ')')
            c = '(';
    }

    string postfix = infixToPostfix(infix);

    reverse(postfix.begin(), postfix.end());

    return postfix;
}


// ============================================================
// 8. NFA DATA STRUCTURES
// ============================================================

struct Transition
{
    int from;
    char symbol;
    int to;
};


struct Fragment
{
    int start;
    int accept;
};


vector<Transition> transitions;
int stateCount = 0;

int newState()
{
    return stateCount++;
}


void addTransition(int from, char symbol, int to)
{
    transitions.push_back({from, symbol, to});
}


// ============================================================
// 11. THOMPSON NFA CONSTRUCTION
//
// Postfix operators:
//
//     a       → symbol
//     .       → concatenation
//     |       → union
//     *       → Kleene star        (zero or more)
//     +       → positive closure   (one or more)
//     ?       → optional           (zero or one)
// ============================================================

Fragment buildNFA(const string& postfix)
{
    stack<Fragment> st;

    for (char c : postfix)
    {

        // Stack:
        // [ 0 --a--> 1 ]

        if (isOperand(c))
        {
            int start = newState();
            int accept = newState();
            // start --a--> accept
            addTransition(start, c, accept);
            st.push({start, accept});
        }


        // 0 --a--> 1 --ε--> 2 --b--> 3
        // start = 0
        // accept = 3

        else if (c == '.')
        {
            // The TOP fragment is B
            Fragment B = st.top();
            st.pop();

            // The fragment below B is A
            Fragment A = st.top();
            st.pop();

            // Connect A to B using ε
            addTransition(A.accept, '#', B.start);

            // Push the combined fragment
            st.push({A.start, B.accept});
        }


        // ====================================================
        // CASE 3: UNION '|'
        // ====================================================
        //
        // Example:
        // A|B
        //
        //              ε → A → ε
        //             /         \
        //        new --           --> new
        //             \         /
        //              ε → B → ε

        else if (c == '|')
        {
            // TOP fragment is B
            Fragment B = st.top();
            st.pop();

            // Next fragment is A
            Fragment A = st.top();
            st.pop();

            // Create new start and accept states
            int start = newState();
            int accept = newState();

            // New start can enter A
            addTransition(start, '#', A.start);

            // New start can enter B
            addTransition(start, '#', B.start);

            // A can finish at the new accept
            addTransition(A.accept, '#', accept);

            // B can finish at the new accept
            addTransition(B.accept, '#', accept);

            // Push the combined A|B fragment
            st.push({start, accept});
        }


        // ====================================================
        // CASE 4: KLEENE STAR '*'   (zero or more)
        // ====================================================
        //
        //              ε
        //          ┌────────→ accept
        //          │
        //       start ──ε──→ A.start ... A.accept
        //                                    │
        //                                    ├──ε──→ accept
        //                                    │
        //                                    └──ε──→ A.start   (loop)

        else if (c == '*')
        {
            // Get the fragment A
            Fragment A = st.top();
            st.pop();

            // Create new start and accept states
            int start = newState();
            int accept = newState();

            // Zero repetitions: start --ε--> accept
            addTransition(start, '#', accept);

            // Enter A: start --ε--> A.start
            addTransition(start, '#', A.start);

            // Finish after one execution: A.accept --ε--> accept
            addTransition(A.accept, '#', accept);

            // Repeat A: A.accept --ε--> A.start
            addTransition(A.accept, '#', A.start);

            // Push the complete A* fragment
            st.push({start, accept});
        }


        // ====================================================
        // CASE 5: POSITIVE CLOSURE '+'   (one or more)
        // ====================================================
        //
        // A+ = AA*  → must go through A at least once, then may
        // repeat any number of additional times. Unlike '*' there
        // is NO bypass edge from start straight to accept, so the
        // empty string is NOT accepted.
        //
        //       start ──ε──→ A.start ... A.accept
        //                                    │
        //                                    ├──ε──→ accept
        //                                    │
        //                                    └──ε──→ A.start   (loop)

        else if (c == '+')
        {
            Fragment A = st.top();
            st.pop();

            int start = newState();
            int accept = newState();

            // Must enter A at least once: start --ε--> A.start
            addTransition(start, '#', A.start);

            // Finish after one execution: A.accept --ε--> accept
            addTransition(A.accept, '#', accept);

            // Repeat A: A.accept --ε--> A.start
            addTransition(A.accept, '#', A.start);

            st.push({start, accept});
        }


        // ====================================================
        // CASE 6: OPTIONAL '?'   (zero or one)
        // ====================================================
        //
        //              ε
        //          ┌────────→ accept
        //          │
        //       start ──ε──→ A.start ... A.accept ──ε──→ accept
        //
        // Like '*' but with NO loop-back edge, so A can occur at
        // most once.

        else if (c == '?')
        {
            Fragment A = st.top();
            st.pop();

            int start = newState();
            int accept = newState();

            // Zero occurrences: start --ε--> accept
            addTransition(start, '#', accept);

            // Enter A: start --ε--> A.start
            addTransition(start, '#', A.start);

            // Finish after the (single) occurrence: A.accept --ε--> accept
            addTransition(A.accept, '#', accept);

            st.push({start, accept});
        }
    }

    // After processing the complete postfix expression,
    // only ONE complete NFA fragment should remain.
    return st.top();
}


// ============================================================
// 11b. RENUMBER STATES SO THE START STATE IS ALWAYS q0
// ============================================================
//
// Thompson's construction builds fragments bottom-up, so the
// state that ends up being the overall "start" is usually NOT
// state 0 (extra wrapper states created by |, *, +, ? get higher
// numbers than the states they wrap). This pass does a BFS from
// the real start state and relabels every state in the order it
// is first discovered, so:
//
//     new start  = q0
//     everything else follows BFS discovery order
// ============================================================

void renumberStates(Fragment& nfa)
{
    map<int, int> newId;
    queue<int> bfsQueue;

    newId[nfa.start] = 0;
    bfsQueue.push(nfa.start);

    while (!bfsQueue.empty())
    {
        int current = bfsQueue.front();
        bfsQueue.pop();

        for (const auto& t : transitions)
        {
            if (t.from == current && newId.find(t.to) == newId.end())
            {
                newId[t.to] = (int)newId.size();
                bfsQueue.push(t.to);
            }
        }
    }

    // Safety net: relabel any state BFS didn't reach (shouldn't
    // happen with a well-formed Thompson NFA, but keeps things
    // robust just in case).
    for (int s = 0; s < stateCount; s++)
    {
        if (newId.find(s) == newId.end())
            newId[s] = (int)newId.size();
    }

    for (auto& t : transitions)
    {
        t.from = newId[t.from];
        t.to   = newId[t.to];
    }

    nfa.start  = newId[nfa.start];
    nfa.accept = newId[nfa.accept];
}


// ============================================================
// 12. DISPLAY NFA TRANSITION TABLE
// ============================================================

void displayTransitionTable(int start, int accept)
{
    cout << "\n";
    cout << "=============================================\n";
    cout << "          NFA TRANSITION TABLE\n";
    cout << "=============================================\n";

    cout << "\n";
    cout << "Start State : q" << start << endl;
    cout << "Final State : q" << accept << endl;

    cout << "\n";
    cout << "------------------------------------------------\n";
    cout << " State\t\t Input\t\tNext State\n";
    cout << "------------------------------------------------\n";

    for (const auto& t : transitions)
    {
        cout << " q" << t.from
             << "\t\t " << (t.symbol == '#' ? "eps" : string(1, t.symbol))
             << "\t\t q" << t.to << endl;
    }

    cout << "------------------------------------------------\n";
}


// ============================================================
// 13. DISPLAY TABLE IN MATRIX STYLE
//
// Generalized to work for ANY set of input symbols that actually
// appear in the regex (not just hardcoded 'a' and 'b'), plus the
// epsilon column.
// ============================================================

void displayMatrixTable(int start, int accept)
{
    cout << "\n";
    cout << "=============================================\n";
    cout << "       NFA TRANSITION TABLE (MATRIX)\n";
    cout << "=============================================\n\n";

    // Collect every distinct real input symbol used (excluding epsilon)
    set<char> symbols;
    for (const auto& t : transitions)
    {
        if (t.symbol != '#')
            symbols.insert(t.symbol);
    }

    cout << "State\t\t";
    for (char sym : symbols)
        cout << sym << "\t\t";
    cout << "#\n";
    cout << "----------------------------------------------------------\n";

    for (int state = 0; state < stateCount; state++)
    {
        cout << "q" << state;

        if (state == start)
            cout << "(S)";

        if (state == accept)
            cout << "(F)";

        cout << "\t\t";

        auto printColumn = [&](char sym)
        {
            set<int> reach;

            for (const auto& t : transitions)
            {
                if (t.from == state && t.symbol == sym)
                    reach.insert(t.to);
            }

            if (reach.empty())
            {
                cout << "-";
            }
            else
            {
                cout << "{";
                bool first = true;

                for (int x : reach)
                {
                    if (!first)
                        cout << ",";
                    cout << "q" << x;
                    first = false;
                }

                cout << "}";
            }

            cout << "\t\t";
        };

        for (char sym : symbols)
            printColumn(sym);

        printColumn('#');

        cout << endl;
    }

    cout << "----------------------------------------------------------\n";
}


// ============================================================
// 13b. ASCII STATE DIAGRAM
// ============================================================

void printASCIIDiagram(int start, int accept)
{
    cout << "\n";
    cout << "=============================================\n";
    cout << "          NFA STATE DIAGRAM (ASCII)\n";
    cout << "=============================================\n\n";

    for (int s = 0; s < stateCount; s++)
    {
        cout << "(q" << s << ")";

        if (s == start)
            cout << "  <-- START";

        if (s == accept)
            cout << "  <-- FINAL (double circle)";

        cout << "\n";

        for (const auto& t : transitions)
        {
            if (t.from == s)
            {
                string sym = (t.symbol == '#') ? "eps" : string(1, t.symbol);
                cout << "    ---" << sym << "---> q" << t.to << "\n";
            }
        }
    }

    cout << "\n=============================================\n";
}


// ============================================================
// 13c. GRAPHVIZ DOT EXPORT
//
// Writes a .dot file that can be rendered into an actual picture:
//     dot -Tpng nfa.dot -o nfa.png
// or pasted into an online viewer such as
//     https://dreampuf.github.io/GraphvizOnline/
// ============================================================

void exportGraphvizDOT(int start, int accept, const string& filename = "nfa.dot")
{
    ofstream out(filename);

    if (!out.is_open())
    {
        cout << "\n(Could not write " << filename << ")\n";
        return;
    }

    out << "digraph NFA {\n";
    out << "    rankdir=LR;\n";
    out << "    \"__start__\" [shape=point];\n";
    out << "    \"__start__\" -> \"q" << start << "\";\n";
    out << "    node [shape = doublecircle]; \"q" << accept << "\";\n";
    out << "    node [shape = circle];\n";

    for (const auto& t : transitions)
    {
        string sym = (t.symbol == '#') ? "&#949;" : string(1, t.symbol);
        out << "    \"q" << t.from << "\" -> \"q" << t.to
            << "\" [label=\"" << sym << "\"];\n";
    }

    out << "}\n";
    out.close();

    cout << "\nGraphviz DOT diagram written to: " << filename << "\n";
    cout << "Render it with:   dot -Tpng " << filename << " -o nfa.png\n";
    cout << "Or paste its contents into: https://dreampuf.github.io/GraphvizOnline/\n";
}


// ============================================================
// 14. #-CLOSURE
// ============================================================

set<int> epsilonClosure(const set<int>& states)
{
    set<int> closure = states;

    stack<int> st;

    for (int state : states)
        st.push(state);

    while (!st.empty())
    {
        int current = st.top();
        st.pop();

        for (const auto& t : transitions)
        {
            if (t.from == current && t.symbol == '#')
            {
                if (closure.find(t.to) == closure.end())
                {
                    closure.insert(t.to);
                    st.push(t.to);
                }
            }
        }
    }

    return closure;
}


// ============================================================
// 15. MOVE
// ============================================================

set<int> moveStates(const set<int>& states, char symbol)
{
    set<int> result;

    for (int state : states)
    {
        for (const auto& t : transitions)
        {
            if (t.from == state && t.symbol == symbol)
            {
                result.insert(t.to);
            }
        }
    }

    return result;
}


// ============================================================
// 16. SIMULATE NFA
// ============================================================

bool simulateNFA(const string& input, int start, int accept)
{
    set<int> currentStates;

    currentStates.insert(start);

    // Initial #-closure
    currentStates = epsilonClosure(currentStates);


    for (char symbol : input)
    {
        // Ignore invalid symbols
        if (!isOperand(symbol))
            return false;

        set<int> nextStates;

        nextStates = moveStates(currentStates, symbol);

        // Follow #-transitions
        nextStates = epsilonClosure(nextStates);

        currentStates = nextStates;

        if (currentStates.empty())
            return false;
    }

    return currentStates.find(accept) != currentStates.end();
}


void printStates(const set<int>& states)
{
    cout << "{";

    bool first = true;

    for (int state : states)
    {
        if (!first)
            cout << ", ";

        cout << "q" << state;

        first = false;
    }

    cout << "}";
}



int main()
{
    string regex;
    cout << "REGULAR EXPRESSION TO NFA\n";
    cout << "Supported operators:\n";
    cout << "  |  = Union / OR\n";
    cout << "  .  = Concatenation\n";
    cout << "  *  = Kleene Star        (zero or more)\n";
    cout << "  +  = Positive Closure   (one or more)\n";
    cout << "  ?  = Optional           (zero or one)\n";
    cout << "  ( ) = Parentheses\n\n";
    cout << "Enter Regular Expression: ";
    getline(cin, regex);
    regex = removeSpaces(regex);
    cout << "\nOriginal RE:\n";
    cout << regex << endl;
    string explicitRE = addConcatenation(regex);
    cout << "\nRE with explicit concatenation:\n";
    cout << explicitRE << endl;
    string postfix = infixToPostfix(explicitRE);
    cout << "\nPostfix:\n";
    cout << postfix << endl;
    string prefix = infixToPrefix(explicitRE);
    cout << "\nPrefix:\n";
    cout << prefix << endl;
    transitions.clear();
    stateCount = 0;
    Fragment nfa = buildNFA(postfix);

    // Force start state to always be q0, in BFS discovery order
    renumberStates(nfa);

    displayTransitionTable(nfa.start, nfa.accept);
    displayMatrixTable(nfa.start, nfa.accept);
    printASCIIDiagram(nfa.start, nfa.accept);
    exportGraphvizDOT(nfa.start, nfa.accept);

    int numberOfStrings;
    cout << "\nHow many strings do you want to test? ";
    cin >> numberOfStrings;
    cin.ignore();
    for (int i = 1; i <= numberOfStrings; i++)
    {
        string input;
        cout << "\nString " << i << ": ";
        getline(cin, input);
        if (input.empty())
            input = "";
        bool accepted = simulateNFA(input, nfa.start, nfa.accept);
        if (accepted)
            cout << "Result: ACCEPTED\n";
        else
            cout << "Result: REJECTED\n";
    }
    return 0;
}