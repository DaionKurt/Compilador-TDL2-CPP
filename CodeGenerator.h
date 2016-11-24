//
// Created by Carlos A. Fernández on 07/11/2016.
// CodeGenerator.h on 12:51 AM 
// Copyright (c) 2016 TK team. All rights reserved.
//

#ifndef CODE_GENERATION_PR5_CODEGENERATOR_H
#define CODE_GENERATION_PR5_CODEGENERATOR_H

class CodeGeneration{
private:
    std::vector<std::string> keys;
    std::vector<int> vals;
    std::vector<Expression*> tree;
public:
    CodeGeneration(std::unordered_map<std::string,int> map,std::vector<Expression*> tree){
        this->tree=tree;
        keys.reserve(map.size());
        vals.reserve(map.size());
        for(auto kv : map) {
            keys.push_back(kv.first);
            vals.push_back(kv.second);
        }
    }
    std::string get_var(std::string valor){
        std::stringstream info;
        for(int i=keys.size()-1,j=0;i>=j;i--){
            info<<keys.at(i)<<" dword "<<valor<<std::endl;
        }
        return info.str();
    }
    std::string get_code(){
        std::stringstream code;
        Expression *expresion;
        for(std::vector<Expression*>::iterator i=tree.begin();i!=tree.end();++i) {
            expresion=*i;
            code<<expresion->generate_code();
        }
        return code.str();
    }
    std::string traslate(){
        std::stringstream info;
        info<<".386"<<std::endl;
        info<<".model flat, stdcall"<<std::endl;
        info<<"option casemap:none"<<std::endl;
        info<<"include \\masm32\\macros\\macros.asm "<<std::endl;
        info<<"include \\masm32\\include\\masm32.inc"<<std::endl;
        info<<"include \\masm32\\include\\kernel32.inc"<<std::endl;
        info<<"includelib \\masm32\\lib\\masm32.lib"<<std::endl;
        info<<"includelib \\masm32\\lib\\kernel32.lib"<<std::endl;
        info<<".data"<<std::endl;
        info<<get_var("0");
        info<<".data?"<<std::endl;
        info<<".code"<<std::endl;
        info<<"inicio:"<<std::endl;
        info<<get_code();
        info<<"exit"<<std::endl;
        info<<"end inicio"<<std::endl;
        return info.str();
    }
};

#endif //CODE_GENERATION_PR5_CODEGENERATOR_H
