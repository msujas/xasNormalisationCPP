#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <tuple>
#include "columnExtractionFunctions.cpp"
#include <filesystem>



int main(int argc, char *argv[]){
float thetaOffset = 0;

string directory; 
if (argc == 2){
 directory = argv[1];
}
else {
directory = ".";
}
cout << directory << endl;




//tuple<vector<vector<float>> , vector<string>> arrayandHeaders = datToVector(file, counterNames);
for (auto entry : filesystem::directory_iterator(directory)){
    string file = entry.path().string();
    string dirname = entry.path().parent_path().string();
    
    
    if (isIn(file,".dat")){
        cout << file << endl;
        int noscans = getLastScan(file);
        cout << "total scans " << noscans <<"\n";
        processFile(file,thetaOffset);
    }
}

for (auto entry : filesystem::directory_iterator(directory+"/columns/")){
    string rgdir = entry.path().string();
    regridFiles(rgdir+"/");
}

}