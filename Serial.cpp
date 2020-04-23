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
}

void Serial::parseBooks(){
    // std::vector<std::string> lines;
    // Utills::splitByRef(lines, this->data_books, "\n");

    // for(auto& line : lines){
    //     if(line.size() < 1) continue;
    //     std::vector<std::string> temp;
    //     Utills::splitByRef(temp, line, ",");
    //     Book* book = new Book(temp);
    //     if(book->ifIncludeGener(gener)){
    //         this->books.insert(std::make_pair(book->getBookID(), book));
    //     }
    // }

    // ------------------------------- V2 ----------------------------------

    std::string data_scope("");
    std::vector<std::string> item;
    int i = 0;
    while(this->data_books[i] != '\n') i++;
    i++;
    for(; i < this->size_books; i++){
        if(this->data_books[i] == ','){
            item.push_back(data_scope);
            data_scope = "";
        }
        else if(this->data_books[i] == '\n'){
            item.push_back(data_scope);
            Book* book = new Book(item);
            if(book->ifIncludeGener(gener))
                this->books.insert(std::make_pair(book->getBookID(), book));
            data_scope = "";
            item.clear();
        } 
        else
            data_scope += this->data_books[i];
    }
}

void Serial::parseReviews(){
    std::string data_scope("");
    int scope = 0;
    int* item = new int[3];
    int i = 0;
    while(this->data_reviews[i] != '\n') i++;
    i++;
    for(; i < this->size_reviews; i++){
        if(this->data_reviews[i] == ','){
            item[scope++] = std::stoi(data_scope.c_str());
            data_scope = "";
        }
        else if(this->data_reviews[i] == '\n'){
            item[scope] = std::stod(data_scope.c_str());
            scope = 0;
            this->reviews.push_back(item);
            item = new int[3];
            data_scope = "";
        } 
        else
            data_scope += this->data_reviews[i];
    }
}

void Serial::findBestBook(){
    
    for(auto& line : reviews){
        std::unordered_map<int, Book*>::iterator itr_book = this->books.find(line[0]);
        if(itr_book != books.end())
            itr_book->second->updateRates(line[1], line[2]);
    }
    
    Book* max_book = NULL;
    float max_pop  = -1;
    for(auto& item : books){
        if(item.second->getPopRate() > max_pop){
            max_book = item.second;
            max_pop = item.second->getPopRate();
        }
    }

    max_book->printBook();
}

void Serial::run(){
    this->readFiles();
    this->parseBooks();  
    this->parseReviews();
    this->findBestBook();
}
