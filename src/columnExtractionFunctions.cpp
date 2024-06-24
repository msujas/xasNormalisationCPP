#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <tuple>
#include <filesystem>
#include "vectorMaths.cpp"
using namespace std;

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

vector<float> arrayByHeader(string header, vector<string> columnNames, vector<vector<float>> array2d){
    for (int i = 0; i< columnNames.size(); i++){
        if (columnNames[i] == header){
            return array2d[i];
        }
    }
    throw runtime_error("header not found");
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



vector<float> splitStringFloat(string inputString, string delimiter= " "){
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

int getLastScan (string file){
    ifstream input;
    input.open(file);
    int noscans = 0;
    if (input.is_open()){
        string s;
        while (getline(input,s)){
        if (isIn(s,"#S ") && isIn(s,"zapline")){
            noscans++;
        }
        }
    }
    return noscans;
}

string fluoCounter = "xmap_roi00";
string monPattern = "mon_";
string ion1Pattern = "ion_1";
vector<string> counterNames = {"ZapEnergy","TwoTheta","mon_3","mon_4","mon_1","ion_1_2","ion_1_3","ion_1_1",fluoCounter};
string toName = "Theta_offset";
string zeName = "ZapEnergy_offset";
string processFile (string filename, float thetaOffset, int startScan = 0){




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
                    filteredColumns += toName + " " + zeName;
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
                    outfile << tFiltered[j][k];
                    if (k!=tFiltered[0].size()-1){
                        outfile << " ";
                    }
                }
                outfile << "\n";
            }
            outfile.close();

        }

        line++;
    }
    return newdir;
    
}


void regridFiles(string directory){
    cout << "regridding " << directory << endl;
    string newdir = directory+"/"+"regrid/";
    filesystem::create_directory(newdir);

    ifstream input;
    int maxScanLength = 0;

    vector<vector<float>> energies;
    vector<vector<float>> monitors;
    vector<vector<float>> i1s;
    vector<vector<float>> fluos;
    vector<bool> fluoBools;
    vector<vector<float>> mutrans;
    vector<string> scanLines;
    vector<string> dtLines;
    vector<string> filenames;
    int maxScanIndex = 0;
    int maxScanDifference = 30;
    int fileno = 0;
    for (auto entry: filesystem::directory_iterator(directory)){
        
        string fname = entry.path().string();
        if (!isIn(fname,".dat")){
            continue;
        }

        filenames.push_back(entry.path().filename().string());
        input.open(fname);
        vector<string> lines;
        string line;
        int count = 0;
        while (getline(input, line)){
            lines.push_back(line);
        }
        input.close();
        scanLines.push_back(lines[0]);
        dtLines.push_back(lines[1]);
        string headerString = lines[2];

        vector<string> headerArray = splitString(headerString, " ");
        vector<vector<float>> dataArray;
        for (int i=3;i<lines.size(); i++){
            dataArray.push_back(splitStringFloat(lines[i], " "));
        }
        vector<vector<float>> tArray = transposeVector(dataArray);
        string monCounter;
        string i1counter;
        bool fluo = false;
        for (string cName: headerArray){
            if (isIn(cName, monPattern)){
                monCounter = cName;
            }
            else if (isIn(cName,ion1Pattern)){
                i1counter = cName;
            }
            else if (cName == fluoCounter){
                fluo=true;
            }
        }
        fluoBools.push_back(fluo);
        vector<float> monArray = arrayByHeader(monCounter,headerArray, tArray);
        vector<float> i1Array = arrayByHeader(i1counter, headerArray, tArray);
        vector<float> energyArray= arrayByHeader(zeName, headerArray, tArray);
        vector<float> fluoArray;
        if (fluo){
            vector<float> fluoArray = arrayByHeader(fluoCounter,headerArray,tArray);
        }
        int minindex = minIndex(energyArray); //because scans sometimes go backwards for a few points initially before going forward
        energyArray = fromIndex(energyArray,minindex);
        i1Array = fromIndex(i1Array,minindex);
        monArray = fromIndex(monArray,minindex);
        vector<float> muF;
        if (fluo){
            fluoArray = fromIndex(fluoArray, minindex);
            for (int i=0; i< fluoArray.size();i++){
                muF.push_back(fluoArray[i]/monArray[i]);
            }
            fluos.push_back(muF);
        }
        else {
            fluos.push_back({});
        }
        if (energyArray.size() > maxScanLength){
            maxScanLength = energyArray.size();
            maxScanIndex = fileno;
        }
        vector<float> muT;
        for (int i= 0; i<energyArray.size(); i++){
            muT.push_back(log(monArray[i]/i1Array[i]));
        }
        mutrans.push_back(muT);
        energies.push_back(energyArray);
        monitors.push_back(monArray);
        i1s.push_back(i1Array);
        fileno++;
    }

    vector<float> longestScan = energies[maxScanIndex];
    float gridStart = longestScan[0];
    float gridStop = longestScan[longestScan.size()-1];
    int gridSize = longestScan.size();
    float gridStep = ((gridStop-gridStart)/(gridSize-1));

    for (int i=0; i < energies.size(); i++){
        
        bool fluoBool = fluoBools[i];
        vector<float> energy = energies[i];
        float currentGridStart = gridStart;
        float currentGridStop = gridStop;
        if (energy.size() < maxScanLength - maxScanDifference){
            continue;
        }
        while (currentGridStop > energy[energy.size()-1]){
            currentGridStop -= gridStep;
        }
        while (currentGridStart < energy[0]){
            currentGridStart += gridStep;
        }

        
        vector<float> grid = range(currentGridStart,currentGridStop, gridStep);

        vector<float> regridded = regrid(grid,energy, mutrans[i]);
        
        vector<float> regriddedF;
        vector<vector<float>> outArray;
        
        if (fluoBool){
            regriddedF = regrid(grid,energy, fluos[i]);
             outArray = {grid,regridded, regriddedF};
        }
        else {
            
            outArray = {grid,regridded};
        }
        vector<vector<float>> toutArray = transposeVector(outArray);

        string newfile = newdir+filenames[i];
        
        ofstream outfile;
        outfile.open(newfile);
        outfile << scanLines[i] << "\n";
        outfile << dtLines[i] << "\n";
        if (!fluoBool){
            outfile << "ZapEnergy_offset muT\n";
        }
        else {
            outfile << "ZapEnergy_offset muT muF\n";
        }
        for (int j=0; j < toutArray.size();j++){
            for (int k=0; k < toutArray[0].size();k++){
                outfile << toutArray[j][k] << " ";
            }
            outfile << "\n";
        }
        outfile.close();
    }
}

