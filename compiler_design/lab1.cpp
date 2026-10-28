#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

struct Symbol
{
    int index;
    string name;
};

vector<Symbol> symbolTable;

string keywords[] =
    {
     "auto",
     "break",
     "case",
     "char",
     "const",
     "continue",
     "default",
     "do",
     "double",
     "else",
     "enum",
     "extern",
     "float",
     "for",
     "goto",
     "if",
     "int",
     "long",
     "register",
     "return",
     "short",
     "signed",
     "sizeof",
     "static",
     "struct",
     "switch",
     "typedef",
     "union",
     "unsigned",
     "void",
     "volatile",
     "while",
     "cout",
     "printf"

};

const int KEYWORD_COUNT = sizeof(keywords) / sizeof(keywords[0]);

bool isKeyword(const string &word)
{
    for (int i = 0; i < KEYWORD_COUNT; i++)
    {
        if (keywords[i] == word)
            return true;
    }

    return false;
}

int addToSymbolTable(const string &name)
{
    for (const auto &symbol : symbolTable)
    {
        if (symbol.name == name)
            return symbol.index;
    }

    int newIndex = symbolTable.size() + 1;

    symbolTable.push_back({newIndex, name});

    return newIndex;
}

bool isIdentifierStart(char c)
{
    return isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool isIdentifierPart(char c)
{
    return isalnum(static_cast<unsigned char>(c)) || c == '_';
}

string removeComments(const string &code)
{
    string result;

    bool inString = false;
    bool inChar = false;

    for (size_t i = 0; i < code.length(); i++)
    {

        if (code[i] == '"' && !inChar)
        {
            inString = !inString;
            result += code[i];
            continue;
        }

        if (code[i] == '\'' && !inString)
        {
            inChar = !inChar;
            result += code[i];
            continue;
        }

        if (inString || inChar)
        {
            result += code[i];

            if (code[i] == '\\' && i + 1 < code.length())
            {
                i++;
                result += code[i];
            }

            continue;
        }

        if (code[i] == '/' &&
            i + 1 < code.length() &&
            code[i + 1] == '/')
        {
            i += 2;

            while (i < code.length() && code[i] != '\n')
            {
                i++;
            }

            result += ' ';

            continue;
        }

        if (code[i] == '/' &&
            i + 1 < code.length() &&
            code[i + 1] == '*')
        {
            i += 2;

            while (i + 1 < code.length())
            {
                if (code[i] == '*' && code[i + 1] == '/')
                {
                    i++;
                    break;
                }

                i++;
            }

            result += ' ';

            continue;
        }

        result += code[i];
    }

    return result;
}

string createCleanCode(const string &code)
{
    string cleanCode;

    bool inString = false;
    bool inChar = false;

    for (size_t i = 0; i < code.length(); i++)
    {
        char c = code[i];

        if (c == '"' && !inChar)
        {
            inString = !inString;
            cleanCode += c;
            continue;
        }

        if (c == '\'' && !inString)
        {
            inChar = !inChar;
            cleanCode += c;
            continue;
        }

        if (inString || inChar)
        {
            cleanCode += c;

            if (c == '\\' && i + 1 < code.length())
            {
                i++;
                cleanCode += code[i];
            }

            continue;
        }

        if (isspace(static_cast<unsigned char>(c)))
        {
            size_t j = i + 1;

            while (j < code.length() &&
                   isspace(static_cast<unsigned char>(code[j])))
            {
                j++;
            }

            if (!cleanCode.empty() &&
                j < code.length() &&
                (isalnum(static_cast<unsigned char>(cleanCode.back())) ||
                 cleanCode.back() == '_') &&
                (isalnum(static_cast<unsigned char>(code[j])) ||
                 code[j] == '_'))
            {
                cleanCode += ' ';
            }

            i = j - 1;
            continue;
        }

        cleanCode += c;
    }

    return cleanCode;
}

void lexicalAnalyzer(const string &code, ofstream &output)
{
    int i = 0;

    while (i < static_cast<int>(code.length()))
    {

        if (isspace(static_cast<unsigned char>(code[i])))
        {
            i++;
            continue;
        }

        if (code[i] == '#')
        {
            string preprocessor;

            while (i < static_cast<int>(code.length()) &&
                   code[i] != '<' &&
                   code[i] != ';' &&
                   !isspace(static_cast<unsigned char>(code[i])))
            {
                preprocessor += code[i];
                i++;
            }

            output << "<PREPROCESSOR," << preprocessor << ">\n";

            /* -----------------------------------------
               Skip whitespace between directive and header // bujhi nai
               ----------------------------------------- */

            while (i < static_cast<int>(code.length()) &&
                   isspace(static_cast<unsigned char>(code[i])))
            {
                i++;
            }

            /* -----------------------------------------
               Header file
               ----------------------------------------- */

            if (i < static_cast<int>(code.length()) &&
                code[i] == '<')
            {
                string header = "<";
                i++;

                while (i < static_cast<int>(code.length()) &&
                       code[i] != '>')
                {
                    header += code[i];
                    i++;
                }

                if (i < static_cast<int>(code.length()) &&
                    code[i] == '>')
                {
                    header += '>';
                    i++;
                }

                output << "<HEADER," << header << ">\n";
            }

            continue;
        }

        if (isIdentifierStart(code[i]))
        {
            string word;

            while (i < static_cast<int>(code.length()) &&
                   isIdentifierPart(code[i]))
            {
                word += code[i];
                i++;
            }

            if (isKeyword(word))
            {
                output << "<KEYWORD," << word << ">\n";
            }

            else
            {
                int index = addToSymbolTable(word);

                output << "<ID," << index << ">\n";
            }

            continue;
        }

        if (isdigit(static_cast<unsigned char>(code[i])))
        {
            string number;

            bool decimalPoint = false;

            while (i < static_cast<int>(code.length()))
            {
                if (isdigit(static_cast<unsigned char>(code[i])))
                {
                    number += code[i];
                    i++;
                }
                else if (code[i] == '.' && !decimalPoint)
                {
                    decimalPoint = true;
                    number += code[i];
                    i++;
                }
                else
                {
                    break;
                }
            }

            if (decimalPoint)
                output << "<FLOAT," << number << ">\n";
            else
                output << "<NUMBER," << number << ">\n";

            continue;
        }

        if (code[i] == '"')
        {
            string str;

            str += code[i];
            i++;

            while (i < static_cast<int>(code.length()))
            {
                str += code[i];

                if (code[i] == '\\' &&
                    i + 1 < static_cast<int>(code.length()))
                {
                    i++;
                    str += code[i];
                }
                else if (code[i] == '"')
                {
                    i++;
                    break;
                }

                i++;
            }

            output << "<STRING_LITERAL," << str << ">\n";

            continue;
        }

        if (code[i] == '\'')
        {
            string character;

            character += code[i];
            i++;

            while (i < static_cast<int>(code.length()))
            {
                character += code[i];

                if (code[i] == '\\' &&
                    i + 1 < static_cast<int>(code.length()))
                {
                    i++;
                    character += code[i];
                }
                else if (code[i] == '\'')
                {
                    i++;
                    break;
                }

                i++;
            }

            output << "<CHAR_LITERAL," << character << ">\n";

            continue;
        }

        if (i + 1 < static_cast<int>(code.length()))
        {
            string twoCharOperator =
                code.substr(i, 2);

            /* Relational operators */

            if (twoCharOperator == "==" ||
                twoCharOperator == "!=" ||
                twoCharOperator == "<=" ||
                twoCharOperator == ">=")
            {
                output << "(RELATIONAL_OP,"
                       << twoCharOperator
                       << ")\n";

                i += 2;
                continue;
            }

            if (twoCharOperator == "&&" ||
                twoCharOperator == "||")
            {
                output << "(LOGICAL_OP,"
                       << twoCharOperator
                       << ")\n";

                i += 2;
                continue;
            }

            if (twoCharOperator == "++" ||
                twoCharOperator == "--")
            {
                output << "(INC_DEC_OP,"
                       << twoCharOperator
                       << ")\n";

                i += 2;
                continue;
            }

            if (twoCharOperator == "+=" ||
                twoCharOperator == "-=" ||
                twoCharOperator == "*=" ||
                twoCharOperator == "/=" ||
                twoCharOperator == "%=")
            {
                output << "<ASSIGN_OP,"
                       << twoCharOperator
                       << ">\n";

                i += 2;
                continue;
            }
        }

        switch (code[i])
        {

        case '+':
        case '-':
        case '*':
        case '/':
        case '%':

            output << "<ARITHMETIC_OP,"
                   << code[i]
                   << ">\n";

            i++;
            break;

        case '=':

            output << "<ASSIGN_OP,=>\n";

            i++;
            break;

        case '<':
        case '>':

            output << "<RELATIONAL_OP,"
                   << code[i]
                   << ">\n";

            i++;
            break;

        case '!':

            output << "<LOGICAL_OP,!>\n";

            i++;
            break;

        case '&':

            output << "<ADDRESS_OP,&>\n";

            i++;
            break;

        case '(':

            output << "<LPAREN,(>\n";

            i++;
            break;

        case ')':

            output << "<RPAREN,)>\n";

            i++;
            break;

        case '{':

            output << "<LBRACE,{>\n";

            i++;
            break;

        case '}':

            output << "<RBRACE,}>\n";

            i++;
            break;

        case '[':

            output << "<LBRACKET,[>\n";

            i++;
            break;

        case ']':

            output << "<RBRACKET,]>\n";

            i++;
            break;

        case ';':

            output << "<SEMICOLON,;>\n";

            i++;
            break;

        case ',':

            output << "<COMMA,,>\n";

            i++;
            break;

        case '.':

            output << "<DOT,.>\n";

            i++;
            break;

        default:

            output << "<UNKNOWN,"
                   << code[i]
                   << ">\n";

            i++;
            break;
        }
    }
}

void printSymbolTable(ofstream &output)
{
    output << "\n";
    output << "----------SYMBOL TABLE\n";

    output << "Index-Identifier-Token\n";


    for (const auto &symbol : symbolTable)
    {
        output << symbol.index
               << "\t"
               << symbol.name
               << "\t\tID\n";
    }


    output << "Total Symbols: "
           << symbolTable.size()
           << "\n";
}

int main()
{
    ifstream inputFile("a.txt");

    if (!inputFile)
    {
        cout << "Error: Cannot open a.txt\n";
        return 1;
    }

    string rawCode(
        (istreambuf_iterator<char>(inputFile)),
        istreambuf_iterator<char>());

    inputFile.close();

    string codeWithoutComments =
        removeComments(rawCode);

    string cleanCode =
        createCleanCode(codeWithoutComments);
    cout << cleanCode << endl;

    ofstream outputFile("output.txt");

    if (!outputFile)
    {
        cout << "Error: Cannot create output.txt\n";
        return 1;
    }

    outputFile << "----------RAW CODE\n";


    outputFile << rawCode;

    outputFile << "\n\n";

    outputFile << "----------CLEAN CODE\n";

    outputFile << cleanCode;

    outputFile << "\n\n";

    outputFile << "--------TOKENS\n";

    lexicalAnalyzer(codeWithoutComments, outputFile);

    printSymbolTable(outputFile);

    outputFile.close();

    cout << "Lexical analysis completed successfully.\n";
    cout << "Generated file: output.txt\n";

    return 0;
}