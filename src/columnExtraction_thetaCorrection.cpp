#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <tuple>
#include "columnExtractionFunctions.cpp"
#include <map>
#include <boost/program_options.hpp>
#include <filesystem>
using namespace std;
namespace po = boost::program_options;



int main(int argc, char *argv[]){

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("thetaOffset", po::value<float>(), "set theta offset on monochromator (default 0)")
        ("directory", po::value<string>(), "directory to run in (default \".\")")
        ;

    po::variables_map vm;
    float thetaOffset;

    po::store(po::parse_command_line(argc,argv,desc),vm);
    po::notify(vm);
    if (vm.count("help")){
        cout << desc << endl;
        return 0;
    }

    if (vm.count("thetaOffset")){
        thetaOffset = vm["thetaOffset"].as<float>();
        
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

//tuple<vector<vector<float>> , vector<string>> arrayandHeaders = datToVector(file, counterNames);
    map<string,filesystem::file_time_type> fileTimeMap;
    map<string, int> fileScanMap;
    for (auto entry : filesystem::directory_iterator(directory)){

        string file = entry.path().string();
        string dirname = entry.path().parent_path().string();
        
        if (isIn(file,".dat")){
            cout << file << endl;
            int noscans = getLastScan(file);
            fileScanMap[file] = noscans;
            cout << "total scans " << noscans <<"\n";
            processFile(file,thetaOffset);
        }
        filesystem::file_time_type mtime = filesystem::last_write_time(entry);
        fileTimeMap[file] = mtime;
        
    }

    for (auto entry : filesystem::directory_iterator(directory+"/columns/")){
        cout << entry.path().string() << endl;
        if (filesystem::is_directory(entry)){
            string rgdir = entry.path().string();
            regridFiles(rgdir+"/");
            }
        }
}