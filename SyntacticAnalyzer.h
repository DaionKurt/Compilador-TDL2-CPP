//
// Created by Carlos A. Fernández on 04/09/2016.
// SyntacticAnalyzer.h on 09:50 AM 
// Copyright (c) 2016 TK team. All rights reserved.
//

#ifndef SYNTACTIC_ANALYZER_SYNTACTICANALYZER_H
#define SYNTACTIC_ANALYZER_SYNTACTICANALYZER_H
#include "LexycalAnalyzer.h"
#include "Expressions.h"
#include <vector>

class SyntacticAnalyzer{
private:
    LexycalAnalyzer* lexycalAnalyzer;
    std::vector<Expression*> tree;
    bool is_error;
    bool is_current_symbol(std::string symbol){
        return lexycalAnalyzer->get_current_symbol()->getContent().compare(symbol)==0;
    }
    bool set_lexycal_analyzer(LexycalAnalyzer* lexycalAnalyzer){
        if(lexycalAnalyzer!= nullptr) {
            this->lexycalAnalyzer = lexycalAnalyzer;
            return true;
        }else return false;
    }
    void check(std::string symbol){
        if((lexycalAnalyzer->get_current_symbol()->getContent().compare(symbol)==0)) lexycalAnalyzer->next_symbol();
        else is_error=true;
    }
    void S(std::vector<Expression*>& expressions){
        bool valid = false;
        if(lexycalAnalyzer->get_current_symbol()->getType()==IDENTIFIER){
            expressions.push_back(A());
            valid=true;
        }else if(lexycalAnalyzer->get_current_symbol()->getType()==WHILE){
            expressions.push_back(W());
            valid=true;
        }else if(lexycalAnalyzer->get_current_symbol()->getType()==IF){
            expressions.push_back(I());
            valid=true;
        }
        if(valid) S(expressions);
    }
    Expression* A(){
        Expression* temp = new Identifier(lexycalAnalyzer->get_current_symbol());
        lexycalAnalyzer->next_symbol();
        Symbol* symbol_node = lexycalAnalyzer->get_current_symbol();
        check("=");
        temp = new Assignment(temp, E(), symbol_node);
        if (lexycalAnalyzer->get_current_symbol()->getContent().compare(";") == 0){
            lexycalAnalyzer->next_symbol();
        }else{
            temp->is_print=true;
        }
        return temp;
    }
    Expression* W(){
        Block* block = new Block(false);
        Symbol* symbol = lexycalAnalyzer->get_current_symbol();
        lexycalAnalyzer->next_symbol();
        Iterative* temp = new Iterative(C(),block,symbol);
        while (lexycalAnalyzer->get_current_symbol()->getContent().compare("fin")!=0 and
               lexycalAnalyzer->get_current_symbol()->getContent().compare("$")!=0 and
               lexycalAnalyzer->get_current_symbol()->getContent().compare("")!=0)
            S(block->get_nodes());
        check("fin");
        return temp;
    }
    Expression* I(){
        Block* block = new Block(false);
        Symbol* symbol = lexycalAnalyzer->get_current_symbol();
        lexycalAnalyzer->next_symbol();
        Selective* temp = new Selective();
        temp->add_child(C());
        while(lexycalAnalyzer->get_current_symbol()->getContent().compare("fin")!=0 and
              lexycalAnalyzer->get_current_symbol()->getContent().compare("$")!=0 and
              lexycalAnalyzer->get_current_symbol()->getContent().compare("")!=0 and
              lexycalAnalyzer->get_current_symbol()->getType()!=ELSE)
            S(block->get_nodes());
        temp->add_child(block);
        temp->symbol=symbol;
        if(lexycalAnalyzer->get_current_symbol()->getType()==ELSE){
            Block* else_block = new Block(true);
            Symbol* symbol = lexycalAnalyzer->get_current_symbol();
            Expression* else_ = new Else(else_block,symbol);
            lexycalAnalyzer->next_symbol();
            while(lexycalAnalyzer->get_current_symbol()->getContent().compare("fin")!=0 and
                    lexycalAnalyzer->get_current_symbol()->getContent().compare("$")!=0 and
                    lexycalAnalyzer->get_current_symbol()->getContent().compare("")!=0)
                S(else_block->get_nodes());
            temp->add_child(else_);
        }
        check("fin");
        return temp;
    }
    Expression* C(){
        Expression* temp = E();
        if(lexycalAnalyzer->get_current_symbol()->getType()==RELATIONAL_OP){
            Symbol* symbol = lexycalAnalyzer->get_current_symbol();
            lexycalAnalyzer->next_symbol();
            temp = new Condition(temp,E(),symbol);
        }else is_error=true;
        return temp;
    }
    Expression* E(){
        Expression* temp = M();
        while(lexycalAnalyzer->get_current_symbol()->getType()==ADDITION_OP || lexycalAnalyzer->get_current_symbol()->getContent().compare("-")==0){
            Symbol* symbol_node=lexycalAnalyzer->get_current_symbol();
            lexycalAnalyzer->next_symbol();
            temp = new Sum(temp,M(),symbol_node);
        }
        return temp;
    }
    Expression* M(){
        Expression* temp = F();
        while(lexycalAnalyzer->get_current_symbol()->getType()==MULTIPLICATION_OP || lexycalAnalyzer->get_current_symbol()->getContent().compare("/")==0){
            Symbol* symbol_node=lexycalAnalyzer->get_current_symbol();
            lexycalAnalyzer->next_symbol();
            temp = new Multiplication(temp,F(),symbol_node);
        }
        return temp;
    }
    Expression* F(){
        Expression* temp = nullptr;
        if(lexycalAnalyzer->get_current_symbol()->getContent().compare("+")==0||lexycalAnalyzer->get_current_symbol()->getContent().compare("-")==0){
            Symbol* symbol= new Symbol();
            symbol->setContent(lexycalAnalyzer->get_current_symbol()->getContent());
            symbol->setType(SIGNE);
            lexycalAnalyzer->next_symbol();
            temp=new Signe(F(),symbol);
        }else if(lexycalAnalyzer->get_current_symbol()->getType()==IDENTIFIER){
            temp=new Identifier(lexycalAnalyzer->get_current_symbol());
            lexycalAnalyzer->next_symbol();
        }else if(lexycalAnalyzer->get_current_symbol()->getContent().compare("(")==0){
            lexycalAnalyzer->next_symbol();
            temp=E();
            check(")");
        }else if(lexycalAnalyzer->get_current_symbol()->getType()==INTEGER){
            temp=new Integer(lexycalAnalyzer->get_current_symbol());
            lexycalAnalyzer->next_symbol();
        }else if(lexycalAnalyzer->get_current_symbol()->getType()==REAL){
            temp=new Float(lexycalAnalyzer->get_current_symbol());
            lexycalAnalyzer->next_symbol();
        }
        return temp;
    }
    void xml(Expression* tree_pointer,std::stringstream& xml_out){
        if(tree_pointer!=nullptr) {
            xml_out<<tree_pointer->open_tag();
            xml_out<<tree_pointer->value();
            if(tree_pointer->has_more){
                std::vector<Expression*> childs;
                if(tree_pointer->open_tag().compare("<BLOQUE>")==0){
                    Block* b=(Block*)tree_pointer;
                    childs=b->get_nodes();
                }else{
                    Selective* s=(Selective*)tree_pointer;
                    childs=s->get_nodes();
                }
                for(std::vector<Expression*>::iterator i=childs.begin();i!=childs.end();++i) xml(*i,xml_out);
            }else{
                xml(tree_pointer->left,xml_out);
                xml(tree_pointer->right,xml_out);
            }
            xml_out<<tree_pointer->close_tag();
        }
    }
public:
    SyntacticAnalyzer(LexycalAnalyzer* lexycalAnalyzer){
        set_lexycal_analyzer(lexycalAnalyzer);
        is_error=false;
    }
    std::vector<Expression*> analyze(){
        lexycalAnalyzer->next_symbol();
        S(tree);
        check("$");
        return tree;
    }
    bool get_error(){
        return is_error;
    }
    void get_XML(std::stringstream& xml_out){
        xml_out<<"<PROGRAMA>\n";
        for(std::vector<Expression*>::iterator i=tree.begin();i!=tree.end();++i) xml(*i,xml_out);
        xml_out<<"</PROGRAMA>";
    }

};
#endif //SYNTACTIC_ANALYZER_SYNTACTICANALYZER_H
