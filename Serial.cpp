#include "Serial.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include "Utills.h"
#include "Book.h"
#include <iomanip>
#include <string.h>
Serial::Serial(const std::string& gener)
{
    this->gener = gener;
}

Serial::~Serial()
{
}

void Serial::readFiles(){
    std::ifstream _file;
    _file.open(DIR_BOOKS);
    _file.seekg(0, std::ios_base::end);
    this->size_books = _file.tellg();
    _file.seekg(0);
    this->data_books = new char[this->size_books];
    _file.read(this->data_books, this->size_books);
    _file.close();
    _file.open(DIR_REVIEW);
    _file.seekg(0, std::ios_base::end);
    this->size_reviews = _file.tellg();
    _file.seekg(0);
    this->data_reviews = new char[this->size_reviews];
    _file.read(this->data_reviews, this->size_reviews);
    _file.close();
    std::cout << strlen(this->data_books) << " " << strlen(this->data_reviews) << std::endl;
}

void Serial::parseBooks(){
    std::vector<std::string> lines;
    Utills::splitByRef(lines, this->data_books, "\n");

    for(auto& line : lines){
        if(line.size() < 1) continue;
        std::vector<std::string> temp;
        Utills::splitByRef(temp, line, ",");
        Book* book = new Book(temp);
        if(book->ifIncludeGener(gener)){
            this->books.insert(std::make_pair(book->getBookID(), book));
        }
    }
}

void Serial::run(){
    this->readFiles();
    this->parseBooks();  
}
