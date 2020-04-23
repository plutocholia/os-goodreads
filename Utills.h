#ifndef UTILLS_H
#define UTILLS_H

#include <vector>
#include <string>

#define DIR_DATASETS    "./datasets/"
#define DIR_REVIEW      "./datasets/reviews.csv"
#define DIR_BOOKS       "./datasets/books.csv"

class Utills
{
public:
    static void removeSpace(std::string&);
    static std::vector<std::string> splitBy(
        std::string, const std::string&
    );
    static void splitByRef(
        std::vector<std::string>&,
        const std::string&, const std::string&
    );
    static void printStringVector(
        const std::vector<std::string>& vec
    );
    static void printPairStringVector(
        const std::vector<std::pair<std::string, std::string> >&
    );
    static void printPairIdStringVector(
        const std::vector<std::pair<int, std::string> >&
    );
};


#endif // !UTILLS_H