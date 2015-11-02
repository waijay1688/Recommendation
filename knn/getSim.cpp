/* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * If there are some bugs, please contact me via email honglianglv@gmail.com or submit the bugs 
 * in the google code project http://code.google.com/p/recsyscode/issues/list
 * 
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 * my google profile:https://profiles.google.com/honglianglv/about?hl=en
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the license GPLV3.
 *
 * this programme calculate the similarity between two items 
 *
 * steps:
 * £¨1£©pre-process: compute the mean rate of every item£¬and store in the mean array
 * £¨2£©for every i£¬j£¬calculate the similarity between i and j
 * £¨3£©for every i,j , find the users who rated both i and j and store in a set
 * £¨4£©use the formula £¨4.12£© in the page 125 fo Recommender system handbook to calculate the similarity of
 *  i and j and store in file
 *  note: the similarity matrix we get must be symmetrical,or there are something wrong.
 *    
 *  if the source is movielens, then get the similarity file by knn::getSimMatrix();
 *  if the source is netflix, then get the similarity file by knn::getSimMatrix("netflix");
 */
#include "../commonHeader.h"
#include "../mlBase.cpp"
#define TRAINING_SET "../dataset/movielens/u1.base"
#define TRAINING_ITEM_INFORMATION "../dataset/movielens/u.item"
#define RATE_SP "	"  //rate Separator
#define RATE_SP_INFO "|"
#define USER_NUM 943 //10K:943 1M:6040
#define ITEM_NUM 1682 //10K:1682 1M:3900

/*
  #define TRAINING_SET "../dataset/netflix/data_without_prob.txt"
  #define RATE_SP ","  //rate Separator
  #define USER_NUM 480189 //10K:943 1M:6040
  #define ITEM_NUM 17770 //10K:1682 1M:3900
*/
namespace knn{
    vector<float> mi(ITEM_NUM+1,0.0);         //store the mean rate of every item ÓÃÀ´´æ´¢Ã¿¸öitemµÄÆ½¾ù´ò·Ö
    float w[ITEM_NUM+1][ITEM_NUM+1] = {0};    //item-item similarity matrix (item-itemÏàËÆ¾ØÕó)
    float cw[ITEM_NUM+1][ITEM_NUM+1] = {0}; 
	map<int,short> rateMatrix[ITEM_NUM+1];    //use a map to store the sparse rate matrix(Ê¹ÓÃÒ»¸ömapÊý×é´æ´¢Ï¡ÊèµÄ´ò·Ö¾ØÕó)
    float mean = 0;                           //mean of all ratings(È«¾ÖµÄÆ½¾ùÖµ)
    //item information
	vector<itemInfoSetNode> itemInfo;
    //function declaration    
    double getSim(int item1,int item2);
    void loadRating(char * filePath, map<int,short> rateMatrixLocal[],const char* separator);
    void loadNetflixRating(char * filePath, map<int,short> rateMatrixLocal[],const char* separator);
	void getContentSimMatrix();

	//vector<itemInfoSetNode> itemInfo;
		

