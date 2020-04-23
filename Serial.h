#ifndef SERIAL_H
#define SERIAL_H
#include <string>
#include <unordered_map>
#include <vector>
#include "Book.h"
class Serial
{
private:
    std::string gener;
    std::unordered_map<int, Book*> books;
    std::vector<std::vector<int> > reviews;
    char* data_books;
    char* data_reviews;
    size_t size_books;
    size_t size_reviews;

public:
    Serial(const std::string&);
    void readFiles();
    void parseBooks();
    void parseReviews();
    void findBestBook();
    ~Serial();
    void run();
};

#endif // SERIAL_H