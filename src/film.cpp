#include "film.h"
#include <string>
using namespace std;
Film::Film(string title, int year, float rating) {
    this->title = title;
    this->year=year;
    this->rating = rating;
}


