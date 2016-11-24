//
// Created by Carlos A. Fernández on 21/10/2016.
// Expression.h on 04:40 PM 
// Copyright (c) 2016 TK team. All rights reserved.
//

#ifndef PARSERTREE_EXPRESSION_H
#define PARSERTREE_EXPRESSION_H
#include <unordered_map>

enum{ERROR,EMPTY,_REAL,_INTEGER};
const std::string NEWLINE = "print chr$(10)";

class SymbolTable{
private:
    std::unordered_map<std::string,int> table;
public:
    SymbolTable(){}
    ~SymbolTable(){}
    void add(std::string key,int v){ table[key]=v; }
    int get_value(std::string key){ return table[key]; }
    bool contains(std::string symbol){return table.find(symbol)!=table.end();}
    std::unordered_map<std::string,int> get_map(){
        return table;
    }
};

static short count;

class Expression{
public:
    bool is_print = false;
    bool is_else = false;
    Expression *left;
    Expression *right;
    Symbol* symbol;
    bool has_more = false;
    virtual void semantic_validation(SymbolTable &table){}
    std::string get_tag(std::string tag,int type){
        std::stringstream tag_;
        tag_ << tag<<count++;
        return tag_.str();
    }
    virtual std::string open_tag() { return "<TAG>"; }
    virtual std::string value(){ return ""; }
    virtual std::string close_tag(){ return "</TAG>"; }
    virtual std::string generate_code(){return "";}
};

class Assignment:public Expression{
public:
    Assignment(Expression *l,Expression *r,Symbol* s){ left=l; right=r; symbol=s; }
    std::string open_tag(){ return "<ASIGNACION>\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</ASIGNACION>\n"; }
    void semantic_validation(SymbolTable &table){
        right->semantic_validation(table);
        if(!table.contains(left->symbol->getContent()))
            table.add(left->symbol->getContent(),right->symbol->getSemanticType());
        symbol->setSemanticType(right->symbol->getSemanticType());
        left->symbol->setSemanticType(symbol->getSemanticType());
        if(left->symbol->getSemanticType()==ERROR || right->symbol->getSemanticType()==ERROR)
            symbol->setSemanticType(ERROR);
        else if(left->symbol->getSemanticType()==left->symbol->getSemanticType())
            symbol->setSemanticType(EMPTY);
    }
    std::string generate_code(){
        std::stringstream info;
        info<<right->generate_code();
        info<<"pop "<<left->symbol->getContent()<<std::endl;
        if(is_print)
            info<<"print str$("<<left->symbol->getContent()<<")"<<std::endl<<NEWLINE<<std::endl;
        return info.str();
    }
};

class Identifier:public Expression{
public:
    Identifier(Symbol* s){ symbol=s; left=right=nullptr; }
    std::string open_tag(){ return "<ID>"; }
    std::string value(){ std::string s=symbol->getContent();return s; }
    std::string close_tag(){ return "</ID>\n"; }
    void semantic_validation(SymbolTable &table){
        symbol->setSemanticType(table.contains(symbol->getContent())?table.get_value(symbol->getContent()):ERROR);
    }
    std::string generate_code(){
        std::stringstream info;
        info<<"push "<<symbol->getContent()<<std::endl;
        return info.str();
    }
};

class Selective:public Expression{
private:
    std::vector<Expression*> child_nodes;
public:
    Selective(){  left=right= nullptr; has_more=true;  }
    std::vector<Expression*>& get_nodes(){ return child_nodes; }
    void add_child(Expression* e){ child_nodes.push_back(e); }
    std::string open_tag(){ return "<SI>\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</SI>\n"; }
    void semantic_validation(SymbolTable &table){
        Expression* expression;
        bool is_error=false;
        for(std::vector<Expression*>::iterator i=child_nodes.begin();i!=child_nodes.end();++i){
            expression=*i;
            expression->semantic_validation(table);
            is_error = expression->symbol->getSemanticType()==ERROR;
            if(is_error) break;
        }
        symbol->setSemanticType(is_error?ERROR:EMPTY);
    }
    std::string generate_code(){
        std::stringstream info;
        std::string if_ = get_tag("si",0);
        std::string else_ = get_tag("otro",1);
        std::string end_ = get_tag("fin",2);
        if(child_nodes[0]!= nullptr) info<<child_nodes[0]->generate_code();
        info<<"pop eax"<<std::endl<<"cmp eax, 1"<<std::endl<<"jge "<<if_<<std::endl<<"jmp "<<else_<<std::endl<<if_<<":"<<std::endl;
        if(child_nodes[1]!= nullptr) info<<child_nodes[1]->generate_code();
        info<<"jmp "<<end_<<std::endl;
        info<<else_<<":"<<std::endl;
        if(child_nodes.size()==3) info<<child_nodes[2]->generate_code();
        info<<end_<<":"<<std::endl;
        return info.str();
    }
};

