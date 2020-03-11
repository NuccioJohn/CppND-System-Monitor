#include <string>
#include <vector>
#include <iostream>

#include "format.h"

using std::string;
using std::vector;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
/*
int n_digits(int number){
    return to_string(number).length();
}

string format_time(int t){
    string st = to_string(t);
    return (n_digits(t) < 2) ? "0" + st : st;
}

string prettify(vector<int> v){
    string placeholder = "";
    for(std::size_t i = 0; i < v.size(); i++){
        placeholder.append(format_time(v[i]));
        if(i != (v.size() -1)){
            placeholder.append(":");
        }
    }
    return placeholder;
}
*/
string Format::ElapsedTime(long seconds) { 
    /*
    int hour_in_seconds = 3600;
    int hours = seconds / hour_in_seconds;
    int t_seconds = seconds % hour_in_seconds;
    int minutes = t_seconds / 60;
    int e_seconds = t_seconds % 60;

    vector<int> c{hours, minutes, e_seconds};
    return prettify(c);*/
    int hour = 3600;
    int hours =seconds / hour;
    int totalSeconds = seconds % hour;
    int minutes = totalSeconds / 60;
    int calcSeconds = totalSeconds % 60;
    
    vector<int> output{hours, minutes, calcSeconds};
    
    std::string str;
    for (std::size_t i = 0; i < output.size(); i++){
        string strTemp = std::to_string(output[i]);
        //adds a zero if it needs one
        if(strTemp.length() < 2){
            strTemp += "0";
        }
        str += strTemp;
        if(i != (output.size()-1)){
            str += ":";
        }
        
    }
    return str; 
}