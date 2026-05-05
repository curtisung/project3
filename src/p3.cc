// p3.c

#include "p3.h"
#include <iostream>
#include <set>

using namespace std;

bool DEBUG = false;

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

bool FIRST_inputStmt(TokenType tt){
    set<TokenType> first = {INPUT};
    return first.count(tt) == 1;
}

bool FIRST_outputStmt(TokenType tt){
    set<TokenType> first = {OUTPUT};
    return first.count(tt) == 1;
}
// TODO: update with each new statement
bool FIRST_stmtList(TokenType tt) {
    set<TokenType> first = {INPUT, OUTPUT};
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

int P3::location(string varName){
    return this->varLocs.at(varName);
}

/* end Parser Helpers */


InstructionNode* P3::parseProgram(){
    IDList idL = parseVarSection();
    printStrList(idL.ids);

    for (string var : idL.ids){
        int location = storeValue(0);
        this->varLocs.insert({var,location});
    }
    
    InstructionNode* iNode = parseBody();
    
    NumList nL = parseInputs();
    for (int inp : nL.nums) {
        inputs.push_back(inp);
    }
    
    printIntList(nL.nums);

    return iNode;
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

InstructionNode* P3::parseBody(){
    expect(LBRACE);
    InstructionNode* iNode = parseStmtList();
    expect(RBRACE);
    return iNode;
}

InstructionNode* P3::parseStmtList(){
    InstructionNode* iNode = parseStmt();
    if (FIRST_stmtList(ttype())){
        iNode->next = parseStmtList();
    }
    return iNode;
}

InstructionNode* P3::parseStmt() {
    InstructionNode* iNode;
    if (FIRST_inputStmt(ttype())){
        iNode = parseInputStmt();
    } else if (FIRST_outputStmt(ttype())){
        iNode = parseOutputStmt();
    }
    return iNode;
}

InstructionNode* P3::parseInputStmt(){
    expect(INPUT);

    Token t = expect(ID);
    InstructionNode* node = new InstructionNode;
    node->type = IN;
    node->input_inst.var_loc = this->varLocs.at(t.lexeme);
    node->next = nullptr;
    // cout << node->input_inst.var_loc << endl;
    
    expect(SEMICOLON);
    return node;
}

InstructionNode* P3::parseOutputStmt(){
    expect(OUTPUT);

    Token t = expect(ID);
    InstructionNode* node = new InstructionNode;
    node->type = OUT;
    node->output_inst.var_loc = this->varLocs.at(t.lexeme);
    node->next = nullptr;
    // cout << node->input_inst.var_loc << endl;
    
    expect(SEMICOLON);
    return node;
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
