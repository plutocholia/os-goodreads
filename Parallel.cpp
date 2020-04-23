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
size_t Parallel::size_books;
size_t Parallel::size_reviews;
// std::vector<std::vector<int> > Parallel::reviews[NUMBER_OF_THREADS];
std::vector<int*> Parallel::reviews[NUMBER_OF_THREADS];

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
    
    // delete buff;
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
    
    // delete buff;
    pthread_exit(NULL);
}

void* Parallel::paraReadBooks(void*){
     
    std::ifstream _file;
    _file.open(DIR_BOOKS);
    _file.seekg(0, std::ios_base::end);
    int file_size = _file.tellg();
    _file.close();
    Parallel::data_book = new char[file_size];
    Parallel::size_books = file_size;
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
    // return 0;
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
    Parallel::size_reviews = file_size;
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
    // return 0;
}

void Parallel::paraReadAllData(){
    pthread_t th1, th2;
    pthread_create(&th1, NULL, &Parallel::paraReadBooks, NULL);
    pthread_create(&th2, NULL, &Parallel::paraReadReviews, NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    // Parallel::paraReadBooks(NULL);
    // Parallel::paraReadReviews(NULL);
}

int Parallel::niceByte(char* data, int pos){
    while(data[pos] != '\n'){pos--;}
    return pos;
}

void Parallel::paraParseBooks(){
    
    pthread_t threads[NUMBER_OF_THREADS];
    int chunck = Parallel::size_books / NUMBER_OF_THREADS;
    std::vector<std::pair<std::vector<int>, std::string> > params(NUMBER_OF_THREADS);
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::vector<int> param(3);
        param[0] = i;
        
        if(i == 0){
            param[1] = 0;
            param[2] = Parallel::niceByte(Parallel::data_book, chunck);
        }
        else if(i == NUMBER_OF_THREADS - 1){
            param[1] = params[i - 1].first[2] + 1;
            param[2] = Parallel::size_books - 1;
        }
        else{
            param[1] = params[i - 1].first[2] + 1;
            param[2] = Parallel::niceByte(Parallel::data_book, param[1] + chunck);
        }
        params[i].first =  param;
        params[i].second = this->gener;
    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&threads[i], NULL, &Parallel::parseBooks, &params[i]);
    
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);
    
}

void* Parallel::parseBooks(void* input){
    std::pair<std::vector<int>, std::string> data = *(static_cast<std::pair<std::vector<int>, std::string>*>(input));
    int tid     = data.first[0];
    int start   = data.first[1];
    int end     = data.first[2];
    std::string gener = data.second;
    // std::vector<std::string> book;
    // for(int i = start; i < end; i++){
    //     char temp[]
    //     if(Parallel::data_book[i] == ','){
            
    //     }
    //     else if(Parallel::data_book[i] == '\n'){

    //     }
    //     else{
            
    //     }
    // }

    // std::string str_data_books(Parallel::data_book, start, end - start + 1);
    // std::vector<std::string> lines;
    // Utills::splitByRef(lines, str_data_books, "\n");

    // size_t i = 0;
    // for(auto& line : lines){
    //     if((tid == 0 && i == 0) || line.size() < 1){i += 1; continue;}
    //     std::vector<std::string> temp;
    //     Utills::splitByRef(temp, line, ",");
    //     Book* book = new Book(temp);
    //     if(book->ifIncludeGener(gener)){
    //         pthread_mutex_lock (&Parallel::mut_books);
    //         Parallel::books.insert(std::make_pair(book->getBookID(), book));
    //         pthread_mutex_unlock (&Parallel::mut_books);
    //     }
    //     i++;
    // }

    std::string data_scope("");
    std::vector<std::string> item;
    int i = start;
    if(i == 0){
        while(Parallel::data_book[i] != '\n') i++;
        i++;
    }
    for(; i <= end; i++){
        if(Parallel::data_book[i] == ','){
            item.push_back(data_scope);
            data_scope = "";
        }
        else if(Parallel::data_book[i] == '\n'){
            item.push_back(data_scope);
            Book* book = new Book(item);
            if(book->ifIncludeGener(gener)){
                pthread_mutex_lock (&Parallel::mut_books);
                Parallel::books.insert(std::make_pair(book->getBookID(), book));
                pthread_mutex_unlock (&Parallel::mut_books);
            }
            data_scope = "";
            item.clear();
        } 
        else
            data_scope += Parallel::data_book[i];
    }

    pthread_exit(NULL);
}

