#ifndef P3_H
#define P3_H

#include "execute.h"
#include "lexer.h"
#include <string>
#include <map>

using namespace std;

struct IDList {
    vector<string> ids;
    IDList(vector<string> ids) : ids(ids) {};
    IDList() : ids(vector<string>{}) {};
};

struct NumList {
    vector<int> nums;
    NumList(vector<int> nums) : nums(nums) {};
    NumList() : nums(vector<int>{}) {};
};

struct Body {

};

class P3 {
    public:
        LexicalAnalyzer lexer;
        map<string, int> varLocs;

        Token expect(TokenType expected_type);
        void syntax_error();
        TokenType ttype();
        int storeValue(int val);
        int location(string varName);


        InstructionNode* parseProgram();
        
        IDList parseVarSection();
        IDList parseIDList();

        InstructionNode* parseBody();
        
        InstructionNode* parseStmtList();
        InstructionNode* parseStmt();


        InstructionNode* parseInputStmt();
        InstructionNode* parseOutputStmt();

        NumList parseInputs();
        NumList parseNumList();
};

#endif