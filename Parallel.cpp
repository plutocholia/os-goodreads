#include "Parallel.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <pthread.h>

#define NUMBER_OF_THREADS 4

char* Parallel::data_book;
std::vector<std::string> Parallel::all_books;
std::unordered_map<int, Book*> Parallel::books;
pthread_mutex_t Parallel::mut_books;
char* Parallel::data_reviews;
std::vector<std::string> Parallel::all_reviews;

Parallel::Parallel(const std::string& gener)
{
    this->gener = gener;
}

Parallel::~Parallel()
{
}

void* Parallel::readFromBooks(void* input){
    std::vector<int> data = *(static_cast<std::vector<int>*>(input));
    int tid     = data[0];
    int len     = data[1];
    int start   = data[2];
    
    std::ifstream file;
    file.open(DIR_BOOKS);
    file.seekg(start);
    char* buff = new char[len];
    file.read(buff, len);
    file.close();

    for(int i = 0; i < len; i++)
        Parallel::data_book[start + i] = buff[i];
    
    delete buff;
    pthread_exit(NULL);
}

void* Parallel::filterBooks(void* input){
    std::pair<std::vector<int>, std::string> data = *(static_cast<std::pair<std::vector<int>, std::string>*>(input));
    int tid     = data.first[0];
    int start   = data.first[1];
    int len     = data.first[2];
    std::string gener = data.second;

    for(auto line : Parallel::all_books){
        Book* book = new Book(Utills::splitBy(line, ","));
        if(book->ifIncludeGener(gener)){
            pthread_mutex_lock (&Parallel::mut_books);
            Parallel::books.insert(std::make_pair(book->getBookID(), book));
            pthread_mutex_unlock (&Parallel::mut_books);
        }
    }

    pthread_exit(NULL);
}

void* Parallel::readFromReviews(void* input){
    std::vector<int> data = *(static_cast<std::vector<int>*>(input));
    int tid     = data[0];
    int len     = data[1];
    int start   = data[2];
    
    std::ifstream file;
    file.open(DIR_REVIEW);
    file.seekg(start);
    char* buff = new char[len];
    file.read(buff, len);
    file.close();

    for(int i = 0; i < len; i++)
        Parallel::data_reviews[start + i] = buff[i];
    
    delete buff;
    pthread_exit(NULL);
}

void* Parallel::paraReadBooks(void*){
     
    std::ifstream _file;
    _file.open(DIR_BOOKS);
    _file.seekg(0, std::ios_base::end);
    int file_size = _file.tellg();
    _file.close();
    Parallel::data_book = new char[file_size];

    pthread_t threads[NUMBER_OF_THREADS];
    
    int num_of_chunks = NUMBER_OF_THREADS;

    std::vector<std::vector<int>> params;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::vector<int> param(3);
        param[0] = i;
        param[1] = file_size / num_of_chunks;
        param[2] = i * file_size / num_of_chunks;
        params.push_back(param);
    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&threads[i], NULL, &Parallel::readFromBooks, &params[i]);
    
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);
    
    pthread_exit(NULL);   
}

void Parallel::paraFilterBooks(){
    pthread_t threads[NUMBER_OF_THREADS];

    std::vector<std::pair<std::vector<int>, std::string> > params;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::pair<std::vector<int>, std::string> param;
        param.first.push_back(i);
        param.first.push_back(i * Parallel::all_books.size() / NUMBER_OF_THREADS);
        param.first.push_back(Parallel::all_books.size() / NUMBER_OF_THREADS);
        param.second = this->gener;
        params.push_back(param);
    }
    pthread_mutex_init(&Parallel::mut_books, NULL);

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&threads[i], NULL, &Parallel::filterBooks, &params[i]);   
    
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mut_books);
}

void* Parallel::paraReadReviews(void*){
    std::ifstream _file;
    _file.open(DIR_REVIEW);
    _file.seekg(0, std::ios_base::end);
    int file_size = _file.tellg();
    _file.close();
    std::cout << "size of reviews in byte: " << file_size << std::endl;
    Parallel::data_reviews = new char[file_size];

    pthread_t threads[NUMBER_OF_THREADS];
    
    int num_of_chunks = NUMBER_OF_THREADS;

    std::vector<std::vector<int>> params;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::vector<int> param(3);
        param[0] = i;
        param[1] = file_size / num_of_chunks;
        param[2] = i * file_size / num_of_chunks;
        params.push_back(param);
    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&threads[i], NULL, &Parallel::readFromReviews, &params[i]);
    
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);
    
    pthread_exit(NULL);
}

void Parallel::readAllData(){
    pthread_t th1, th2;
    pthread_create(&th1, NULL, &Parallel::paraReadBooks, NULL);
    pthread_create(&th2, NULL, &Parallel::paraReadReviews, NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
}

void Parallel::run(){
    this->readAllData();
    // this->paraReadBooks();
    // Parallel::all_books = Utills::splitBy(Parallel::data_book, "\n");
    // std::cout << "Number of Books Record: " << Parallel::all_books.size() << std::endl;
    // this->paraFilterBooks();
    // std::cout << "Number of Filterd Results: " << Parallel::books.size() << std::endl;
    // this->paraReadReviews();
    // // Parallel::all_reviews = Utills::splitBy(Parallel::data_reviews, "\n");
    // std::cout << "Number of Reviews Record: " << Parallel::all_reviews.size() << std::endl;



    // std::cout << Parallel::data_book;
    // int nch = 10;
    // for(int i = 0; i < nch; i++){
    //     std::ifstream file1;
    //     file1.open("./datasets/test.csv");
    //     file1.seekg(i * file_size / nch);
    //     char* buff1 = new char[file_size];
    //     file1.read(buff1, file_size / nch);
    //     file1.close();
    //     std::cout << "[" << buff1 << "]" << std::endl;
    // }
}