#include <algorithm>
#include <iostream>
using namespace std;

int main(){
   vector<int>my_vec={10, 20};
   int values[] = { 1, 2, 3, 4};   

   ostream_iterator<int> output(cout, " ");
   auto it = inserter(my_vec, my_vec.begin());

   copy(my_vec.begin(), my_vec.end(), output);
   cout << endl;

   *it = 0;
   *it = 1;
   copy(my_vec.begin(), my_vec.end(), output);

   cout << endl;
 
   auto iter = inserter(my_vec, my_vec.begin());
   copy(values, values+2, iter);

   copy(my_vec.begin(), my_vec.end(), output);
}
