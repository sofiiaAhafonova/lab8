//
// Created by sofia on 30.05.17.
//

#ifndef LAB8_REQUESTHANDLER_H
#define LAB8_REQUESTHANDLER_H

#include <vector>
#include <string>
#include "film.h"
/**
 @brief emum for response analyze
 */
using namespace std;
enum {
    WRONG_REQUEST,
    WRONG_ID,
    ROOT,
    FAVORITES,
    FAVORITES_ID,
    FAVORITES_KEY,
    FILE_INF,
    FILE_DATA
};
/**
  @brief parses string into string vector
  @param req = request string for parsing
  @returns string vector with command and path
 */
vector <string> parseRequest(string req);
/**
  @brief analyzes string vector
  @param req - string vector contains parsed request
  @returns int represented command
 */
int analyzeRequest(vector<string> req);
/**
  @brief decides what  response to make
  @param films - vector of Film objects
  @param status - int represented request command
  @param req - string vector with command and path
  @returns response string
 */
string response(vector<Film> films,int status, vector <string> req);
/**
  @brief creates root response 
  @param films - vector of Film objects
  @returns response on /
 */
string rootResponse(vector <Film> films);
/**
  @brief creates  response of favorite films
  @param films - vector of Film objects
  @returns response on /favorites
 */
string responseFavorites(vector<Film>films);
/**
  @brief creates  response of favorite films by if
  @param films - vector of Film objects
  @param req - string vector with command and path
  @returns response on /favorites/{id}
 */
string responseFavoritesId(vector <Film> films, vector<string> req);
/**
  @brief  creates response with films which searches by value in key-field
  @param films - vector of Film objects
  @param req - string vector with command and path
  @returns response on /favorites?{key}={value}
 */
string responseFavoritesKey(vector <Film> films, vector<string>req);
/**
  @brief creates response from file information
 
  @returns response on /file
 */
string responseFile();
/**
  @brief creates response from file data

  @returns response on /file
 */
string responseFileData();
/**
  @brief  reads file
  @param fileName-  name of the file
  @returns file content
 */
char* readFile(const char* fileName);
/**
  @brief  calculate file size 
  @param fileName-  name of the file
  @returns file size
 */
long fileSize(const char* fileName);
#endif //LAB8_REQUESTHANDLER_H