void Parallel::paraParseReviews(){
    pthread_t threads[NUMBER_OF_THREADS];
    int chunck = Parallel::size_reviews / NUMBER_OF_THREADS;
    std::vector<std::vector<int> > params(NUMBER_OF_THREADS);
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        std::vector<int> param(3);
        param[0] = i;
        
        if(i == 0){
            param[1] = 0;
            param[2] = Parallel::niceByte(Parallel::data_reviews, chunck);
        }
        else if(i == NUMBER_OF_THREADS - 1){
            param[1] = params[i - 1][2] + 1;
            param[2] = Parallel::size_reviews - 1;
        }
        else{
            param[1] = params[i - 1][2] + 1;
            param[2] = Parallel::niceByte(Parallel::data_reviews, param[1] + chunck);
        }
        params[i] =  param;
    }

    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&threads[i], NULL, &Parallel::parseReviews, &params[i]);
    
    for(int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);
}

void* Parallel::parseReviews(void* input){
    std::vector<int> data = *(static_cast<std::vector<int>*>(input));
    int tid     = data[0];
    int start   = data[1];
    int end     = data[2];


    // std::string str_data_reviews(Parallel::data_reviews, start, end - start + 1);
    // std::vector<std::string> lines;
    // Utills::splitByRef(lines, str_data_reviews, "\n");

    // size_t i = 0;
    // for(auto& line : lines){
    //     if((tid == 0 && i == 0) || line.size() < 1){
    //         i++; 
    //         continue;
    //     }
    //     std::vector<std::string> review;
    //     Utills::splitByRef(review, line, ",");
    //     std::vector<int> payload(3);
    //     payload[0] = std::atoi(review[0].c_str());
    //     payload[1] = std::atoi(review[1].c_str());
    //     payload[2] = std::atoi(review[2].c_str());
    //     Parallel::reviews[tid].push_back(payload);
    //     i++;
    // }

    
    std::string data_scope("");
    int scope = 0;
    int* item = new int[3];
    int i = start;
    if(i == 0){
        while(Parallel::data_reviews[i] != '\n') i++;
        i++;
    }
    for(; i <= end; i++){
        if(Parallel::data_reviews[i] == ','){
            item[scope++] = std::stoi(data_scope.c_str());
            data_scope = "";
        }
        else if(Parallel::data_reviews[i] == '\n'){
            item[scope] = std::stoi(data_scope.c_str());
            scope = 0;
            Parallel::reviews[tid].push_back(item);
            item = new int[3];
            data_scope = "";
        } 
        else
            data_scope += Parallel::data_reviews[i];
    }

    pthread_exit(NULL);
}

void Parallel::findBestBook(){
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        for(auto& line : reviews[i]){
            std::unordered_map<int, Book*>::iterator itr_book = Parallel::books.find(line[0]);
            if(itr_book != books.end())
                itr_book->second->updateRates(line[1], line[2]);
        }
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

void Parallel::run(){
    this->paraReadAllData();
   

    this->paraParseBooks();
    this->paraParseReviews();
    this->findBestBook();
    // std::cout << Parallel::books.size() << std::endl;
    // for(int i = 0; i < NUMBER_OF_THREADS; i++){
    //     std::cout << Parallel::reviews[i].size() << std::endl;
    // }

    

  

    // Utills::splitByRef(Parallel::all_books, Parallel::data_book, "\n");
    // Utills::splitByRef(Parallel::all_reviews, Parallel::data_reviews, "\n");
    // // this->paraFilterBooks();

    // size_t i = 0;
    // for(const auto& line : Parallel::all_books){
    //     if(i != 0){
    //         Book* book = new Book(Utills::splitBy(line, ","));
    //         if(book->ifIncludeGener(gener)){
    //             // pthread_mutex_lock (&Parallel::mut_books);
    //             Parallel::books.insert(std::make_pair(book->getBookID(), book));
    //             // pthread_mutex_unlock (&Parallel::mut_books);
    //         }
    //     }
    //     i++;
    // }    

    // Book* max_book = NULL;
    // float max_pop  = -1;

    // // size_t i = 0;
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