#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

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
        for (int j=0; j < vec[i].size(); j++){
        cout << vec[i][j];
        if (j != vec[i].size()-1){
            cout << ',';}
            }
        cout << "}";
        if (i != vec.size()-1){
            cout << ','<< endl;
        }
    }
    cout << "}" << endl;
}

vector<float> range(float start, float stop, float step){
    float element = start;
    vector<float> array ;
    while (element < stop){
        array.push_back(element) ;
        element += step;
    }
    return array;
}



float interpolate(float x, float x1, float x2, float y1, float y2){
    float n = (x1+x2)/x;
    return (y1+y2)/n;
}

int findGridIndex(float x, vector<float> array){
    if (x < array[0]){
        throw runtime_error("grid value below array range");
    }
    else if (x > array[array.size()-1]){
        throw runtime_error("grid value above array range");
    }
    for (int i; i < array.size();i++){
        if (i > x){
            return i-1;
        }
    }
    throw runtime_error("grid value above array range");
}

vector<float> regrid(vector<float> grid, vector<float> x, vector<float> y){
    vector<float> yregrid;
    for (float item :  grid){
        int index = findGridIndex(item, x);
        float yinterp = interpolate(item,x[index],x[index+1],y[index],y[index+1]);
        yregrid.push_back(yinterp);
    }
    return yregrid;
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

float min1dVector(vector<float> vec){
    float min = vec[0];
    for (float item : vec){
        if (item < min){
            min = item;
        }
    }
    return min;
}

int minIndex(vector<float> vec){
    int minindex = 0;
    for (int i=0; i<vec.size();i++){
        if (vec[i] < vec[minindex]){
            minindex = i;
        }
    }
    return minindex;
}

int maxndex(vector<float> vec){
    int maxindex = 0;
    for (int i=0; i<vec.size();i++){
        if (vec[i] > vec[maxindex]){
            maxindex = i;
        }
    }
    return maxindex;
}

vector<float> fromIndex(vector<float> vec, int index){
    vector<float> newvec;
    for (int i=index; i < vec.size(); i++){
        newvec.push_back(vec[i]);
    }
    return newvec;
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
