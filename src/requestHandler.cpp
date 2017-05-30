//
// Created by sofia on 30.05.17.
//#
#include "requestHandler.h"
#include <jansson.h>
#include "film.h"
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
using namespace std;
const char* filePath = "data/data.txt";
vector <string> parseRequest(string req){
    string delim = "/";
    string cmd = req.substr(0,req.find(delim));
    vector <string>parsedReq;
    parsedReq.push_back(cmd);
    string path  = req.substr(req.find(delim),string::npos);
    parsedReq.push_back(path);
    return parsedReq;

}
int analyzeRequest(vector<string> req){
    string cmd = req.at(0);
    string path = req.at(1);
    if(cmd.compare("GET") == 0){
        if(path.compare("/") == 0) return ROOT;
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
string response(vector<Film> films,int status, vector <string> req){
    switch (status){

        case ROOT: return  rootResponse(films);
        case FAVORITES: return responseFavorites(films);
        case FAVORITES_ID:
            return responseFavoritesId(films, req);
        case FAVORITES_KEY: return  responseFavoritesKey(films,req);
        case FILE_INF: return responseFile();
        case FILE_DATA: return responseFileData();
        default:
            return "HTTP/1.1 404 not found";
    }

}

string responseFavorites(vector<Film>films){
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
    return response + jsonStr;
}


string rootResponse(vector <Film> films){
    string response = "HTTP/1.1 200 OK\nConnection: Closed\nContent-Type:text/html;\r\n\r\n";
    time_t timing;
    time(&timing);
    auto timeInfo = localtime(&timing);
    auto json = json_object();
    json_object_set_new(json,"title", json_string("MyServ"));
    json_object_set_new(json,"developer", json_string("Sofia Agafonova"));
    json_object_set_new(json,"time",json_string(asctime(timeInfo)));

    char* jsonStr = json_dumps(json,JSON_INDENT(2));
    return response + jsonStr ;
}

string responseFavoritesId(vector <Film> films, vector<string> req){

    string favoritesResponse = "HTTP/1.1 200 OK\n"
            "Connection: Closed\r\n\r\n";

    auto str = req.at(1);
    size_t last_index = str.find_last_not_of("0123456789");
    string result = str.substr(last_index + 1);
    unsigned int index = stoi(result);
    if(index < 0 || index >= films.size()){
        return "HTTP/1.1 404 not found";
    }

    json_t * json = json_object();
    json_t * arr = json_array();

    Film movie = films.at(index);
    json_t * movieJ = json_object();
    json_object_set_new(movieJ, "title", json_string(movie.title.c_str()));
    json_object_set_new(movieJ, "rating", json_real(movie.rating));
    json_object_set_new(movieJ, "year", json_integer(movie.year));
    json_array_append(arr, movieJ);

    json_object_set_new(json, "films", arr);
    char * jsonString = json_dumps(json, JSON_INDENT(2));

    favoritesResponse += jsonString;

    return favoritesResponse;

}
string responseFavoritesKey(vector <Film> films, vector<string>req){
    string favoritesResponse = "HTTP/1.1 200 OK\n"
            "Connection: Closed\r\n\r\n";

    auto str = req.at(1);
    string delim = "?";
    string delim1 ="=";
    string cmd = str.substr(str.find(delim)+1, str.find(delim1));

    if(cmd.compare("rating")!=0) return   "HTTP/1.1 404 not found";
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

        }


    }
    json_object_set_new(json, "films", arr);


    char * jsonString = json_dumps(json, JSON_INDENT(2));

    favoritesResponse += jsonString;


    return favoritesResponse;
}
string responseFile(){
    string fileDataResponse = "HTTP/1.1 200 OK\n";
    fileDataResponse += "Connection: Closed\n"
            "Content-Type: text/html; charset=iso-8859-1\r\n\r\n";

    char* data = readFile(filePath);
    json_t * json = json_object();
    json_object_set_new(json, "name", json_string( "text.txt"));
    json_object_set_new(json, "size", json_integer(fileSize(filePath)));
    json_object_set_new(json, "content", json_string(data));

    char * jsonString = json_dumps(json, JSON_INDENT(2));
    fileDataResponse += jsonString;
    return fileDataResponse;
}
string responseFileData(){
    string fileDataResponse = "HTTP/1.1 200 OK\n";
    fileDataResponse += "Connection: Closed\n"
            "Content-Type: text/html; charset=iso-8859-1\r\n\r\n";

    char* text = readFile(filePath);
    std::string data(text);
    istringstream iss(data);
    vector<std::string> tokens{istream_iterator<std::string>{iss},istream_iterator<std::string>{}};
    json_t * json = json_object();
    json_t * arr = json_array();
    for (auto str : tokens) // access by reference to avoid copying
    {
        json_t * numberJ = json_object();
        json_object_set_new(numberJ,  "number", json_integer(stoi(str)));
        json_array_append(arr, numberJ);
    }

    json_object_set_new(json, "numbers", arr);
    char * jsonString = json_dumps(json, JSON_INDENT(2));
    fileDataResponse += jsonString;
return fileDataResponse;

}
char* readFile(const char* fileName){
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
long fileSize(const char* fileName){
    FILE* file = fopen(fileName,"r");
    if(file == NULL) return-1;
    fseek(file,0,SEEK_END);
    auto size = ftell(file);
    rewind(file);
    if(fclose(file)) return  -1;
    return  size;
}
