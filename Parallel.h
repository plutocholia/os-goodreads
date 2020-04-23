#ifndef PARALLEL_H
#define PARALLEL_H
#include <string>
#include "Utills.h"
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include "Book.h"
#define NUMBER_OF_THREADS 4

class Parallel
{
private:
    std::string gener;
public:
    static char* data_book;
    static std::unordered_map<int, Book*> books;
    static pthread_mutex_t mut_books;
    static char* data_reviews;
    static size_t size_books;
    static size_t size_reviews;
    static std::vector<int*> reviews[NUMBER_OF_THREADS];

    Parallel(const std::string&);
    void run();
    static void* paraReadBooks(void*);
    static void* paraReadReviews(void*);
    void paraReadAllData();
    static void* readFromBooks(void*);
    static void* readFromReviews(void*);
    void paraParseBooks();
    static void* parseBooks(void*);
    static int niceByte(char*, int);
    static void* parseReviews(void*);
    void paraParseReviews();
    void findBestBook();
    ~Parallel();    
};

#endif // !PARALLEL_H