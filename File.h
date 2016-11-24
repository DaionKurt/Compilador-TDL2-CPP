//
// Created by Carlos A. Fernández on 28/08/2016.
// File.h on 02:10 PM 
// Copyright (c) 2016 TK team. All rights reserved.
//
#ifndef LEXYCAL_ANALYZER_FILE_H
#define LEXYCAL_ANALYZER_FILE_H

#include <string>
#include <sstream>

class File {
    std::istream &file;
    std::istringstream buffer;
    std::string next_line();
    bool empty_line();
    bool end()const;
    void save_next_line();
public:
    explicit File(std::istream& file): file(file){}
    inline operator bool() { return eof(); }
    bool eof();
    char next_character() ;
    void backward();
};
char File::next_character() {
    if(empty_line()) save_next_line();
    return (char)buffer.get();
}

#include <iostream>
void File::save_next_line() {
    buffer.clear();
    std::string line=next_line();
    file.unget();
    if(file.get()=='\n') line+='\n';
    buffer.str(line);
}
void File::backward() {
    if(!buffer.unget()){
        buffer.clear();
        file.clear();
        file.seekg(-((int)buffer.str().size()),file.cur);
        file.unget();
        while(file.unget()&&file.peek()!='\n') {}
        if(file.peek()=='\n')file.get();
        else file.clear();
        save_next_line();
        buffer.seekg(-1,buffer.end);
    }
}
bool File::eof(){
    return empty_line()&&end();
}
bool File::empty_line(){
    return buffer.tellg()==buffer.str().size();
}
bool File::end() const{
    return file.eof();
}
std::string File::next_line() {
    std::string line;
    std::getline(file,line);
    return line;
}

#endif //LEXYCAL_ANALYZER_FILE_H
