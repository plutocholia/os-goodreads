#ifndef BOOK_H
#define BOOK_H
#include <string>
#include <vector>

class Book
{
private:
    int book_id;
    std::string book_title;
    std::string gener_1;
    std::string gener_2;
    int pages;
    std::string author_name;
    float author_average_rating;

public:
    int total_num_of_likes;
    int numerator;
    Book(const std::vector<std::string>&);
    bool ifIncludeGener(const std::string&);
    float getAuthorAverageRating();
    int getBookID();
    void printBook();
    float getPopRate();
    void updateRates(int, int);
    ~Book();
};

#endif // !BOOK_H
