//
// Created by Carlos A. Fernández on 02/11/2016.
// Semantic Analyzer.h on 05:13 PM 
// Copyright (c) 2016 TK team. All rights reserved.
//

#ifndef PARSER_TREE_PR3_SEMANTIC_ANALYZER_H
#define PARSER_TREE_PR3_SEMANTIC_ANALYZER_H
#include "SyntacticAnalyzer.h"

class SemanticAnalyzer{
private:
    SyntacticAnalyzer* syntacticAnalyzer;
    SymbolTable* table;
    std::vector<Expression*> tree;
public:
    SemanticAnalyzer(SyntacticAnalyzer* syntacticAnalyzer){
        this->syntacticAnalyzer=syntacticAnalyzer;
        table=new SymbolTable();
    }
    int analyze(){
        tree=syntacticAnalyzer->analyze();
        Expression *expresion;
        int result;
        for(int i=0,j=tree.size();i!=j;i++){
            expresion=tree.at(i);
            expresion->semantic_validation(*table);
        }
        for(std::vector<Expression*>::iterator i=tree.begin();i!=tree.end();++i){
            expresion=*i;
            result = expresion->symbol->getSemanticType()!=ERROR?1:0;
        }
        return result;
    }
    std::unordered_map<std::string,int> get_map(){
        return table->get_map();
    }
    std::vector<Expression*> get_tree(){
        return  tree;
    }
};

#endif //PARSER_TREE_PR3_SEMANTIC_ANALYZER_H
