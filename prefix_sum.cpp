#include <bits/stdc++.h>
using namespace std;


void   prefix_sum(vector<int> a)
{
    for (int i = 1; i < a.size(); ++i)
        a[i] += a[i-1];

    int q;
    cin >> q;
    while (q--)
    {int i, j;
    cin >> i >> j;
    if (i == 0){ std::cout << a[j] << "  "; continue;}
    --i;
    std::cout <<  a[j] - a[i] << "   ";
    }
}

int main()
{
    vector<int> a = {5,2,7,1,0,3,2};
    int q;

    prefix_sum(a) ;//<< "   ";
  return 0;

}