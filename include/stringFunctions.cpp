#include <vector>
#include <string>
#define stringFunctions

using namespace std;

bool isIn(string s, string pattern){
    if (s.find(pattern)!= -1){
        return true;
    }
    else {
        return false;
    }
}

vector<string> splitString(string inputString, string delimiter=" "){
    int pos;
    string substring;
    vector<string> outVector;
    string  tempString = inputString;
    if (delimiter ==" "){
        while (isIn(tempString,"  ")){ // check if 2 consecutive spaces in string, replace with 1
            tempString = tempString.replace(tempString.find("  "),2," ");

        }
        if (tempString.find(" ") == 0){
            tempString = tempString.replace(0,1,"");
        }
    }
    while (true){
        pos = tempString.find(delimiter);
        substring = tempString.substr(0,pos);
        outVector.push_back(substring);
        if (tempString.find(delimiter) == -1){
            break;
        }
        tempString.erase(0,pos+delimiter.length());    
    }
    return outVector;
}

