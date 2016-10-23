#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#define SUB_A 3
#define SUB_B 8

ifstream fin;
ofstream posOut[SUB_A];
ofstream negOut[SUB_B];
int posLine[SUB_A] = {0,0,0};
int negLine[SUB_B] = {0,0,0,0,0,0,0,0};
string tempname;
string token;

int main( int argc, char *argv[])
{
	int i, pos = 0, neg = 0;
	ofstream *curOut;

	if( argc != 3 )
	{
		cout << "random <input> <subGeneralName>\n";
		return 1;
	}

	fin.open(argv[1]);
	tempname = argv[2];

	if( !fin )
	{
		cout << "Cannot open input file\n";
		return 1;
	}

	for( i = 0; i < SUB_A; ++i )
	{
		posOut[i].open((tempname+char('0'+i)+".pos").c_str());
		if( !posOut[i] )
		{
			cout << "Cannot open output file\n";
			return 1; 
		}
	}

	for( i = 0; i < SUB_B; ++i )
	{
		negOut[i].open((tempname+char('0'+i)+".neg").c_str());
		if(!negOut[i])
		{
			cout << "Cannot open output file\n";
			return 1;
		}
	}

	i = 0;

	while(getline(fin,token) )
	{
		if( token.at(0) == 'A' )
		{
			curOut = &posOut[pos];
			*curOut << (posLine[pos]==0?"+1":"\n+1"); 
			++posLine[pos];
			pos = (pos+1) % SUB_A;
		}
		else //if( token.at(0) == '-' )
		{
			curOut = &negOut[neg];
			*curOut << (negLine[neg]==0?"-1":"\n-1"); 
			++negLine[neg];
			neg = (neg+1) % SUB_B;
		}

		char ans[500000]={0};

		strcpy(ans,token.c_str());

	//	cout<<"**"<<ans<<"**"<<endl;

		char *t = strtok(ans," ");

		//cout<<"!!"<<t<<"!!"<<endl;

		t=strtok(NULL,"\n");

		//cout<<"&&"<<t<<"&&"<<endl;
		*curOut << ' ' <<t;

	}
	return 0;
}
