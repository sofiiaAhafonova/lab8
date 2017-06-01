//
// Created by sofia on 30.05.17.
//#
#include "requestHandler.h"
#include <jansson.h>
#include <string.h>
#include "film.h"
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <fstream>

using namespace std;

typedef struct Request Request;
const char* filePath = "../data/data.txt";
requestHandler::requestHandler(){
}
requestHandler::~requestHandler(){
}
 requestHandler:: Request requestHandler:: parseRequest(string req){

    string delim = " ";
    string delim1 = "/";
    auto found = req.find(delim);
    string cmd = req.substr(0,found);
     Request parsedReq;
    parsedReq.method = cmd;
    auto found2 = req.find("H",0) -5;
    string path  = req.substr(req.find(delim1), found2);
    parsedReq.path = path;
   
    return parsedReq;

}
int requestHandler:: analyzeRequest(Request req){
    string cmd = req.method;
    string path = req.path;
    if(cmd.compare("GET") == 0){
        if(!path.compare("/") ) return ROOT;
        if(path.compare("/favorites") == 0) return FAVORITES;
        if(path.compare("/file") == 0) return FILE_INF;
        if(path.compare("/file/data") == 0) return FILE_DATA;
        auto f=path.find("/favorites?") ;
        if(f!=string::npos) return FAVORITES_KEY;
        f=path.find("/favorites/") ;
        if(f!=string::npos) return FAVORITES_ID;
    }
    return WRONG_REQUEST;
}
string requestHandler:: response(vector<Film> films,int status, Request req){
    switch (status){

        case ROOT: return  rootResponse(films);
        case FAVORITES: return responseFavorites(films);
        case FAVORITES_ID:
            return responseFavoritesId(films, req);
        case FAVORITES_KEY: return  responseFavoritesKey(films,req);
        case FILE_INF: return responseFile();
        case FILE_DATA: return responseFileData();
        default:
            return "HTTP/1.1 404 not found\n";
    }

}

string requestHandler:: responseFavorites(vector<Film>films){
    auto json_obj = json_object();
    auto j_array = json_array();
    for(unsigned int i = 0; i < films.size(); i++){
        Film movie = films.at(i);
        auto json_movie = json_object();
        json_object_set_new(json_movie,"id",json_integer(i));
        json_object_set_new(json_movie,"title",json_string(movie.title.c_str()));
        json_object_set_new(json_movie,"year",json_integer(movie.year));
        json_object_set_new(json_movie,"rating",json_real(movie.rating));
        json_array_append(j_array,json_movie);
    }
    json_object_set_new(json_obj,"films",j_array);
    char* jsonStr = json_dumps(json_obj, JSON_INDENT(2));
    string response = "HTTP/1.1 200 OK\nConnection: Closed\r\n\r\n";
    response+=jsonStr;
    free(jsonStr);
    return response;
}


string requestHandler:: rootResponse(vector <Film> films){
    string response = "HTTP/1.1 200 OK\nConnection: Closed\nContent-Type:text/html;\r\n\r\n";
    time_t timing;
    time(&timing);
    auto timeInfo = localtime(&timing);
    auto json = json_object();
    json_object_set_new(json,"title", json_string("MyServ\n"));
    json_object_set_new(json,"developer", json_string("Sofia Agafonova\n"));
    json_object_set_new(json,"time",json_string(asctime(timeInfo)));

    char* jsonStr = json_dumps(json,JSON_INDENT(2));
    response+=jsonStr;
    free(jsonStr);
    return response  ;
}

