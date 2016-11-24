//
// Created by Carlos A. Fernández on 28/08/2016.
// Utilities.h on 02:10 PM 
// Copyright (c) 2016 TK team. All rights reserved.
//
#ifndef LEXYCAL_ANALYZER_UTILITIES_H
#define LEXYCAL_ANALYZER_UTILITIES_H
#include <exception>
#define num(x)  (sizeof(x) / sizeof((x)[0]))

class Exception :public std::exception{
    private:
        std::string message;
    public:
        Exception(std::string message){ this->message=message; }
        std::string getMessage(){ return message; }
        virtual const char* what() const throw(){ return message.c_str(); }
};

class FileNotFound :public Exception{
    public:
        FileNotFound(std::string e):Exception(e){}
};

#endif //LEXYCAL_ANALYZER_UTILITIES_H
