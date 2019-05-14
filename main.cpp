#include <iostream>
#include<bits/stdc++.h>
#include<fstream>
#include<cstdlib>
#include "parser.h"
#include "pass2.h"
#include "numberBases.h"
#include<regex>
using namespace std;
int main()
{

	regex e("\\s*pass2\\s+(.+)\\s*", regex_constants::icase);
	smatch m;
	string line;
	cout<<"    enter as follow (pass2 sourcefile)"<<endl;
	getline(cin, line);
	if (regex_search(line, m, e))
		{
            pass2 pass(m[1]);
            if(pass.passOneDone)
            pass.excutePassTwo();
            else{
                cout << "inCompletely assembling" << endl;
            }
		}
	else
		cout<<" enter source file in proper format";

    return 0;
}
