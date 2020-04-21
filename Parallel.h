#ifndef PARALLEL_H
#define PARALLEL_H
#include <string>
#include "Utills.h"
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include "Book.h"

class Parallel
{
private:
    std::string gener;
public:
    static char* data_book;
    static std::vector<std::string> all_books;
    static std::unordered_map<int, Book*> books;
    static pthread_mutex_t mut_books;

    Parallel(const std::string&);
    void run();
    void paraReadBooks();
    void paraFilterBooks();
    static void* readFromBooks(void*);
    static void* filterBooks(void*);
    ~Parallel();
};

#endif // !PARALLEL_H