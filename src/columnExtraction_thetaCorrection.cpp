#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <tuple>
#include "columnExtractionFunctions.cpp"
#include <map>
#include <boost/program_options.hpp>
#include <filesystem>
using namespace std;
namespace po = boost::program_options;

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
            fileScanMap[file] = noscans;
            if (noscans == 0){
                continue;
            }
            cout << "total scans " << noscans <<"\n";
            rgdir = processFile(file,thetaOffset);
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

    po::options_description desc("Allowed options");

    desc.add_options()
        ("help", "produce help message")
        ("thetaOffset,to", po::value<float>(), "set theta offset on monochromator (default 0)")
        ("directory", po::value<string>(), "directory to run in")
        ;

    po::positional_options_description p;
    p.add("directory",0);


    po::variables_map vm;
    float thetaOffset;

    po::store(po::command_line_parser(argc,argv).
    options(desc).positional(p).run(), vm);
    po::notify(vm);
    if (vm.count("help")){
        cout << desc << endl;
        return 0;
    }

    if (vm.count("thetaOffset")){
        thetaOffset = vm["thetaOffset"].as<float>();
    }
    else if (vm.count("to")){
        thetaOffset = vm["to"].as<float>();
    }
    else {
        thetaOffset = 0;
    }

    cout << "thetaOffset=" << thetaOffset << endl;

    string directory;
    if (vm.count("directory")){
    directory = vm["directory"].as<string>();
    }
    else {
    directory = ".";
    }
    cout << directory << endl;

    while (true){
        bool foundFiles = searchDir(directory, thetaOffset);
        if (foundFiles){
            cout << "looking for new files\n";}
    }
}

