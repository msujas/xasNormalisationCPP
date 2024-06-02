#include "testLib.cpp"
#include <iostream>
#include "vectorMaths.cpp"
#include "boost/lambda/lambda.hpp"
#include <iterator>
#include <algorithm>
using namespace std;

int main() {
    int a = 5;
    int b = 6;
    int c = add2(a,b);
    cout << c << endl;
    vector<vector<float>> v = {{1,2,3},{1,2}};
    print2dfloatVector(v);
     using namespace boost::lambda;
    typedef istream_iterator<int> in;

    for_each(
        in(cin), in(), cout << (_1 * 3) << "\n" );
}