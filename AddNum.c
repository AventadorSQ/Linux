#include<iostream>
using namespace std;

class ADD
{
    public:
	int Add(int n, int m)
	{
	    return (n^m)^((n&m)<<1);
	}

};
int main()
{
    int a,b;
    ADD num;
    cin>>a>>b;
    cout<<num.Add(a,b)<<endl;
    return 0;
}
