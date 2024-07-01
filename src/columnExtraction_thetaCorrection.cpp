#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <tuple>
#include <map>
//#include <stringFunctions.cpp>
#include "columnExtractionFunctions.cpp"
#ifndef stringFunctions
#include "stringFunctions.cpp"
#endif
//#include <boost/program_options.hpp>
#include <filesystem>
#include "argParser.cpp"


using namespace std;
//namespace po = boost::program_options;

map<string,filesystem::file_time_type> fileTimeMap;
map<string, int> fileScanMap;


bool searchDir(string directory, float thetaOffset){
    string rgdir;
    bool foundFiles = false;
    vector<bool> foundFilesList;
    tuple<bool, map<string,filesystem::file_time_type>>  outTuple;
    for (auto entry : filesystem::directory_iterator(directory)){
        
        string file = entry.path().string();
        string dirname = entry.path().parent_path().string();
        filesystem::file_time_type mtime = filesystem::last_write_time(entry);
        if (filesystem::is_directory(entry.path()) && !isIn(file,"columns") && !isIn(file,"regrid")){
            foundFilesList.push_back(searchDir(file, thetaOffset));
        }
        if (fileTimeMap[file] == mtime){
            continue;
        }
        if (isIn(file,".dat")){
            cout << file << endl;
            int noscans = getLastScan(file);
            
            if (noscans == 0){
                continue;
            }
            cout << "total scans " << noscans <<"\n";
            rgdir = processFile(file,thetaOffset, fileScanMap[file]);
            fileScanMap[file] = noscans;
            fileTimeMap[file] = mtime;
            regridFiles(rgdir);
            foundFiles = true;
        }
    }
    if (foundFiles){
    return foundFiles;}
    else {
        for (bool b : foundFilesList){
            if (b){
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]){
    
    ArgParser ap;
    ap.addKW("thetaOffset","0","to");
    ap.addPositional("directory",".");
    ap.readArguments(argc,argv);
    float thetaOffset = ap.getArg<float>("thetaOffset");
    
    cout << "thetaOffset=" << thetaOffset << endl;

    string directory = ap.getArg<string>("directory");
    
    cout << directory << endl;
    while (true){
        bool foundFiles = searchDir(directory, thetaOffset);
        if (foundFiles){
            cout << "looking for new files\n";}
        _sleep(1000);
    }
}

