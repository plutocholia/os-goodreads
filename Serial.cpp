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

void Serial::parseReviews(){
    std::vector<std::string> lines;
    Utills::splitByRef(lines, this->data_reviews, "\n");

    for(auto& line : lines){
        if(line.size() < 1){
            continue;
        }
        std::vector<std::string> review;
        Utills::splitByRef(review, line, ",");
        int* payload = new int[3];
        payload[0] = std::atoi(review[0].c_str());
        payload[1] = std::atoi(review[1].c_str());
        payload[2] = std::atoi(review[2].c_str());
        this->reviews.push_back(payload);
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
