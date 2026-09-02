#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

/*
    REGULAR EXPRESSION → NFA

    Supported operators:
        |   = Union / OR
        .   = Concatenation
        *   = Kleene Star
        ( ) = Parentheses

    The program performs:
        1. Remove spaces
        2. Add explicit concatenation '.'
        3. Infix → Postfix
        4. Infix → Prefix
        5. Postfix → Thompson NFA
        6. Display NFA transition table
        7. Simulate NFA on input strings
*/


bool isOperand(char c)
{
    return isalnum(c);
}

int precedence(char c)
{
    if (c == '*')
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
    return isOperand(c) || c == ')' || c == '*';
}

bool isRightConcat(char c)
{
    return isOperand(c) || c == '(';
}

/*
    ab       → a.b
    a(b|c)   → a.(b|c)
    (a|b)c   → (a|b).c
    a*b      → a*.b
*/
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

/*
    a.(b|c)
    ↓
    abc|.
*/
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
                '*' is postfix unary operator.

                Example:
                    a*
                We can immediately append it after processing a.
            */

            if (c == '*')
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
//     *       → Kleene star
// ============================================================

Fragment buildNFA(const string& postfix)
{
    stack<Fragment> st;

    // a → b → | → * → a → . → b → .

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
        // Suppose:
        //
        // A:  0 --a--> 1
        // B:  2 --b--> 3
        //
        // Create new start = 4
        // Create new accept = 5
        //
        // Result:
        //
        //              ε → A → ε
        //             /         \
        //        4 --             --> 5
        //             \         /
        //              ε → B → ε
        //
        // This means:
        // From 4 we can choose A OR B.

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
        // CASE 4: KLEENE STAR '*'
        // ====================================================
        //
        // Example:
        // A*
        //
        // A means one occurrence.
        //
        // A* allows:
        //
        // ε        → zero A
        // A        → one A
        // AA       → two A's
        // AAA      → three A's
        // ...
        //
        // Suppose:
        //
        // A = 4 ... 5
        //
        // Create:
        // new start  = 6
        // new accept = 7
        //
        // Structure:
        //
        //              ε
        //          ┌────────→ 7
        //          │
        //        6 ──ε──→ 4 ... 5
        //                      │
        //                      ├──ε──→ 7
        //                      │
        //                      └──ε──→ 4
        //
        // The last transition back to 4 creates the loop.

        else if (c == '*')
        {
            // Get the fragment A
            Fragment A = st.top();
            st.pop();

            // Create new start and accept states
            int start = newState();
            int accept = newState();

            // ------------------------------------------------
            // Zero repetitions
            // ------------------------------------------------
            //
            // start --ε--> accept
            //
            // This allows A* to match ε.

            addTransition(start, '#', accept);

            // ------------------------------------------------
            // Enter A
            // ------------------------------------------------
            //
            // start --ε--> A.start
            //
            // This allows us to execute A.

            addTransition(start, '#', A.start);

            // ------------------------------------------------
            // Finish A
            // ------------------------------------------------
            //
            // A.accept --ε--> accept
            //
            // This allows us to finish after one execution.

            addTransition(A.accept, '#', accept);

            // ------------------------------------------------
            // Repeat A
            // ------------------------------------------------
            //
            // A.accept --ε--> A.start
            //
            // This creates the loop for:
            // AA, AAA, AAAA, ...

            addTransition(A.accept, '#', A.start);

            // Push the complete A* fragment
            st.push({start, accept});
        }
    }

    // After processing the complete postfix expression,
    // only ONE complete NFA fragment should remain.
    //
    // Example:
    // (a|b)*abb
    //
    // Final:
    //
    // start = 6
    // accept = 13
    //
    // Return that complete fragment.

    return st.top();
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
             << "\t\t " << t.symbol
             << "\t\t q" << t.to << endl;
    }

    cout << "------------------------------------------------\n";
}


// ============================================================
// 13. DISPLAY TABLE IN MATRIX STYLE
// ============================================================

void displayMatrixTable(int start, int accept)
{
    cout << "\n";
    cout << "=============================================\n";
    cout << "       NFA TRANSITION TABLE (MATRIX)\n";
    cout << "=============================================\n\n";

    cout << "State\t\t a\t\t b\t\t #\n";
    cout << "----------------------------------------------------------\n";

    for (int state = 0; state < stateCount; state++)
    {
        cout << "q" << state;

        if (state == start)
            cout << "(S)";

        if (state == accept)
            cout << "(F)";

        cout << "\t\t";

        // a transitions
        set<int> aStates;

        for (const auto& t : transitions)
        {
            if (t.from == state && t.symbol == 'a')
                aStates.insert(t.to);
        }

        if (aStates.empty())
            cout << "-";
        else
        {
            cout << "{";
            bool first = true;

            for (int x : aStates)
            {
                if (!first)
                    cout << ",";
                cout << "q" << x;
                first = false;
            }

            cout << "}";
        }

        cout << "\t\t";


        // b transitions
        set<int> bStates;

        for (const auto& t : transitions)
        {
            if (t.from == state && t.symbol == 'b')
                bStates.insert(t.to);
        }

        if (bStates.empty())
            cout << "-";
        else
        {
            cout << "{";
            bool first = true;

            for (int x : bStates)
            {
                if (!first)
                    cout << ",";
                cout << "q" << x;
                first = false;
            }

            cout << "}";
        }

        cout << "\t\t";


        // epsilon transitions
        set<int> epsilonStates;

        for (const auto& t : transitions)
        {
            if (t.from == state && t.symbol == '#')
                epsilonStates.insert(t.to);
        }

        if (epsilonStates.empty())
            cout << "-";
        else
        {
            cout << "{";
            bool first = true;

            for (int x : epsilonStates)
            {
                if (!first)
                    cout << ",";
                cout << "q" << x;
                first = false;
            }

            cout << "}";
        }

        cout << endl;
    }

    cout << "----------------------------------------------------------\n";
}


// ============================================================
// 14. #-CLOSURE
//
// Given a set of states:
//
//     {q1, q3}
//
// follow all possible # transitions.
//
// Example:
//
//     q1 --#--> q2
//     q2 --#--> q4
//
// #-closure(q1) = {q1,q2,q4}
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
//
// From a set of states, consume one input symbol.
//
// Example:
//
//     current = {q1,q2}
//     symbol = a
//
// Find all states reachable using 'a'.
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
    cout << "  *  = Kleene Star\n";
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
    displayTransitionTable(nfa.start, nfa.accept);
    displayMatrixTable(nfa.start, nfa.accept);
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