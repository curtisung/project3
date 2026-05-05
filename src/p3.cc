// p3.c

#include "p3.h"
#include <iostream>
#include <set>

using namespace std;

bool DEBUG = true;

void printIntList(vector<int> list) {
    if (DEBUG) {
        for (auto i : list) {
            cout << i << " ";
        }
        cout << endl;
    }
}

void printStrList(vector<string> list) {
    if (DEBUG) {
        for (auto i : list) {
            cout << i << " ";
        }
        cout << endl;
    }
}

bool FIRST_IDList(TokenType tt) {
    set<TokenType> first = {ID};
    return first.count(tt) == 1;
}

bool FIRST_numList(TokenType tt) {
    set<TokenType> first = {NUM};
    return first.count(tt) == 1;
}

// TODO: update with each new statement
bool FIRST_stmtList(TokenType tt) {
    set<TokenType> first = {INPUT};
    return first.count(tt) == 1;
}

/* Parser Helpers */

TokenType P3::ttype(){
    return lexer.peek(1).token_type;
}

Token P3::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type) {
        syntax_error();
    }
    return t;
}

void P3::syntax_error() {
    cout<< "SYNTAX ERROR !!!!!!!!!!!!!!\n";
    exit(1);
}

int P3::storeValue(int val){
    mem[next_available] = val;
    return next_available++;
}

/* end Parser Helpers */


InstructionNode* P3::parseProgram(){
    IDList idL = parseVarSection();
    printStrList(idL.ids);

    parseBody();
    NumList nL = parseInputs();
    printIntList(nL.nums);

    return new InstructionNode;
}

IDList P3::parseVarSection(){
    IDList idL = parseIDList();
    expect(SEMICOLON);
    return idL;
}

IDList P3::parseIDList() {
    Token t = expect(ID);
    IDList idL = IDList{vector<string>{t.lexeme}};
    if (ttype() == COMMA) {
        expect(COMMA);
        vector<string> moreIds = parseIDList().ids;
        idL.ids.insert(idL.ids.end(), 
                moreIds.begin(), moreIds.end());
    }
    return idL;
}

void P3::parseBody(){
    expect(LBRACE);
    parseStmtList();
    expect(RBRACE);
}

void P3::parseStmtList(){
    parseStmt();
    if (FIRST_stmtList(ttype())){
        parseStmtList();
    }
}

void P3::parseStmt() {
    parseInputStmt();
}

void P3::parseInputStmt(){
    expect(INPUT);
    Token t = expect(ID);
    int loc = storeValue(0);
    cout << loc << endl;
    expect(SEMICOLON);
}

NumList P3::parseInputs() {
    return parseNumList();
}

NumList P3::parseNumList(){
    Token t = expect(NUM);

    vector<int> nums = vector<int>{stoi(t.lexeme)};
    NumList numL = NumList{nums};

    if (FIRST_numList(ttype())) {
        vector<int> moreNums = parseNumList().nums;
        numL.nums.insert(numL.nums.end(), 
                    moreNums.begin(), moreNums.end());
    }
    return numL;
}


struct InstructionNode *parse_Generate_Intermediate_Representation(){
    P3 p3 = P3();
    InstructionNode* iNode = p3.parseProgram();

    return iNode;
}
