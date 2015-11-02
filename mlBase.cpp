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
 * This file contains some common operations of movielens dataset, for example reading the test set
 * and reading the test set
 *
 * the ratings of  training set store in the array (USER X ITEM, user is the row ,and item is the column)
 */
#ifndef MLBASE_CPP_
#define MLBASE_CPP_

/**
 * load the training set of movielens dataset
 * 
 */
void loadRating(char * fileName, vector< vector<rateNode> >& rateMatrixLocal, const char * separator)
{
    char rateStr[256];
    char* pch;   
    int fileNum = 0;
    std::ifstream from (fileName);
    if (!from.is_open()) {
        cout << "can't open input file!\n";
        exit(1);
    }
        
    int itemId = 0, userId = 0, rate = 0,timestamp=0;
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
            else if(i == 3) timestamp=atoi(pch)/800000;
			else if(i>3) break;
            ++i;
            pch = strtok (NULL,separator);
          }
		/*
		if(0 == itemId || 0 == userId ||  0 == rate ) {
            cout<<strTemp<<"#####################userId:"<<userId<<" itemId:"<<itemId<<" rate:"<<rate<<endl;
            exit(1);
        }
		*/
        //³õÊ¼»¯rateMatrix
        try {
            rateNode tmpNode;
            tmpNode.item = itemId;
            tmpNode.rate = (short)rate;
			tmpNode.time=timestamp;
            rateMatrixLocal[userId].push_back(tmpNode);
        }
        catch (bad_alloc& ba){
            cerr << "bad_alloc caught: " << ba.what() << endl;
        }
    }
    from.close();
    cout<<"read training set file sucessfully!"<<endl;
    return;
}


//load test set
void loadProbe(char * fileName,vector<testSetNode>& probeSet, const char* separator)
{
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "can't open test set file!\n";
        exit(1);
    }
    char rateStr[256];
    char* pch ; // store a token of a string

    string strTemp;
    int rateValue,itemId,userId,probeNum=0,timestamp=0;
    
    while(in.getline(rateStr,256)){
        strTemp = rateStr;
        if(strTemp.length() < 4) continue;
        int i = 0;
        pch = strtok (rateStr,separator);
        while (pch != NULL) {
            if(0 == i) userId = atoi(pch);
            else if(1 == i) itemId = atoi(pch);
            else if(2 == i) rateValue = atoi(pch);
            else if(i ==3) timestamp=atoi(pch)/800000;
			else if(i>3) break;
            ++i;
            pch = strtok (NULL,separator);
        }
        try {
            testSetNode tmpNode;
            tmpNode.item = itemId;
            tmpNode.rate = (short)rateValue;
            tmpNode.user = userId;
			tmpNode.time=timestamp;
            probeSet.push_back(tmpNode);
            ++probeNum;
        }
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    cout << "Load " << probeNum << " test ratings successfully!"<<endl;
    in.close(); 
}

