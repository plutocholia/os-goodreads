#include "Book.h"
#include <string>
#include <iostream>
#include <cstdio>

Book::Book(const std::vector<std::string>& data)
{
    this->book_id               = std::atoi(data[0].c_str());
    this->book_title            = data[1];
    this->gener_1               = data[2];
    this->gener_2               = data[3];
    this->pages                 = std::atoi(data[4].c_str());
    this->author_name           = data[5];
    this->author_average_rating = std::atof(data[6].c_str());
    this->total_num_of_likes    = 0;
    this->numerator             = 0;
}

Book::~Book()
{
}

bool Book::ifIncludeGener(const std::string& gener){
    if(this->gener_1 == gener || this->gener_2 == gener)
        return true;
    else return false;
}

float Book::getAuthorAverageRating(){
    return this->author_average_rating; 
}

int Book::getBookID(){
    return this->book_id;
}

void Book::printBook(){
    std::cout << "id: " << this->book_id << std::endl;
    std::cout << "Title: " << this->book_title << std::endl;
    std::cout << "Genres: " << this->gener_1 << ", " << this->gener_2 << std::endl;
    std::cout << "Number of Pages: " << this->pages << std::endl;
    std::cout << "Author: " << this->author_name << std::endl;
    printf("Average Rating: %.2f\n", this->getPopRate());
}

float Book::getPopRate(){
    if(this->total_num_of_likes == 0) return 0.0;
    else return (this->author_average_rating + this->numerator/this->total_num_of_likes)/10;
}

void Book::updateRates(int rate, int number_of_like){
    this->numerator += (rate * number_of_like);
    this->total_num_of_likes += number_of_like;
}