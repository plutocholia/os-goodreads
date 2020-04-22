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
    static std::vector<std::string> all_books;
    static std::unordered_map<int, Book*> books;
    static pthread_mutex_t mut_books;
    static char* data_reviews;
    static std::vector<std::string> all_reviews;

    Parallel(const std::string&);
    void run();
    static void* paraReadBooks(void*);
    void paraFilterBooks();
    static void* paraReadReviews(void*);
    void paraReadAllData();
    static void* readFromBooks(void*);
    static void* filterBooks(void*);
    static void* readFromReviews(void*);
    ~Parallel();    
};

#endif // !PARALLEL_H