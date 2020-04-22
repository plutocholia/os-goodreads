#include "Parallel.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

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

    size_t i = 0;
    for(const auto& line : Parallel::all_books){
        if(i != 0){
            Book* book = new Book(Utills::splitBy(line, ","));
            if(book->ifIncludeGener(gener)){
                pthread_mutex_lock (&Parallel::mut_books);
                Parallel::books.insert(std::make_pair(book->getBookID(), book));
                pthread_mutex_unlock (&Parallel::mut_books);
            }
        }
        i++;
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

    for(int j = 0; j < len; j++)
        Parallel::data_reviews[start + j] = buff[j];
    
    // std::cout << "thread number " << tid << std::endl;
    // std::cout << "\t" << "start = " << start << " and len of : " << len << std::endl;
    // std::cout << "\tbuff size: " << strlen(buff) << std::endl;
    // std::cout << "\tdata_reviews size : " << strlen(data_reviews) << std::endl; 
    
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
    std::cout << "Book.csv size in bytes: " << file_size << std::endl;
    pthread_t threads[NUMBER_OF_THREADS];
    
    int num_of_chunks = NUMBER_OF_THREADS;

    std::vector<std::vector<int>> params;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::vector<int> param(3);
        param[0] = i;
        param[2] = i * static_cast<int>(file_size / num_of_chunks);
        if(i == NUMBER_OF_THREADS - 1)
            param[1] = file_size - param[2];
        else
            param[1] = static_cast<int>(file_size / num_of_chunks);
        
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
    int size = Parallel::all_books.size();
    int len = static_cast<int>(size / NUMBER_OF_THREADS);

    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::pair<std::vector<int>, std::string> param;
        param.first.push_back(i);
        param.first.push_back(i * len);
        if(i == NUMBER_OF_THREADS - 1)
            param.first.push_back(size - i*len);
        else param.first.push_back(len);
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
        param[2] = i * static_cast<int>(file_size / num_of_chunks);
        if(i == NUMBER_OF_THREADS - 1)
            param[1] = file_size - param[2];
        else
            param[1] = static_cast<int>(file_size / num_of_chunks);
        
        params.push_back(param);
    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&threads[i], NULL, &Parallel::readFromReviews, &params[i]);
    
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);
    
    pthread_exit(NULL);
}

void Parallel::paraReadAllData(){
    pthread_t th1, th2;
    pthread_create(&th1, NULL, &Parallel::paraReadBooks, NULL);
    pthread_create(&th2, NULL, &Parallel::paraReadReviews, NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
}

void Parallel::run(){
    this->paraReadAllData();
    // Utills::splitByRef(Parallel::all_books, Parallel::data_book, "\n");
    // Utills::splitByRef(Parallel::all_reviews, Parallel::data_reviews, "\n");
    // this->paraFilterBooks();
    
    // Book* max_book = NULL;
    // float max_pop  = -1;

    // size_t i = 0;
    // for(auto line : all_reviews){
    //     if(i != 0 && line.size() != 0){        
    //         std::vector<std::string> review = Utills::splitBy(line, ",");
    //         int book_id         = std::atoi(review[0].c_str());
    //         int rating          = std::atoi(review[1].c_str());
    //         int number_of_likes = std::atoi(review[2].c_str());
    //         std::unordered_map<int, Book*>::iterator itr_book = books.find(book_id);
    //         if(itr_book != books.end())
    //             itr_book->second->updateRates(rating, number_of_likes);
    //     }
    //     i += 1;
    // }
    // for(auto item : books){
    //     if(item.second->getPopRate() > max_pop){
    //         max_book = item.second;
    //         max_pop = item.second->getPopRate();
    //     }
    // }
    // max_book->printBook();
}