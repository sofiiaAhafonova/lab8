//
// Created by sofia on 30.05.17.
//

#ifndef LAB8_FILMS_H
#define LAB8_FILMS_H

#include <iostream>
#include <string>
using namespace std;
class Film{
public:
    int id;
    string title;
    int  year;
    float  rating;
/**
  @brief  class constructor
  @param title - film title 
  @param year - year of creating film
  @param rating - rating of the film
 
 */
    Film(string title, int year , float rating);

};
#endif //LAB8_FILMS_H
