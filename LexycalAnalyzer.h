//
// Created by Carlos A. Fernández on 28/08/2016.
// LexycalAnalyzer.h on 02:10 PM
// Copyright (c) 2016 TK team. All rights reserved.
//
#ifndef ANALIZADORLEXICO_H_INCLUDED
#define ANALIZADORLEXICO_H_INCLUDED

#include <string>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include "Utilities.h"
#include "Symbol.h"
#include "File.h"

class LexycalAnalyzer {
private:
    Symbol* current_symbol;
    File file;
    std::string symbol;
    int state;
    bool continue_;
    bool error;
    void skip_blank();
    int next_state(int current_state, char character);
    void forward(int state, char character);
    bool is_acceptance(int state);
    bool is_fixed_length(int state);
    void set_error(bool e);
    static const char fixed_length[];
    static const int acceptance_states[];
public:
    explicit LexycalAnalyzer(std::istream &file):symbol(),state(0),continue_(false),error(false),file(file) {}
    Symbol* next_symbol();
    bool is_error() const;
    bool file_eof();
    bool analyze();
    Symbol* get_current_symbol(){
        return current_symbol;
    }
};

const char LexycalAnalyzer::fixed_length[]={'+', '-', '/', '*', '=', '<', '>', '!', '|', '&', '(',')',';',' ', '\n','\t',EOF,','};
const int LexycalAnalyzer::acceptance_states[]= {1,2,4,6,11,10,9,12,13,8,14,15,16,14,20};
bool LexycalAnalyzer::analyze() {
    bool error=false;
    current_symbol=next_symbol();
    while(!error&&(current_symbol->getContent()!="-1"))    {
        error = is_error();
        current_symbol=next_symbol();
    }
    return error;
}
bool LexycalAnalyzer::is_error() const{
    return error;
}
Symbol* LexycalAnalyzer::next_symbol() {
    current_symbol = new Symbol();
    state = 0;
    skip_blank();
    symbol="";
    continue_ = true;
    char character;//=file.next_character();
    set_error(false);
    if(character == EOF) current_symbol->setContent("$");
    else{
        do{
            character=file.next_character();
            if(character == EOF) current_symbol->setContent("$");
        }while(isspace(character) || character=='\n' || character==' ');
        while (continue_) {
            int next=next_state(state,character);
            if(next!=-1) {
                forward(next,character);
                current_symbol->concatenateContent(character);
                character=file.next_character();
            }else{
                if(is_acceptance(state)&&
                   (is_fixed_length(symbol[symbol.size()-1])||isspace(character)||character==EOF||is_fixed_length(character))||state==6){
                    if(character!=EOF) file.backward();
                    continue_ = false;
                }else {
                    while(!is_fixed_length(character)) {
                        symbol += character;
                        current_symbol->concatenateContent(character);
                        character = file.next_character();
                    };
                    if(character!=EOF)file.backward();
                    continue_ = false;
                    set_error(true);
                }
            }
        }
    }

    if(current_symbol->getContent().compare("<")==0) current_symbol->setContent("&lt;");
    else if(current_symbol->getContent().compare(">")==0) current_symbol->setContent("&gt;");
    else if(current_symbol->getContent().compare("<=")==0) current_symbol->setContent("&lt;=");
    else if(current_symbol->getContent().compare(">=")==0) current_symbol->setContent("&gt;=");




    if(current_symbol->getContent().compare("mientras")==0) current_symbol->setType(WHILE);
    else if(current_symbol->getContent().compare("si")==0) current_symbol->setType(IF);
    else if(current_symbol->getContent().compare("otro")==0) current_symbol->setType(ELSE);
    else if(current_symbol->getContent().compare("fin")==0) current_symbol->setType(END);
    return current_symbol;
}
int LexycalAnalyzer::next_state(int state, char character) {
    int next_state=-1;
    switch (state){
        case 0:
            if(character=='<'||character=='>'){ current_symbol->setType(RELATIONAL_OP); next_state=11;}
            else if(character=='='){ current_symbol->setType(ASSIGNMENT_OP);next_state=10;}
            else if(character=='!'){ current_symbol->setType(LOGICAL_OP); next_state=9;}
            else if(character=='+'||character=='-'){ current_symbol->setType(ADDITION_OP);next_state=12;}
            else if(character==';'){current_symbol->setType(DELIMITER); next_state=13;}
            else if(character=='*'){current_symbol->setType(MULTIPLICATION_OP); next_state=14;}
            else if(character=='/'){current_symbol->setType(ARITHMETIC_OP); next_state=15;}
            else if(character==')'||character=='('){current_symbol->setType(PARENTHESIS); next_state=16;}
            else if(character=='|') next_state=17;
            else if(character=='&') next_state=18;
            else if(character=='\n'){current_symbol->setType(SPACE); next_state=20;}
            else if(character==','){ current_symbol->setType(COMMA); next_state=20;}
            else if(isalpha(character)||character=='_'){current_symbol->setType(IDENTIFIER); next_state=1;}
            else if(character=='"'){current_symbol->setType(STRING); next_state=5;}
            else if(isdigit(character)){current_symbol->setType(INTEGER); next_state=2;}
            break;
        case 1:if(isalnum(character)||character=='_') next_state=1;break;
        case 2:if(isdigit(character)) next_state=2; else if(character=='.'){current_symbol->setType(REAL); next_state=3;}break;
        case 3:if(isdigit(character)) next_state=4;break;
        case 4:if(isdigit(character)) next_state=4;break;
        case 5:if(character!=EOF&&character!='"') next_state=5; else if(character=='"') next_state=6;
        case 11:case 9:case 10:if(character=='='){ current_symbol->setType(RELATIONAL_OP); next_state=8;}break;
        case 17:if(character=='|'){current_symbol->setType(LOGICAL_OP); next_state=19;}break;
        case 18:if(character=='&'){current_symbol->setType(LOGICAL_OP); next_state=19;}break;
        default:break;
    }
    return next_state;
}
void LexycalAnalyzer::set_error(bool error) {
    this->error=error;
    if(error)state = -1;
}
bool LexycalAnalyzer::is_fixed_length(int state) {
    return std::count(fixed_length,fixed_length+num(fixed_length),state)>0;
}
bool LexycalAnalyzer::is_acceptance(int state) {
    return std::count(acceptance_states,acceptance_states+num(acceptance_states),state)>0;
}
void LexycalAnalyzer::forward(int state, char character) {
    this->state=state;
    symbol.push_back(character);
}
void LexycalAnalyzer::skip_blank() {
    char character;
    while((character=file.next_character())==' '||character=='\t'||character=='\r'){};
    if(character!=EOF) file.backward();
}
bool LexycalAnalyzer::file_eof(){
    return file.eof();
}

#endif // ANALIZADORLEXICO_H_INCLUDED