string requestHandler:: responseFavoritesId(vector <Film> films, Request req){

    string favoritesResponse = "HTTP/1.1 200 OK\n"
            "Connection: Closed\r\n\r\n";

    auto str = req.path;
    size_t last_index = str.find_last_not_of("0123456789");
    string result = str.substr(last_index + 1);
    unsigned int index = stoi(result);
     string error = "HTTP/1.1 404 not found\n";
    if(index < 0 || index >= films.size()){
        return error;
    } 
   
    Film movie = films.at(index);
    json_t * movieJ = json_object();
    json_object_set_new(movieJ, "title", json_string(movie.title.c_str()));
    json_object_set_new(movieJ, "rating", json_real(movie.rating));
    json_object_set_new(movieJ, "year", json_integer(movie.year));
    

   
    char * jsonString = json_dumps(movieJ, JSON_INDENT(2));

    favoritesResponse += jsonString;
    free(jsonString);
    json_decref(movieJ);
 
    return favoritesResponse;

}
string requestHandler::  responseFavoritesKey(vector <Film> films,  Request req){
    string favoritesResponse = "HTTP/1.1 200 OK\n"
            "Connection: Closed\r\n\r\n";

    auto str = req.path;
    string delim = "?";
    string delim1 ="=";
    string cmd = str.substr(11, 6);
  
    if(cmd.compare("rating")!=0)  return "HTTP/1.1 404 not found\n";
    string value = str.substr(str.find(delim1)+1,string::npos);

    float result = atof(value.c_str());

    json_t * json = json_object();
    json_t * arr = json_array();
    for (unsigned int i = 0; i < films.size(); i++) {
        Film  movie = films.at(i);

        if(movie.rating == result){
            json_t * movieJ = json_object();
            json_object_set_new(movieJ, "title", json_string(movie.title.c_str()));
            json_object_set_new(movieJ, "rating", json_real(movie.rating));
            json_object_set_new(movieJ, "year", json_integer(movie.year));
            json_array_append(arr, movieJ);
            json_decref(movieJ);

        }


    }
    json_object_set_new(json, "films", arr);


    char * jsonString = json_dumps(json, JSON_INDENT(2));

    favoritesResponse += jsonString;   
    free(jsonString);
    json_decref(arr);
    json_decref(json);
    return favoritesResponse;
}
string requestHandler::  responseFile(){
    string fileDataResponse = "HTTP/1.1 200 OK\n";
    fileDataResponse += "Connection: Closed\n"
            "Content-Type: text/html; charset=iso-8859-1\r\n\r\n";

    char* data = readFile(filePath);
    data[fileSize(filePath)-1]='\0';
    json_t * json = json_object();
    json_object_set_new(json, "name", json_string( "text.txt"));
    json_object_set_new(json, "size", json_integer(fileSize(filePath)));
    json_object_set_new(json, "content", json_string(data));
	
    char * jsonString = json_dumps(json, JSON_INDENT(2));
    fileDataResponse += jsonString;
    free(jsonString);
    
    free(data);
    return fileDataResponse;
}
bool isDigits(string word){
	for(unsigned int i = 0; i < word.size(); ++i){
		if(!isdigit(word[i])) return false;
	}	
	return true;
}
string requestHandler:: responseFileData(){
    string fileDataResponse = "HTTP/1.1 200 OK\n";
    fileDataResponse += "Connection: Closed\n"
            "Content-Type: text/html; charset=iso-8859-1\r\n\r\n";

    //char* text = readFile(filePath);
ifstream fin("../data/data.txt");
 string line; 
json_t * json = json_object();
    json_t * arr = json_array();
while (getline(fin, line)){
    
    stringstream extract; // extract words by words;
    extract << line; //enter the sentence that we want to extract word by word
    string word = "";
     
    //while there are words to extract
    while(!extract.fail())
    {
	extract >> word; //extract the word
	if(isDigits(word)){//if the string is full of digits
			
		stringstream convert; //converts strings to ints
		convert << word; //insert the digits into our converter
		int theNumber = 0; //will hold the number inside the string			
			//convert the string number into a int number and place it inside of theNumber variable
		convert >> theNumber;
		 json_array_append(arr, json_integer(theNumber)); 
		word ="";
	}
    }
}

   

    json_object_set_new(json, "numbers", arr);
    char * jsonString = json_dumps(json, JSON_INDENT(2));
    fileDataResponse += jsonString;
    free(jsonString);
    json_decref(arr);
    json_decref(json);
    return fileDataResponse;

}
char* requestHandler:: readFile(const char* fileName){
    FILE* file = fopen(fileName,"r");
    if(file == NULL) return NULL;
    auto size = fileSize(fileName);
    if (size<0)return NULL;
    char* text =(char*) malloc (sizeof(char)*size);
    if(text == NULL) return NULL;
    fread(text,1,size,file);
    if(fclose(file)) return  NULL;
    return text;
}
long requestHandler:: fileSize(const char* fileName){
    FILE* file = fopen(fileName,"r");
    if(file == NULL) return-1;
    fseek(file,0,SEEK_END);
    auto size = ftell(file);
    rewind(file);
    if(fclose(file)) return  -1;
    return  size;
}
