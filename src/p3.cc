// p3.c

#include "p3.h"
#include <iostream>
#include <set>

using namespace std;

bool DEBUG = false;

set<TokenType> FIRST_IDLIST = set<TokenType>{ID};
set<TokenType> FIRST_NUMLIST = set<TokenType>{NUM};
set<TokenType> FIRST_INPUT = set<TokenType>{INPUT};
set<TokenType> FIRST_OUTPUT = set<TokenType>{OUTPUT};
set<TokenType> FIRST_ASSIGN = set<TokenType>{ID};
set<TokenType> FIRST_STMTLIST = set<TokenType>{INPUT, OUTPUT, ID};
set<TokenType> FIRST_PRIMARY = set<TokenType>{ID, NUM};
set<TokenType> FIRST_OP = set<TokenType>{PLUS, MINUS, MULT, DIV};

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


bool FIRST(TokenType tt, set<TokenType> first){
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
        cout << "Unexpected Token: " << t.lexeme << ", " << t.token_type <<endl;
        cout << "expected: " << expected_type <<endl;
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
    if (FIRST(ttype(), FIRST_STMTLIST)){
        iNode->next = parseStmtList();
    }
    return iNode;
}

InstructionNode* P3::parseStmt() {
    InstructionNode* iNode;
    if (FIRST(ttype(), FIRST_INPUT)){
        iNode = parseInputStmt();
    } else if (FIRST(ttype(), FIRST_OUTPUT)){
        iNode = parseOutputStmt();
    } else if (FIRST(ttype(), FIRST_ASSIGN)) {
        iNode = parseAssignStmt();
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

InstructionNode* P3::parseAssignStmt(){
    /*
    assign stmt → ID EQUAL primary SEMICOLON |
                  ID EQUAL expr SEMICOLON
    */
    string id = expect(ID).lexeme;
    expect(EQUAL);

    int op1Loc = parsePrimary();
    int op2Loc;
    ArithmeticOperatorType op = OPERATOR_NONE;
    
    if (FIRST(ttype(), FIRST_OP)) {
        op = parseOperator();
        op2Loc = parsePrimary();
    }

    expect(SEMICOLON);

    InstructionNode* iNode = new InstructionNode;
    iNode->type = ASSIGN;
    iNode->assign_inst.lhs_loc = this->varLocs.at(id);
    iNode->assign_inst.op1_loc = op1Loc;
    iNode->assign_inst.op = op;
    iNode->assign_inst.op2_loc = op2Loc;
    iNode->next = nullptr;
    return iNode;
}

int P3::parsePrimary() {
    Token t = lexer.GetToken();

    if (t.token_type == ID) {
        return this->varLocs.at(t.lexeme);
    } else if (t.token_type == NUM) {
        int constLoc = storeValue(stoi(t.lexeme));
        this->varLocs.insert({t.lexeme, constLoc});
        return constLoc;
    } else {
        syntax_error();
    }

    return -1;
}

ArithmeticOperatorType P3::parseOperator() {
    Token t = lexer.GetToken();
    map<TokenType, ArithmeticOperatorType> opMap = map<TokenType, ArithmeticOperatorType>{
        {PLUS, OPERATOR_PLUS},
        {MINUS, OPERATOR_MINUS},
        {MULT, OPERATOR_MULT},
        {DIV, OPERATOR_DIV},
    };
    return opMap.at(t.token_type);
}

NumList P3::parseInputs() {
    return parseNumList();
}

NumList P3::parseNumList(){
    Token t = expect(NUM);

    vector<int> nums = vector<int>{stoi(t.lexeme)};
    NumList numL = NumList{nums};

    if (FIRST(ttype(), FIRST_NUMLIST)) {
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
