//
// Created by Carlos A. Fernández on 28/08/2016.
// main.cpp on 06:10 PM
// Copyright (c) 2016 TK team. All rights reserved.
//
#include <iostream>
#include <vector>
#include "Semantic Analyzer.h"
#include "CodeGenerator.h"

int main(){
    std::ifstream in_file("entrada.txt");
    std::ofstream out_file("salida.asm");
    int result;
    LexycalAnalyzer lexycalAnalyzer(in_file);
    SyntacticAnalyzer syntacticAnalyzer(&lexycalAnalyzer);
    SemanticAnalyzer semanticAnalyzer(&syntacticAnalyzer);
    result=(semanticAnalyzer.analyze()?1:0);
    if(result){
        CodeGeneration codeGeneration(semanticAnalyzer.get_map(),semanticAnalyzer.get_tree());
        std::string s = codeGeneration.traslate();
        std::cout<<s<<std::endl;
        out_file<<s;
    }
    system("\\masm32\\bin\\ml /c  /coff  /Cp salida.asm");
    system("\\masm32\\bin\\link /SUBSYSTEM:CONSOLE /LIBPATH:.\\lib salida");
    system("salida");
    //Prueba de GIT
    return 0;
}