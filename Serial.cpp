#include "Serial.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include "Utills.h"
#include "Book.h"
#include <iomanip>

Serial::Serial(const std::string& gener)
{
    this->gener = gener;
}

Serial::~Serial()
{
}

void Serial::run(){
    std::cout << "Serial Filtering is running on Gener: " << this->gener << std::endl;
    std::fstream ifile;
    std::unordered_map<int, Book*> books;

    ifile.open(DIR_BOOKS, std::ios::in);
    if(ifile.is_open()){
        std::string line;
        size_t line_number = 0;
        while(getline(ifile, line)){
            if(!(line_number == 0)){
                Book* book = new Book(Utills::splitBy(line, ","));
                if(book->ifIncludeGener(this->gener))
                    books.insert(std::make_pair(book->getBookID(), book));
            }
            line_number += 1;
        }
        ifile.close();
    }
    else{
        std::cout << "Unable to open books.csv\n";
        exit(EXIT_FAILURE);
    }

    ifile.open(DIR_REVIEW, std::ios::in);
    Book* max_book = NULL;
    float max_pop  = -1;
    if(ifile.is_open()){
        std::string line;
        size_t line_number = 0;
        while(getline(ifile, line)){
            if(!(line_number == 0)){
                std::vector<std::string> review = Utills::splitBy(line, ",");
                int book_id         = std::atoi(review[0].c_str());
                int rating          = std::atoi(review[1].c_str());
                int number_of_likes = std::atoi(review[2].c_str());
                std::unordered_map<int, Book*>::iterator itr_book = books.find(book_id);
                if(itr_book != books.end()){
                    Book* temp = itr_book->second;
                    temp->numerator += (rating * number_of_likes);
                    temp->total_num_of_likes += number_of_likes;
                }
            }
            line_number += 1;
        }
        ifile.close();
    }

    for(auto item : books){
        if(item.second->getPopRate() > max_pop){
            max_book = item.second;
            max_pop = item.second->getPopRate();
        }
    }
    max_book->printBook();
}
