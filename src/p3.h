#ifndef P3_H
#define P3_H

#include "execute.h"
#include "lexer.h"
#include <string>

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
        Token expect(TokenType expected_type);
        void syntax_error();
        TokenType ttype();
        int storeValue(int val);

        InstructionNode* parseProgram();
        
        IDList parseVarSection();
        IDList parseIDList();

        void parseBody();
        void parseStmtList();
        void parseStmt();


        void parseInputStmt();

        NumList parseInputs();
        NumList parseNumList();
};

#endif