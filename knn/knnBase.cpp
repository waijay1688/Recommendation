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
 * knnBase.cpp contains some common functions of svd model.
 */
#ifndef KNN_KNNBASE_CPP_
#define KNN_KNNBASE_CPP_
namespace knn{
    //use some global variables£¬s(similarity matrix) 
    double mi[ITEM_NUM+1] = {0.0};             //store the mean rating of each item
    int    buNum[USER_NUM+1] = {0};       //ratings num of every user ��??��u�䨰��?��?item������y��? 
	int    biNum[ITEM_NUM+1] = {0};            //the rating num of every item(´ò¹ýitem i·ÖµÄÓÃ»§×ÜÊý)
    float s[ITEM_NUM+1][ITEM_NUM+1] = {0};     //item-item similarity matrix(item-itemÏàËÆ¾ØÕó)
    float kmax[ITEM_NUM+1] = {0};              //k-max array ,the k-max similarity of each item
    //map<int,int> rateMatrix[ITEM_NUM+1];     //use a map array to store the sparse matrix of training-set rating 
    
	double bu[USER_NUM+1] = {0};       // the user bias in the baseline predictor
    double bi[ITEM_NUM+1] = {0};       // the item bias in the baseline predictor
	float mean = 0.0;
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);   //store training set
    vector<testSetNode> probeRow;                            //store test set
    
    void initialBais()
    {
        using namespace knn;
        int i,j;
        for(i = 1; i < USER_NUM+1; ++i){
            int vSize = rateMatrix[i].size();
            for(j=0; j < vSize; ++j) {
                bi[rateMatrix[i][j].item] += (rateMatrix[i][j].rate - mean);
                biNum[rateMatrix[i][j].item] += 1;
            }            
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
            if(biNum[i] >=1)bi[i] = bi[i]/(biNum[i]+25);
            else bi[i] = 0.0;
            
        }
       
        for(i = 1; i < USER_NUM+1; ++i){
            int vSize = rateMatrix[i].size();
            for(j=0; j < vSize; ++j) {
                bu[i] += (rateMatrix[i][j].rate - mean - bi[rateMatrix[i][j].item]);
                buNum[i] += 1;
            }            
        }
        for(i = 1; i < USER_NUM+1; ++i) {
            if(buNum[i]>=1)bu[i] = bu[i]/(buNum[i]+10);
            else bu[i] = 0.0;
        }
    }
    //load the similarity matrix from file
    void loadSimMatrix(float s[ITEM_NUM+1][ITEM_NUM+1], const char* simFile)
    {
        char rateStr[256];
        char* pch;    
        int itemNum = 0;
        std::ifstream from(simFile);
        if (!from.is_open()) {
            cout << "can't open  operation failed!\n";
            exit(1);
        }
        char* separator = "\t";
        int itemI = 0, itemJ = 0;
        float sim = 0.0;
        while(from.getline(rateStr,256)){
            string strTemp(rateStr);
            int pos = strTemp.find(":");
            if(-1 != pos) {
                itemI = atoi(strTemp.substr(0,pos).c_str());
                
                if(0 == itemI ) {
                    cout<<strTemp<<"#####################"<<endl;
                    exit(1);
                }
                ++itemNum;     
                if(itemNum %3000 ==0) {
                    cout<<"read item "<<itemNum<<" sucessfully!"<<endl;
                }
                continue;
            }
            if(strTemp.length() < 3)continue;
            int i = 0;
            pch = strtok (rateStr,separator);
            while (pch != NULL) {
                if(0 == i) itemJ = atoi(pch);
                else if(1 == i) sim = atof(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,separator);
            }
            if(0 == itemI || 0 == itemJ) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }        
            s[itemI][itemJ] = sim;
        }
        from.close();
        cout<<"end load training rate!"<<endl;
    }
    
    //load k-max similarity value from file
    void loadKMax(float kmaxLocal[ITEM_NUM+1], const char* fileName, const char* separator)
    {
        char rateStr[256];
        char* pch;   
        int fileNum = 0;
        std::ifstream from (fileName);
        if (!from.is_open()) {
            cout << "can't open input file!\n";
            exit(1);
        }
            
        int itemId = 0;
        float simTmp = 0.0;
        string strTemp = "";
        
        while(from.getline(rateStr,256)){
            string strTemp(rateStr);
            if(strTemp.length() < 3)continue;
            int i = 0;
            pch = strtok (rateStr,separator);
            while (pch != NULL) {
                if(0 == i) itemId = atoi(pch);
                else if(1 == i) simTmp = atof(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,separator);
            }
            if(0 == itemId) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }
            kmaxLocal[itemId] = simTmp;
        }
        from.close();
        cout<<"read k-max file sucessfully!"<<endl;
        return;
    }
    
    void getItemMean()
    {
        //Çómi£¬¼´Ã¿¸öitemµÄÆ½¾ù´ò·Ö
        using namespace knn;
        int i,j;
        for(i = 1; i < USER_NUM+1; ++i){
            int vSize = rateMatrix[i].size();
            for(j=0; j < vSize; ++j) {
                mi[rateMatrix[i][j].item] += rateMatrix[i][j].rate;
                biNum[rateMatrix[i][j].item] += 1;
            }            
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
            if(biNum[i] >=1)mi[i] = mi[i]/biNum[i];
            else mi[i] = mean;
        }
    }
	/*
    float predictRateBias(int user, int item, int dim)
	{
		
		int RuNum = rateMatrix[user].size(); //the num of items rated by user(ÓÃ»§user´ò¹ý·ÖµÄitemÊýÄ¿)
		double sumNumerator = 0.0;
		double sumDenominator = 0.0;
		
		for(int p=0; p< RuNum; ++p) {
			int j = rateMatrix[user][p].item;
			int rate = rateMatrix[user][p].rate;
			if( s[item][j] < kmax[item]) continue; //the K of the KNN is here(KNN µÄK¾ÍÌåÏÖÔÚÕâÀïÀ²)
			else {
				sumDenominator += fabs(s[item][j]);
				sumNumerator =+ s[item][j] * ( rate - mi[j]);
			}
		}
		float ret =0.0;
		if(sumDenominator > 0.02) ret =  (sumNumerator/sumDenominator);
	
		return ret;
	}
	*/
	float predictRateBias(int user, int item, int dim)
	{
		
		int RuNum = rateMatrix[user].size(); //the num of items rated by user(ÓÃ»§user´ò¹ý·ÖµÄitemÊýÄ¿)
		double sumNumerator = 0.0;
		double sumDenominator = 0.0;
		
		for(int p=0; p< RuNum; ++p) {
			int j = rateMatrix[user][p].item;
			int rate = rateMatrix[user][p].rate;
			if( s[item][j] < kmax[item]) continue; //the K of the KNN is here(KNN µÄK¾ÍÌåÏÖÔÚÕâÀïÀ²)
			else {
				sumNumerator+=(rate-mi[j])*s[item][j]; //rate-mi[j]-bu[p]-bi[i];
                //sumNumerator+=(rate-mean-bu[user]-bi[j])*s[item][j];
				sumDenominator+=s[item][j];
			}
		}
		float ret =0.0;
		//if(sumDenominator > 0.02) ret =  sumNumerator
	
		return sumNumerator/sumDenominator;
	}
	void writeCFBias()
	{
		cout<<"begin write cf Bias"<<endl;
		
		ofstream outputw("cfBias1");		    
		for(int i = 1; i < USER_NUM+1; ++i)
		{
			outputw <<i<<":"<<endl;
            for(int j=1; j < ITEM_NUM+1; ++j)
				outputw << j << "\t" << predictRateBias(i,j,K_NUM)<<endl;				                
        }
        outputw.close();
		cout<<"BIAS write successfully"<<endl;
	}
	/*
	void writeKMaxIndex()
	{
		cout<<"begin write cf Kmax index"<<endl;
		
		ofstream kmaxindex("kmaxindex");	    
		for(int i = 1; i < USER_NUM+1; ++i)
		{
			outputw <<i<<":"<<endl;
            int RuNum = rateMatrix[i].size(); //the num of items rated by user(ÓÃ»§user´ò¹ý·ÖµÄitemÊýÄ¿)
			double sumNumerator = 0.0;
			double sumDenominator = 0.0;
		
			for(int p=0; p< RuNum; ++p) {
				int j = rateMatrix[i][p].item;
				int rate = rateMatrix[i][p].rate;
				if( s[item][j] < kmax[item]) continue; //the K of the KNN is here(KNN µÄK¾ÍÌåÏÖÔÚÕâÀïÀ²)
			else {
				//sumNumerator+=(rate-mi[j])*s[item][j]; //rate-mi[j]-bu[p]-bi[i];
                sumNumerator+=(rate-mean-bu[user]-bi[j])*s[item][j];
				sumDenominator+=s[item][j];
			}
		}			                
        }
        outputw.close();
		cout<<"BIAS write successfully"<<endl;
	}
	*/
	
    void model(const char* simFile, const char* kmaxFile)
    {
        initialBais();
        cout << "begin initialization: " << endl;
        loadSimMatrix(s, simFile);               //load sim matrix
        loadKMax(kmax, kmaxFile,"\t");                //load k-max
        
        loadRating(TRAINING_SET,rateMatrix,RATE_SP);  //load training set
        loadProbe(PROBE_SET,probeRow,RATE_SP);   //load test set
        
        mean = setMeanRating(USER_NUM,rateMatrix); //calculate the mean
        getItemMean();         //calculate the mean of each item

        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe(probeRow,K_NUM);
        cout <<"end testRMSEProbe(): " << endl;
		writeCFBias();
        return;
    }
};

/**
 * predict the rate
 */
float predictRate(int user, int item, int dim)
{
    using namespace knn;
    
    int RuNum = rateMatrix[user].size(); //the num of items rated by user(ÓÃ»§user´ò¹ý·ÖµÄitemÊýÄ¿)
    double sumNumerator = 0.0;
    double sumDenominator = 0.0;
    
    for(int p=0; p< RuNum; ++p) {
        int j = rateMatrix[user][p].item;
        int rate = rateMatrix[user][p].rate;
        if( s[item][j] < kmax[item]) continue; //the K of the KNN is here(KNN µÄK¾ÍÌåÏÖÔÚÕâÀïÀ²)
        else {
            sumDenominator += fabs(s[item][j]);
            sumNumerator =+ s[item][j] * ( rate - mi[j]);
        }
    }
    float ret =0.0;
    if(sumDenominator > 0.02) ret =  mi[item] + (sumNumerator/sumDenominator)/sqrt(RuNum);
    else ret =  mi[item];
    if(ret < MIN_RATE) ret = MIN_RATE;
    if(ret > MAX_RATE) ret = MAX_RATE;
    return ret;
}
#endif // KNN_KNNBASE_CPP_ 