//load item information
void loadItemInformation(char * fileName,vector<itemInfoSetNode>& iteminfoSet, const char* separator)
{
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "can't open test set file!\n";
        exit(1);
    }
    char rateStr[356];
    char* pch ; // store a token of a string

    string strTemp;
    int itemId=0;
	int itemInforNum=0;
	short attri[19];
    
    while(in.getline(rateStr,356)){
        strTemp = rateStr;
        if(strTemp.length() < 22) continue;
        int i = 0;
        pch = strtok (rateStr,separator);
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) ;
            else if(2 == i) ;
            else if(3 == i) ;
			else if(i>3&&i<23)
				attri[i-4]=atoi(pch);
			else break;
            ++i;
            pch = strtok (NULL,separator);
        }
        try {
            itemInfoSetNode tmpNode;
            tmpNode.item = itemId;
            for(int i=0;i<19;i++)
				tmpNode.attribute[i]=attri[i];
            iteminfoSet.push_back(tmpNode);
            ++itemInforNum;
			
        }
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    cout << "Load " << itemInforNum << " item information successfully!"<<endl;
    in.close(); 
}
//load item content information 1m
void loadItemContentInfo(char * fileName,vector<itemContentInfoNode>& iteminfoSet, const char* separator)
{
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "can't open test set file!\n";
        exit(1);
    }
    char rateStr[256];
    char* pch ; // store a token of a string

    string strTemp;
    int itemId=0;
	char * str1,*str2;
	int itemInforNum=0;
	//set the first one empty
	itemContentInfoNode tmpNode;
    iteminfoSet.push_back(tmpNode);
    while(in.getline(rateStr,256)){
        strTemp = rateStr;
        if(strTemp.length() < 3) continue;
        int i = 0;
        pch = strtok (rateStr,":");
        while (pch != NULL) {
            if(0 == i) itemId = atoi(pch);
            else if(1 == i) str1=pch;
            else if(2 == i) str2=pch;
			else if(i>2) break;
            ++i;
            pch = strtok (NULL,":");
        }
        try 
		{
            itemContentInfoNode tmpNode;
            tmpNode.item = itemId;
			char stryear[5];
            for(int i=strlen(str1);i>0;i--)
				if(str1[i]==')')
				{
					i--;
					stryear[4]='\0';
					int n=3;
					while(n>=0&&str1[i]!='(')
					{
						stryear[n]=str1[i];
						i--;n--;
					}
					
					tmpNode.year=atoi(stryear);
					break;
				}
			
			for(i=0;i<19;i++)
				tmpNode.attribute[i]=false;
			char* pch = strtok (str2,"|");
			while (pch != NULL) 
			{
				if(int(strcmp(pch,"unknown"))==0)
				{
					tmpNode.attribute[0]=true;
				}
				else
				if(strcmp(pch,"Action")==0)
					tmpNode.attribute[1]=true;
				else
				if(strcmp(pch, "Adventure")==0) 
					tmpNode.attribute[2]=true;
				else
				if(strcmp(pch, "Animation")==0)
					tmpNode.attribute[3]=true;
				else
				if(strcmp(pch, "Children's" )==0)
					tmpNode.attribute[4]=true;
				else
				if(strcmp(pch, "Comedy")==0)
					tmpNode.attribute[5]=true;
				else
				if(strcmp(pch, "Crime" )==0)
					tmpNode.attribute[6]=true;
				else
				if(strcmp(pch, "Documentary" )==0)
					tmpNode.attribute[7]=true;
				else
				if(strcmp(pch, "Drama")==0 )
					tmpNode.attribute[8]=true;
				else
				if(strcmp(pch, "Fantasy")==0)
					tmpNode.attribute[9]=true;
				else
				if(strcmp(pch, "Film-Noir")==0)
					tmpNode.attribute[10]=true;
				else
				if(strcmp(pch, "Horror")==0 )
					tmpNode.attribute[11]=true;
				else
				if(strcmp(pch, "Musical")==0)
					tmpNode.attribute[12]=true;
				else
				if(strcmp(pch, "Mystery")==0) 
					tmpNode.attribute[13]=true;
				else
				if(strcmp(pch, "Romance" )==0)
					tmpNode.attribute[14]=true;
				else
				if(strcmp(pch, "Sci-Fi" )==0)
					tmpNode.attribute[15]=true;
				else
				if(strcmp(pch, "Thriller" )==0)
					tmpNode.attribute[16]=true;
				else
				if(strcmp(pch,  "War" )==0)
					tmpNode.attribute[17]=true;
				else
				if(strcmp(pch,  "Western" )==0)
					tmpNode.attribute[18]=true;
				
            pch = strtok (NULL,"|");
			}
				
			iteminfoSet.push_back(tmpNode);
			++itemInforNum;
			
		}
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    cout << "Load " << itemInforNum << " item information successfully!"<<endl;
    in.close(); 
}

//load user information 1m
void loadUserInfo(char * fileName,vector<userInfoNode>& userInfoSet, const char* separator)
{
    ifstream in(fileName);
    if (!in.is_open()) {
        cout << "can't open test set file!\n";
        exit(1);
    }
    char rateStr[256];
    char* pch ; // store a token of a string

    string strTemp;
    int userId=0;
	short age,occupation;
	char * str1;
	int userInforNum=0;
	//set the first one empty
	userInfoNode tmpNode;
    userInfoSet.push_back(tmpNode);
    while(in.getline(rateStr,256)){
        strTemp = rateStr;
        if(strTemp.length() < 5) continue;
        int i = 0;
        pch = strtok (rateStr,":");
        while (pch != NULL) {
            if(0 == i) userId = atoi(pch);
            else if(1 == i) str1=pch;
            else if(2 == i) age=atoi(pch);
			else if(3==i) occupation=atoi(pch);
            else if(i>3) break;
			++i;
            pch = strtok (NULL,":");
        }
        try 
		{
            userInfoNode tmpNode;
            tmpNode.user = userId;
			if(str1[0]=='F')
				tmpNode.gender=true;
			else
				tmpNode.gender=false;
			if(age==1)
				tmpNode.age=0;
			else if(age==18)
				tmpNode.age=1;
			else if(age==25)
				tmpNode.age=2;
			else if(age==35)
				tmpNode.age=3;
			else if(age==45)
				tmpNode.age=4;
			else if(age==50)
				tmpNode.age=5;
			else if(age==56)
				tmpNode.age=6;
			tmpNode.occupation=occupation;
							
			userInfoSet.push_back(tmpNode);
			++userInforNum;
			
		}
        catch (bad_alloc& ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            cout << "Can't allocate the momery!" << endl; exit(1);
        }
    }
    cout << "Load " << userInforNum << " user information successfully!"<<endl;
    in.close(); 
}
#endif // MLBASE_CPP_ 
