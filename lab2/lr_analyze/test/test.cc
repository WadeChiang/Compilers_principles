#include <bits/stdc++.h>

using namespace std;

int main() {
    vector<int> v;
    for (size_t i = 0; i < 7; i++)
    {
        int index = v.end() - v.begin();
        v.push_back(i);
        cout << v[index] << endl;
    }

    return 0;
}