	void readContentSimaMatrix()
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
                else if(1 == i) cw[itemI][itemJ] = atof(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,RATE_SP);
            }
            if(0 == itemI || 0 == itemJ) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }        
		}
	}

	void getContentSimMatrix()
	{
		//item information
		
		cout<<"begin cout the content similarity"<<endl;
		for(int i = 1; i < ITEM_NUM+1; ++i)
		{
			for(int j=i;j < ITEM_NUM+1; ++j) 
			{
				if(i==j)
					cw[i][j]=1;
				else
				{
					int count1=0,count2=0;
					float countco=0;
					for(int k=0;k<19;k++)
					{
						if(itemInfo[i-1].attribute[k]&&itemInfo[j-1].attribute[k])
						{
							count1++;
							count2++;
							countco=countco+0.8;
						}
						else
						if(!itemInfo[i-1].attribute[k]&&itemInfo[j-1].attribute[k])
						{
							count2++;
							countco=countco+0.2;
						}
						else
						if(itemInfo[i-1].attribute[k]&&!itemInfo[j-1].attribute[k])
						{
							count1++;
							countco+=0.2;
						}
						else
						{
							countco+0.1;
						}
					}
					cw[i][j]=countco/(sqrt(count1)*sqrt(count2));
					cw[j][i]=cw[i][j];
				}
			}
		}
		
		ofstream outputw("contSim");
        for(i=1; i < ITEM_NUM+1; ++i)
            {
                outputw <<i<<":"<<endl;
                for(int j=1; j < ITEM_NUM+1; ++j) {
                    outputw << j << "\t" << cw[i][j]<<endl;
                }
            }
        outputw.close();
		
        cout<<"Cont Sim successfully exit!"<<endl;
	}

    void getSimMatrix(const char* source="movielens1")
    {
        cout << "begin initialization: " << endl;   
        if("movielens1" == source)loadRating(TRAINING_SET,rateMatrix,RATE_SP);  //initialization,load ratings to rateMatrix
        else if("netflix" == source)loadNetflixRating(TRAINING_SET,rateMatrix,RATE_SP);
        loadItemInformation(TRAINING_ITEM_INFORMATION,itemInfo, RATE_SP_INFO);

		//readContentSimaMatrix();
		int i,u,j,k;
        
        //get the mean rate of every item
        for(i = 1; i < ITEM_NUM+1; ++i){  
            map <int,short> ::iterator it = rateMatrix[i].begin(); 
            map <int,short> ::iterator end = rateMatrix[i].end();
            while(it != end) {
                mi[i] += (*it).second;
                ++it;
            }
            if(rateMatrix[i].size() > 0)mi[i] = mi[i]/rateMatrix[i].size();
            else mi[i] = 0;
        } 
        
        for(i = 1; i < ITEM_NUM+1; ++i){  //get similarity for every i and j
            for(j = i; j < ITEM_NUM+1; ++j) {
                w[i][j] = getSim(i,j);
                if(j != i)w[j][i] = w[i][j];
            }
            if( i % 100 == 0)cout <<i<< "  over!"<<endl;
        }
        
        //output the similarity matrix
        ofstream outputw(source);
        for(i=1; i < ITEM_NUM+1; ++i)
            {
                outputw <<i<<":"<<endl;
                for(j=1; j < ITEM_NUM+1; ++j) {
                    outputw << j << "\t" << w[i][j]<<endl;
                }
            }
        outputw.close();
        cout<<"successfully exit!"<<endl;
    }
    
    /**
     * load ratings in filePath to rateMatrix
     */
    void loadRating(char * filePath, map<int,short> rateMatrixLocal[],const char* separator)
    {
        cout<<"begin load training rate:"<<endl;
        std::ifstream from (filePath);
        if (!from.is_open()) {
            cout << "can't open  operation failed!\n";
            exit(1);
        }
        char rateStr[256];
        char* pch;
        int itemId = 0, userId = 0, rate = 0;
        string strTemp = "";
        
        while(from.getline(rateStr,256)){
            string strTemp(rateStr);
            if(strTemp.length() < 3)continue;
                
            int i = 0;
            pch = strtok (rateStr,separator);
            while (pch != NULL) {
                if(0 == i) userId = atoi(pch);
                else if(1 == i) itemId = atoi(pch);
                else if(2 == i) rate = atoi(pch);
                else if(i > 2) break;
                ++i;
                pch = strtok (NULL,separator);
            }
            if(0 == itemId || 0 == userId ||  0 == rate ) {
                cout<<strTemp<<"#####################userId:"<<userId<<" itemId:"<<itemId<<" rate:"<<rate<<endl;
                exit(1);
            }
            rateMatrixLocal[itemId][userId] = rate;
        }
        from.close();
        cout<<"end load training rate!"<<endl;
        return;
    }
	
	
    /**
     * load the ratings in filePath to rateMatrix
     * 
     */
    void loadNetflixRating(char * filePath, map<int,short> rateMatrixLocal[],const char* separator)
    {
        cout<<"begin load training rate:"<<endl;
        char rateStr[256];
        char* pch;    
        vector<string> rateDetail;
        int fileNum = 0;
        std::ifstream from(filePath);
        if (!from.is_open()) {
            cout << "can't open  operation failed!\n";
            exit(1);
        }
        
        int itemId = 0, userId = 0, rate = 0;
        while(from.getline(rateStr,256)){
            string strTemp(rateStr);
            int pos = strTemp.find(":");
            if(-1 != pos) {
                itemId = atoi(strTemp.substr(0,pos).c_str());
                if(0 == itemId ) {
                    cout<<strTemp<<"#####################"<<pos<<"####"<<strTemp.substr(0,pos).c_str()<<endl;
                    exit(1);
                }        
                ++fileNum;     
                if(fileNum %3000 ==0) {
                    cout<<"read file "<<fileNum<<" sucessfully! size:"<<sizeof(rateMatrixLocal)<<endl;
                }
                continue;
            }
            if(strTemp.length() < 3)continue;
            int i = 0;
            pch = strtok (rateStr,separator);
            while (pch != NULL) {
                if(0 == i) userId = atoi(pch);
                else if(1 == i) rate = atoi(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,separator);
            }
            if(0 == itemId || 0 == userId ||  0 == rate) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }        
            rateMatrixLocal[itemId][userId] = rate;
        }
        from.close();
        cout<<"end load training rate!"<<endl;
        return;
    }
    
    double getSim(int item1, int item2) {
        //(1)find the users who rated both item1 and item2(ÕÒµ½´ò·ÖµÄ¹«¹²ÓÃ»§¼¯ºÏ) 
        //(2)calculate the similarity according the pearson formula(°´ÕÕ¹«Ê½¼ÆËã)
        if(item1 == item2)return 1.0;
		int itemNum1,itemNum2,itemNumCo=0;
        if((itemNum1=rateMatrix[item1].size())==0 || (itemNum2=rateMatrix[item2].size())==0 ) return 0.0;

        map <int,short> ::iterator it = rateMatrix[item1].begin(); 
        map <int,short> ::iterator end = rateMatrix[item1].end(); 
        double sum1 = 0.0, sumSquare1 = 0.0, sumSquare2 = 0.0;
        for(; it != end; ++it) {
            int user = (*it).first;
            if(rateMatrix[item2].find(user) != rateMatrix[item2].end()) {
                //cout<<"common user:"<<user<<'\t'<<rateMatrix[item1][user]<<'\t'<<rateMatrix[item2][user]<<endl;
                //already find the common user(ÒÑ¾­ÕÒµ½ÁË¹«¹²ÔªËØ)
                sum1 += (rateMatrix[item1][user] - mi[item1]) *(rateMatrix[item2][user] - mi[item2]);
                sumSquare1 += pow(rateMatrix[item1][user] - mi[item1],2); 
                sumSquare2 += pow(rateMatrix[item2][user] - mi[item2],2);
				itemNumCo++;
            }
        }
        double sim = 0.0;
		/*
		for(int k=0;k<19;k++)
		{
			sumSquare1+=itemInfo[item1-1].attribute[k]*0.25;
			sumSquare2+=itemInfo[item2-1].attribute[k]*0.25;
			sum1+=itemInfo[item1-1].attribute[k]*itemInfo[item2-1].attribute[k]*0.5;
		}
        if(sumSquare1 > 0.0000000001  && sumSquare2 > 0.0000000001 &&itemNumCo!=0)
			sim = sum1 / sqrt( sumSquare1 * sumSquare2)*itemNumCo/(sqrt(itemNum1)*sqrt(itemNum2));
        return sim;
		*/
        if(sumSquare1 > 0.0000000001  && sumSquare2 > 0.0000000001 &&itemNumCo!=0)
			sim = sum1 / sqrt( sumSquare1 * sumSquare2)*itemNumCo/(sqrt(itemNum1)*sqrt(itemNum2));
        return sim;
		
    }
};

int main(int argc, char ** argv)
{
    time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration; 
    start = time(NULL);
    timeStartInfo = localtime(&start);
    string timeStartStr = asctime(timeStartInfo);
    knn::getSimMatrix();
	//knn::getContentSimMatrix();
    end = time(NULL);
    duration = (end-start);
    timeEndInfo = localtime(&end);
    cout << "Start at "<<timeStartStr<<", and end at "<< asctime(timeEndInfo);
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
