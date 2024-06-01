#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <tuple>
#include <filesystem>

float angle_to_kev(float angle){
    float pi = 3.14159265;
    float dspacing = 3.13379;
    float planck = 6.62607015e-34;
    float charge = 1.60217663e-19;
    int speedOfLight = 299792458;
    float wavelength = 2*dspacing*sin(angle*pi/(180));
    float wavelength_m = wavelength*pow(10,-10);
    float energy_kev = planck*speedOfLight/(wavelength_m*charge*1000);
    return energy_kev;
}

string numberFormat(int n, int width){
    string outstring(width,'0');
    string strDigit = to_string(n);
    for (int i = strDigit.length(); i--; i==0){
        outstring[width+i-strDigit.length()] = strDigit[i];
    }
    return outstring;
}

bool isIn(string s, string pattern){
    if (s.find(pattern)!= -1){
        return true;
    }
    else {
        return false;
    }
}



vector<string> splitString(string inputString, string delimiter){
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

float mean1dVector(vector<float> vec){
    float sum = 0;
    for (float item : vec){
        sum += item;
    }
    sum /= vec.size();
    return sum;
}

float max1dVector(vector<float> vec){
    float max = vec[0];
    for (float item : vec){
        if (item > max){
            max = item;
        }
    }
    return max;
}

vector<float> splitStringFloat(string inputString, string delimiter){
    int pos;
    string substring;
    vector<float> outVector;
    string tempString = inputString;
    if (delimiter ==" "){
        while (isIn(tempString,"  ")){
            tempString = tempString.replace(tempString.find("  "),2," ");
        }
        if (tempString.find(" ") == 0){
            tempString = tempString.replace(0,1,"");
        }
    }
    while (true){
        pos = tempString.find(delimiter);
        substring = tempString.substr(0,pos);
        outVector.push_back(stof(substring));
        if (tempString.find(delimiter) == -1){
            break;
        }
        tempString.erase(0,pos+delimiter.length());    
    }
    return outVector;
}

vector<vector<float>> transposeVector(vector<vector<float>> inputVector){

    vector<vector<float>> transposedVector;
    for (int i=0;i < inputVector[0].size();i++){
        transposedVector.push_back({});
        for (int j = 0; j < inputVector.size();j++){
            transposedVector[i].push_back(inputVector[j][i]);
        }
    }
    return transposedVector;
}

void print1dStringVector(vector<string> inputVector){
    cout << "{";
    for (int i = 0; i < inputVector.size(); i++){
        cout << inputVector[i];
        if (i != inputVector.size()-1){
            cout << ',';}
    }
    cout << "}" << endl;
}

void print2dfloatVector(vector<vector<float>> vec){

    cout << "{";
    for (int i = 0; i < vec.size(); i++){
        cout << "{";
        for (int j=0; j < vec[0].size(); j++){
        cout << vec[i][j];
        if (j != vec[0].size()-1){
            cout << ',';}
            }
        cout << "}";
        if (i != vec.size()-1){
            cout << ','<< endl;
        }
    }
    cout << "}" << endl;
}

int getLastScan (string file){
    ifstream input;
    vector<string> scanlines;
    input.open(file);
    if (input.is_open()){
        string s;
        while (getline(input,s)){
        if (isIn(s,"#S ")){
            scanlines.push_back(s);
        }
        }
    }

    int lastscan = stoi(splitString(scanlines.back(), " ")[1]);
    return lastscan;
}

void processFile (string filename, float thetaOffset){

    string fluoCounter = "xmap_roi00";
    string monPattern = "mon_";
    string ion1Pattern = "ion_1";

    vector<string> counterNames = {"ZapEnergy","TwoTheta","mon_3","mon_4","mon_1","ion_1_2","ion_1_3","ion_1_1",fluoCounter, 
    "Theta_offset", "ZapEnergy_offset"};
    vector<string> counterNames_NF; //NF - no fluorescence

    for (int i = 0; i < counterNames.size(); i++){
        string item = counterNames[i];
        if (item != fluoCounter){
            counterNames_NF.push_back(item);
        }
    }
    filesystem::path path(filename);
    string dirname = path.parent_path().string();
    string file = path.filename().string();

    ifstream input;
    string file2 = filename;
    string colname = dirname + "/columns/";
    string basefile = file.replace(file.find(".dat"),4,"");
    string newdir = colname+basefile + "/";

    filesystem::create_directory(colname);
    filesystem::create_directories(newdir);
    input.open(filename);

    string s;
   
    int startLine;
    int endLine;
    bool onscan = false;
    int spectrumCount = -1;

    vector<string> fileLineSplit;
    vector<string> arrayHeader;
    while (getline(input,s)){
        fileLineSplit.push_back(s);
    }
    input.close();
    int line = 0;
    string scanLine;
    string dtLine;
    string headString;
    vector<vector<float>> dataArray;
    bool scanStart = false;
    string columnString = "";
    float timeStep;
    for (string col : counterNames){
        columnString += col + " ";
    }
    for (int i = 0; i < fileLineSplit.size(); i++){

        s = fileLineSplit[i];
        if (isIn(s,"#S") && isIn(s,"zapline")){
            dataArray = {};
            scanLine = s;
            onscan = true;
            spectrumCount++;
            }
        else if (isIn(s,"#D") && onscan){
            dtLine = s;
        }
        else if (isIn(s,"#T") && onscan){
            string timestring = splitString(s," ")[1];
            timeStep = stof(timestring);
            }
        else if (isIn(s,"#L") && onscan){
            startLine = line+1;
            headString = s.replace(s.find("#L "),3,"");
            arrayHeader = splitString(headString," ");
            scanStart = true;
        }
        else if (scanStart && !isIn(s,"#")){
            dataArray.push_back(splitStringFloat(s," "));
        }
        else if (isIn(s,"#C") && onscan){
            onscan = false;
            scanStart = false;
            endLine = line -1;
            vector<vector<float>> filteredArray;
            string filteredColumns ="";
            vector<vector<float>> tArray = transposeVector(dataArray);
            for (int i = 0; i < counterNames.size(); i++){
                string cName = counterNames[i];
                for (int j = 0; j< arrayHeader.size(); j++){
                    if (isIn(arrayHeader[j],cName)){
                        vector<float> columnArray = tArray[j];
                    float max = max1dVector(columnArray);
                    if (max > timeStep && (isIn(cName ,ion1Pattern) || isIn(cName,monPattern))){
                        filteredArray.push_back(columnArray);
                        filteredColumns += cName + " " ;
                    }
                    else if (isIn(cName,fluoCounter) && max > 10){
                        filteredArray.push_back(columnArray);
                        filteredColumns +=  cName + " ";
                    }



                    else if (!isIn(cName,ion1Pattern) && !isIn(cName,monPattern) && cName != fluoCounter) {
                        filteredArray.push_back(columnArray);
                        filteredColumns +=  cName + " ";
                    }
                }
            }
            }
            for (int i = 0; i<counterNames.size();i++){
                string cname = counterNames[i];
                if (cname == "TwoTheta"){
                    vector<float> column = filteredArray[i];
                    vector<float> thetaOffsetArray;
                    vector<float> energyOffsetArray;
                    for (float item : column){
                        thetaOffsetArray.push_back(item+thetaOffset);
                        energyOffsetArray.push_back(angle_to_kev(item+thetaOffset));
                    }
                    filteredArray.push_back(thetaOffsetArray);
                    filteredArray.push_back(energyOffsetArray);
                    filteredColumns += "Theta_offset ZapEnergy_offset";
                    break;
                }
            }

            vector<vector<float>> tFiltered = transposeVector(filteredArray);
            ofstream outfile;
            string number = numberFormat(spectrumCount,4);
            
            string newfilename = newdir+ basefile + "_" + number + ".dat";
            cout << newfilename << "\n";
            outfile.open(newfilename);

            outfile << scanLine << "\n" << dtLine<< "\n" << filteredColumns << "\n";
            
            for (int j=0; j< tFiltered.size();j++ ){
                
                for (int k=0 ; k< tFiltered[0].size(); k++){
                    outfile << tFiltered[j][k] << " ";
                }
                outfile << "\n";
            }
            outfile.close();

        }

        line++;
    }
     
    
}


void regrid(vector<float> grid, vector<float> x, vector<float> y){

}
