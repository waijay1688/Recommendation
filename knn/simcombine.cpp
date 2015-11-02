#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#define RATE_SP "	"  //rate Separator
#define USER_NUM 943 //10K:943 1M:6040
#define ITEM_NUM 1682 //10K:1682 1M:3900

using namespace std;
float cw1[ITEM_NUM+1][ITEM_NUM+1] = {0}; 
float cw2[ITEM_NUM+1][ITEM_NUM+1] = {0}; 
int main()
{
		
	{
	std::ifstream from("contSim");
        char contSim[256];
		char* pch;  
        if (!from.is_open()) {
            cout << "can't open  operation failed!\n";
            exit(1);
        }
        int itemI = 0, itemJ = 0;
        float sim = 0.0;
        while(from.getline(contSim,256))
		{
            string strTemp(contSim);
            int pos = strTemp.find(":");
            if(-1 != pos) 
			{
                itemJ = itemI; //store the last item
                itemI = atoi(strTemp.substr(0,pos).c_str());
                if(0 == itemJ) 
				{//the first line
                    continue;
                }
                if(0 == itemI ) {
                    cout<<strTemp<<"#####################"<<endl;
                    exit(1);
                }              
                continue;
            }
			if(strTemp.length() < 3)continue;
            int i = 0;
            pch = strtok (contSim,RATE_SP);
            while (pch != NULL) 
			{
                if(0 == i) itemJ = atoi(pch);
                else if(1 == i) cw1[itemI][itemJ] = atof(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,RATE_SP);
            }
            if(0 == itemI || 0 == itemJ) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }        
		}
		from.close();
	}

	{
	std::ifstream from("movielens1");
        char contSim[256];
		char* pch;  
        if (!from.is_open()) {
            cout << "can't open  operation failed!\n";
            exit(1);
        }
        int itemI = 0, itemJ = 0;
        float sim = 0.0;
        while(from.getline(contSim,256))
		{
            string strTemp(contSim);
            int pos = strTemp.find(":");
            if(-1 != pos) 
			{
                itemJ = itemI; //store the last item
                itemI = atoi(strTemp.substr(0,pos).c_str());
                if(0 == itemJ) 
				{//the first line
                    continue;
                }
                if(0 == itemI ) {
                    cout<<strTemp<<"#####################"<<endl;
                    exit(1);
                }              
                continue;
            }
			if(strTemp.length() < 3)continue;
            int i = 0;
            pch = strtok (contSim,RATE_SP);
            while (pch != NULL) 
			{
                if(0 == i) itemJ = atoi(pch);
                else if(1 == i) cw2[itemI][itemJ] = atof(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,RATE_SP);
            }
            if(0 == itemI || 0 == itemJ) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }        
		}
		from.close();
	}

		ofstream outputw("movielens");
        for(int i=1; i < ITEM_NUM+1; ++i)
            {
                outputw <<i<<":"<<endl;
                for(int j=1; j < ITEM_NUM+1; ++j) {
                    outputw << j << "\t" << pow(cw1[i][j],0.1)*pow(cw2[i][j],1.5)<<endl;
                }
            }
        outputw.close();
		return 0;
}