class Iterative:public Expression{
public:
    Iterative(Expression *l,Expression *r,Symbol *s){left=l; right=r; symbol=s; }
    std::string open_tag(){ return "<MIENTRAS>\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</MIENTRAS>\n"; }
    void semantic_validation(SymbolTable &table){
        left->semantic_validation(table);
        right->semantic_validation(table);
        symbol->setSemanticType(left->symbol->getSemanticType()==ERROR || right->symbol->getSemanticType()==ERROR?ERROR:EMPTY);
    }
    std::string generate_code(){
        std::stringstream info;
        std::string while_ = get_tag("mientras",0);
        std::string end_ = get_tag("fin",1);
        info<<while_<<":"<<std::endl<<left->generate_code()<<"pop eax"<<std::endl<<"cmp eax, 1"<<std::endl<<"jl "<<end_<<std::endl;
        info<<right->generate_code()<<"jmp "<<while_<<std::endl<<end_<<":"<<std::endl;
        return info.str();
    }
};

class Condition:public Expression{
public:
    Condition(Expression *l,Expression *r,Symbol *s){ left=l; right=r; symbol=s; }
    std::string get_jm(){
        std::string op=symbol->getContent();
        if(op.compare("&lt;")==0)return "jl";
        else if(op.compare("&gt;")==0)return "jg";
        else if(op.compare("==")==0)return "je";
        else if(op.compare("&lt;=")==0)return "jle";
        else if(op.compare("&gt;=")==0)return "jge";
        else return op;
    }
    std::string open_tag(){ return "<EXPRESION value=\""+symbol->getContent()+"\">\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</EXPRESION>\n"; }
    void semantic_validation(SymbolTable &table){
        left->semantic_validation(table);
        right->semantic_validation(table);
        symbol->setSemanticType(left->symbol->getSemanticType()==ERROR || right->symbol->getSemanticType()==ERROR?ERROR:EMPTY);
    }
    std::string generate_code(){
        std::stringstream info;
        std::string true_ = get_tag("verdad",0);
        std::string end_ = get_tag("terminaComparacion",1);
        info<<left->generate_code()<<right->generate_code()<<"pop ebx"<<std::endl<<"pop eax"<<std::endl<<"cmp eax,ebx"<<std::endl;
        info<<get_jm()<<" "<<true_<<std::endl<<"push 0"<<std::endl<<"jmp "<<end_<<std::endl<<true_<<":"<<std::endl<<"push 1"<<std::endl<<end_<<":"<<std::endl;
        return info.str();
    }
};
class Block:public Expression{
private:
    std::vector<Expression*> child_nodes;
public:
    Block(bool ie){left=right= nullptr; has_more=true; symbol=new Symbol();is_else=ie; }
    std::vector<Expression*>& get_nodes(){ return child_nodes; }
    std::string open_tag(){ return is_else?"":"<BLOQUE>\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return is_else?"":"</BLOQUE>\n"; }
    void semantic_validation(SymbolTable &table){
        Expression* expression;
        bool is_error=false;
        for(std::vector<Expression*>::iterator i=child_nodes.begin();i!=child_nodes.end();++i){
            expression=*i;
            expression->semantic_validation(table);
            is_error = expression->symbol->getSemanticType()==ERROR;
            if(is_error) break;
        }
        symbol->setSemanticType(is_error?ERROR:EMPTY);
    }
    std::string generate_code(){
        std::stringstream info;
        Expression* e;
        for(std::vector<Expression*>::iterator i=child_nodes.begin();i!=child_nodes.end();++i){
            e=*i;
            info<<e->generate_code();
        }
        return info.str();
    }
};
class Else:public Expression{
public:
    Else(Expression* e,Symbol* s){left=e;right= nullptr; symbol=s;}
    std::string open_tag(){ return "<OTRO>\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</OTRO>\n"; }
    void semantic_validation(SymbolTable &table){
        left->semantic_validation(table);
        symbol->setSemanticType(left->symbol->getSemanticType());
    }
    std::string generate_code(){
        return left->generate_code();
    }
};

class Signe:public Expression{
public:
    Signe(Expression *l,Symbol* s){ left=l; right= nullptr; symbol=s; }
    std::string open_tag(){ return "<SIGNO value=\""+symbol->getContent()+"\">\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</SIGNO>\n"; }
    void semantic_validation(SymbolTable &table){
        left->semantic_validation(table);
        symbol->setSemanticType(left->symbol->getSemanticType());
    }
    std::string generate_code(){
        std::stringstream info;
        info<<left->generate_code();
        if(symbol->getContent().compare("-")==0)
        info<<"pop eax"<<std::endl<<"push 0"<<std::endl<<"pop ebx"<<std::endl<<"sub ebx,eax"<<std::endl<<"push ebx"<<std::endl;
        return info.str();
    }
};

class Sum:public Expression{
public:
    Sum(Expression *l,Expression *r,Symbol* s){ left=l; right=r; symbol=s; }
    std::string open_tag(){ return "<SUMA value=\""+symbol->getContent()+"\">\n"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</SUMA>\n"; }
    void semantic_validation(SymbolTable &table){
        left->semantic_validation(table);
        right->semantic_validation(table);
        symbol->setSemanticType(left->symbol->getSemanticType()==_INTEGER && right->symbol->getSemanticType()==_INTEGER?
                                _INTEGER:left->symbol->getSemanticType()==_REAL && right->symbol->getSemanticType()==_REAL?_REAL:ERROR);
    }
    std::string generate_code(){
        std::stringstream info;
        info<<left->generate_code();
        info<<right->generate_code();
        info<<"pop ebx"<<std::endl<<"pop eax"<<std::endl<<(
        (symbol->getContent().compare("+")==0)?"add eax,ebx":"sub eax,ebx"
        )<<std::endl<<"push eax"<<std::endl;
        return info.str();
    }
};

class Multiplication:public Expression{
public:
    Multiplication(Expression *l,Expression *r,Symbol* s){ left=l; right=r; symbol=s; }
    std::string open_tag(){ return "<MULT value=\""+symbol->getContent()+"\">\n";}
    std::string value(){ return ""; }
    std::string close_tag(){ return "</MULT>\n"; }
    void semantic_validation(SymbolTable &table){
        left->semantic_validation(table);
        right->semantic_validation(table);
        symbol->setSemanticType(left->symbol->getSemanticType()==_INTEGER && right->symbol->getSemanticType()==_INTEGER?
                                _INTEGER:left->symbol->getSemanticType()==_REAL && right->symbol->getSemanticType()==_REAL?_REAL:ERROR);
    }
    std::string generate_code(){
        std::stringstream info;
        info<<left->generate_code();
        info<<right->generate_code();
        if(symbol->getContent().compare("*")==0)
            info<<"pop ebx"<<std::endl<<"pop eax"<<std::endl<<"mul ebx"<<std::endl<<"push eax"<<std::endl;
        else
            info<<"pop ebx"<<std::endl<<"pop eax"<<std::endl<<"xor edx,edx"<<std::endl<<"idiv ebx"<<std::endl<<"push eax"<<std::endl;
        return info.str();    }
};

class Integer:public Expression{
public:
    Integer(Symbol* s){ symbol=s; left=right=nullptr; }
    std::string open_tag(){ return "<ENTERO>"; }
    std::string value(){ std::string s=symbol->getContent(); return s; }
    std::string close_tag(){ return "</ENTERO>\n"; }
    void semantic_validation(SymbolTable &table){
        symbol->setSemanticType(_INTEGER);
    }
    std::string generate_code(){
        std::stringstream info;
        info<<"push "<<symbol->getContent()<<"\n";
        return info.str();
    }
};

class Float:public Expression{
public:
    Float(Symbol* s){ symbol=s; left=right=nullptr; }
    std::string open_tag(){ return "<REAL>"; }
    std::string value(){ std::string s=symbol->getContent(); return s; }
    std::string close_tag(){ return "</REAL>\n"; }
    void semantic_validation(SymbolTable &table){
        symbol->setSemanticType(_REAL);
    }
    std::string generate_code(){
        std::stringstream info;
        info<<"push "<<symbol->getContent()<<std::endl;
        return info.str();
    }
};
class Print:public Expression{
private:
    Expression* e;
public:
    Print(Expression* e){ this->e=e; }
    std::string open_tag(){ return "<IMPRESION>\n<EXPRESION>"; }
    std::string value(){ return ""; }
    std::string close_tag(){ return "</EXPRESION>\n</IMPRESION>\n"; }
    std::string generate_code(){
        return "";
    }
};

#endif //PARSERTREE_EXPRESSION_H
