/* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * If there are some bugs, please contact me via email honglianglv@gmail.com or submit the bugs 
 * in the google code project http://code.google.com/p/recsyscode/issues/list
 * 
 * my blog: http://lifecrunuh.biz
 * my twitter: http://twitter.com/honglianglv
 * my google profile:https://profiles.google.com/honglianglv/about?hl=en
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the license GPLV3.
 *
 * svdBase.cpp contains some common functions of svd model.
 */
#ifndef SVD_SVDBASE_CPP_
#define SVD_SVDBASE_CPP_
namespace svd{
      //use some global variables£¬store the parameter bu, bi, p, q
    double bu[USER_NUM+1] = {0};       // the user bias in the baseline predictor
    double bi[ITEM_NUM+1] = {0};       // the item bias in the baseline predictor
    double bt[TIME_NUM+1] = {0};

    int buNum[USER_NUM+1] = {0};       //ratings num of every user ÓÃ»§u´ò·ÖµÄitem×ÜÊý£¬ 
    int biNum[ITEM_NUM+1] = {0};       //rating num of every item  ´ò¹ýitem i·ÖµÄÓÃ»§×ÜÊý 
    
    double p[USER_NUM+1][K_NUM+1] = {0};   //user uharacter Matrix ÓÃÓÚ´æ´¢ÓÃ»§µÄÊôÐÔÃèÊöp   
    double q[ITEM_NUM+1][K_NUM+1] = {0};   //item uharacter Matrix ÓÃÓÚitemµÄÊôÐÔÃèÊöq       
    double x[ITEM_NUM+1][K_NUM+1] = {0};
	double y[ITEM_NUM+1][K_NUM+1] = {0};

	double tx[USER_NUM+1][K_NUM+1] = {0};
	double ts[ITEM_NUM+1][K_NUM+1] = {0};
	double tz[TIME_NUM+1][K_NUM+1] = {0};
	double ty[TIME_NUM+1][K_NUM+1] = {0};
	
	double ca[ATTRIBUTE_NUM]={0};
	double ba[AGE_NUM]={0};           //user age
	double bo[OCCUPATION_NUM]={0};
	double by[YEAR_NUM]={0};

	double ax[ITEM_NUM+1][K_NUM+1] = {0};
	double ay[AGE_NUM][K_NUM+1]    = {0};
	
	double ox[ITEM_NUM+1][K_NUM+1] = {0};
	double oy[OCCUPATION_NUM][K_NUM+1]= {0};

	double yx[USER_NUM+1][K_NUM+1] = {0};
	double yy[YEAR_NUM][K_NUM+1]   = {0};

	double gx[ITEM_NUM+1][K_NUM+1] = {0};
	double gy[GENDER_NUM][K_NUM+1] = {0};
	float cfBias[USER_NUM+1][ITEM_NUM+1]={0};
//	double ug[USER_NUM+1][K_NUM+1] = {0};
//	double ul[ITEM_NUM+1][K_NUM+1] = {0};
//    double uh[TIME_NUM+1][K_NUM+1] = {0};
	float mean = 0;                        //mean of all ratings   È«¾ÖµÄÆ½¾ùÖµ             
	

	int tu[USER_NUM+1]={0};//the earliest time that the user has rated something   the bigest one
	int ti[ITEM_NUM+1]={0};
	int Early=0,Present=0;
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);   //store training set
    vector<testSetNode> probeRow;                            //store test set
	vector<testSetNode> All;                            //store all set
    vector<itemContentInfoNode> itemInfo;                      //store content set
	vector<userInfoNode> userInfo;    
	float predictRateBias(int user, int item,int index, int dim);
	

    //read cfbias
	void loadCFBias(float cfBias[USER_NUM+1][ITEM_NUM+1], const char* simFile)
	{
        char rateStr[256];
        char* pch;    
        int userNum = 0;
        std::ifstream from(simFile);
        if (!from.is_open()) {
            cout << "can't open  operation failed!\n";
            exit(1);
        }
        char* separator = "\t";
        int userI = 0, itemJ = 0;
        float CFBias = 0.0;
        while(from.getline(rateStr,256)){
            string strTemp(rateStr);
            int pos = strTemp.find(":");
            if(-1 != pos) {
                userI = atoi(strTemp.substr(0,pos).c_str());
                
                if(0 == userI ) {
                    cout<<strTemp<<"#####################"<<endl;
                    exit(1);
                }
                ++userNum;     
                if(userNum %3000 ==0) {
                    cout<<"read user "<<userNum<<" sucessfully!"<<endl;
                }
                continue;
            }
            if(strTemp.length() < 3)continue;
            int i = 0;
            pch = strtok (rateStr,separator);
            while (pch != NULL) {
                if(0 == i) itemJ = atoi(pch);
                else if(1 == i) CFBias = atof(pch);
                else if(i > 1) break;
                ++i;
                pch = strtok (NULL,separator);
            }
            if(0 == userI || 0 == itemJ) {
                cout<<strTemp<<"#####################"<<endl;
                exit(1);
            }        
            cfBias[userI][itemJ] = CFBias;
        }
        from.close();
        cout<<"end load cf Bias"<<endl;
    }
    //initialize the bias bu and bi, the method in the page 2 of koren's TKDD'09 paper
    void initialBais()
    {
        using namespace svd;
        int i,j;
        for(i = 1; i < USER_NUM+1; ++i){
            int vSize = rateMatrix[i].size();
            for(j=0; j < vSize; ++j) {
                //bi[rateMatrix[i][j].item] += (rateMatrix[i][j].rate - mean);
				bi[rateMatrix[i][j].item] += cfBias[i][rateMatrix[i][j].item];
				bu[i]+=cfBias[i][rateMatrix[i][j].item];
                biNum[rateMatrix[i][j].item] += 1;
				buNum[i] += 1;
            }            
        }
        
        for(i = 1; i < ITEM_NUM+1; ++i) {
            if(biNum[i] >=1)bi[i] = bi[i]/(biNum[i]+25);
            else bi[i] = 0.0;
            
        }
       /*
        for(i = 1; i < USER_NUM+1; ++i){
            int vSize = rateMatrix[i].size();
            for(j=0; j < vSize; ++j) {
                bu[i] += (rateMatrix[i][j].rate - mean - bi[rateMatrix[i][j].item]);
                buNum[i] += 1;
            }            
        }
		*/
        for(i = 1; i < USER_NUM+1; ++i) {
            if(buNum[i]>=1)bu[i] = bu[i]/(buNum[i]+10);
            else bu[i] = 0.0;
        }
    }
    
    //intialize the matrix of user uharacter(P) and the matrix of item uharacter(Q)
    void initialPQ(int itemNum, int userNum,int dim)
    {
        using namespace svd;
        //int i;
        //@TODO should do some optimization to the initialization
        //is the ramdom function a best way to initialize the p and q?
        for(int i = 1; i < itemNum+1; ++i){
            setRand(q[i],dim,0);   
        }
        for(i = 1; i < userNum+1; ++i){
            setRand(p[i],dim,0);   
        }
        //for(int i = 1; i < userNum+1; ++i){
//		for(i = 1; i < userNum+1; ++i){
//            setRand(p[i],dim,0);   
//        }
		
		for(i = 1; i < itemNum+1; ++i){
            setRand(x[i],dim,0);   
        }
		for(i = 1; i < itemNum+1; ++i){
            setRand(y[i],dim,0);   
        }
/*
		for(i = 1; i < userNum+1; ++i){
            setRand(tx[i],dim,0);
			//setRand(ug[i],dim,0);
		}
		for(i = 1; i < itemNum+1; ++i){
            setRand(ts[i],dim,0); 
			//setRand(ul[i],dim,0);
		}
		for(i = 0; i < TIME_NUM+1; ++i){
            setRand(tz[i],dim,0);   
			setRand(ty[i],dim,0); 
			//setRand(uh[i],dim,0);
		}
*/
	
		for(i = 1; i < TIME_NUM+1; ++i){
            setRand(tz[i],dim,0);   
        }
        for(i = 1; i < userNum+1; ++i){
            setRand(tx[i],dim,0);   
        }
		for(i = 1; i < TIME_NUM+1; ++i){
            setRand(ty[i],dim,0);   
        }
        for(i = 1; i < itemNum+1; ++i){
            setRand(ts[i],dim,0);   
        }

		for(i = 1; i < itemNum+1; ++i){
            setRand(ax[i],dim,0);   
        }
        for(i = 1; i < AGE_NUM+1; ++i){
            setRand(ay[i],dim,0);   
        }

		for(i = 1; i < itemNum+1; ++i){
            setRand(ox[i],dim,0);   
        }
        for(i = 1; i < OCCUPATION_NUM+1; ++i){
            setRand(oy[i],dim,0);   
        }

		for(i = 1; i < userNum+1; ++i){
            setRand(yx[i],dim,0);   
        }
        for(i = 1; i <YEAR_NUM+1; ++i){
            setRand(yy[i],dim,0);   
        }

		for(i = 1; i < itemNum+1; ++i){
            setRand(gx[i],dim,0);   
        }
        for(i = 1; i <GENDER_NUM+1; ++i){
            setRand(gy[i],dim,0);   
        }
		
    }
    void getP(int user, int item,int dim)
	{
		
			int RuNum = rateMatrix[user].size(); //the num of items rated by user(ÓÃ»§u´ò¹ý·ÖµÄitemÊýÄ¿)
			float sqrtRuNum = 0.0;
			if(RuNum>1) 
				sqrtRuNum = (1.0/sqrt(RuNum));
			for(int k=1;k<dim+1;k++)
				p[user][k]=0;
			for(int i=0; i < RuNum; ++i) 
			{
				int itemI = rateMatrix[user][i].item;
				short rui = rateMatrix[user][i].rate; //real rate
				int tui=rateMatrix[user][i].time;
				int tgapu=tu[user]-tui;
				int tgapi=ti[itemI]-tui;
                //double bui = mean + bu[user] + bi[itemI]+bt[tui-Present]+
				//	dot(tx[user],tz[tgapu],dim)+ dot(ty[tgapi],ts[item],dim);
				double bui = mean + bu[user] + bi[itemI];
				for(int k=1;k<dim+1;k++)
					p[user][k]+=x[itemI][k]*(rui-bui);
					//p[user][k]+=x[itemI][k]*(rui-bui);// //cfBias[user][itemI]
			}

			for(k=1;k<dim+1;k++)
				p[user][k]*=sqrtRuNum;
			
			double tempP[K_NUM+1]={0};
			for(i=1;i<ITEM_NUM+1;i++)
				for(int kk=1;kk<dim+1;kk++)
					tempP[kk]+=y[i][kk];
			for(k=1;k<dim+1;k++)
				p[user][k]+=(tempP[k]/sqrt(ITEM_NUM));
		
	}
/*
	void getTheEarlyTime()
	{
		int eariest=0,presentest=10000000;
		for(int i=1;i<USER_NUM+1;i++)
		{
			int RuNum = rateMatrix[i].size(); //the num of items rated by user(ÓÃ»§user´ò¹ý·ÖµÄitemÊýÄ¿)		
			for(int p=0; p< RuNum; ++p) 
			{
				if(tu[i]<rateMatrix[i][p].time)
					tu[i]=rateMatrix[i][p].time;
				if(ti[rateMatrix[i][p].item]<rateMatrix[i][p].time)
					ti[rateMatrix[i][p].item]=rateMatrix[i][p].time;
				if(eariest<rateMatrix[i][p].time)
					eariest=rateMatrix[i][p].time;
				if(presentest>rateMatrix[i][p].time)
					presentest=rateMatrix[i][p].time;
			}			
		}
		Early=eariest;
		Present=presentest;
	}
*/
	void getTheEarlyTime()
	{
		int eariest=0,presentest=10000000;
		int allsize = All.size(); //the num of items rated by user(ÓÃ»§user´ò¹ý·ÖµÄitemÊýÄ¿)		
		for(int p=0; p< allsize; ++p) 
		{
			if(tu[All[p].user]<All[p].time)
				tu[All[p].user]=All[p].time;
			if(ti[All[p].item]<All[p].time)
				ti[All[p].item]=All[p].time;
			if(eariest<All[p].time)
				eariest=All[p].time;
			if(presentest>All[p].time)
				presentest=All[p].time;			
		}
		Early=eariest;
		Present=presentest;
	}

	void analysizeItemInfo()
	{
		int size=itemInfo.size();
		int early=3000,late=0;
		for(int i=1;i<size;i++)
		{
			if(early>itemInfo[i].year)
				early=itemInfo[i].year;
			if(late<itemInfo[i].year)
				late=itemInfo[i].year;
		}
		cout<<"early\t"<<early<<"\t"<<"late\t"<<late<<endl;
	}

	void analysizeItemInfo(vector<testSetNode> All)
	{
		std::ofstream itemyear("itemyear.txt");
		int size=All.size();
		double average[8]={0};
		int count[8]={0};
		double average1[10]={0};
		int count1[10]={0};
		for(int i=0;i<size;i++)
		{
			int year=itemInfo[All[i].item].year;
			if(year<=2000&&year>1990)
			{
				average[0]+=All[i].rate;
				count[0]+=1;
				if(year==2000)
				{
					average1[0]+=All[i].rate;
					count1[0]+=1;
				}
				else
				if(year==1999)
				{
					average1[1]+=All[i].rate;
					count1[1]+=1;
				}
				else
				if(year==1998)
				{
					average1[2]+=All[i].rate;
					count1[2]+=1;
				}
				else
				if(year==1997)
				{
					average1[3]+=All[i].rate;
					count1[3]+=1;
				}
				else
				if(year==1996)
				{
					average1[4]+=All[i].rate;
					count1[4]+=1;
				}
				else
				if(year==1995)
				{
					average1[5]+=All[i].rate;
					count1[5]+=1;
				}
				else
				if(year==1994)
				{
					average1[6]+=All[i].rate;
					count1[6]+=1;
				}
				else
				if(year==1993)
				{
					average1[7]+=All[i].rate;
					count1[7]+=1;
				}
				else
				if(year==1992)
				{
					average1[8]+=All[i].rate;
					count1[8]+=1;
				}
				else
				if(year==1991)
				{
					average1[9]+=All[i].rate;
					count1[9]+=1;
				}
			}
			else
			if(year<=1990&&year>1980)
			{
				average[1]+=All[i].rate;
				count[1]+=1;
			}
			else
			if(year<=1980&&year>1970)
			{
				average[2]+=All[i].rate;
				count[2]+=1;
			}
			else
			if(year<=1970&&year>1960)
			{
				average[3]+=All[i].rate;
				count[3]+=1;
			}
			else
			if(year>1950)
			{
				average[4]+=All[i].rate;
				count[4]+=1;
			}
			else
			if(year>1940)
			{
				average[5]+=All[i].rate;
				count[5]+=1;
			}
			else
			if(year>1930)
			{
				average[6]+=All[i].rate;
				count[6]+=1;
			}
			else
			if(year>1919)
			{
				average[7]+=All[i].rate;
				count[7]+=1;
			}
		}
		for(i=0;i<8;i++)
			cout<<average[i]<<"\t"<<count[i]<<"\t"<<average[i]/count[i]<<"\t";
		cout<<endl;
		for(i=0;i<10;i++)
			cout<<average1[i]<<"\t"<<count1[i]<<"\t"<<average1[i]/count1[i]<<"\t";
		cout<<endl;

		for(i=0;i<8;i++)
			itemyear<<average[i]/count[i]<<"\t";
		itemyear<<endl;
		for(i=0;i<10;i++)
			itemyear<<average1[i]/count1[i]<<"\t";
		itemyear<<endl;
		itemyear.close();
	}

	void analysizeUserInfo(vector<testSetNode> All)
	{
		std::ofstream userinfo("user.txt");
		int size=All.size();
		long double malerate=0,femalerate=0;
		int malecount=0,femalecount=0;
		double agerate[7]={0};
		int agecount[7]={0};
		double occupationrate[21]={0};
		int occupationcount[21]={0};
		for(int i=0;i<size;i++)
		{
			int user=All[i].user;
			if(userInfo[user].gender)
			{
				malerate+=All[i].rate;
				malecount++;
			}
			else
			{
				femalerate+=All[i].rate;
				femalecount++;
			}
			int age=userInfo[user].age;
			agerate[age]+=All[i].rate;
			agecount[age]++;
			int occupation=userInfo[user].occupation;
			occupationrate[occupation]+=All[i].rate;
			occupationcount[occupation]++;
		}
		cout<<malecount<<"\t"<<malerate/malecount<<"\t"<<femalecount<<"\t"<<femalerate/femalecount<<endl;
		for(i=0;i<7;i++)
		{
			cout<<agecount[i]<<"\t"<<agerate[i]/agecount[i]<<"\t";
			ba[i]=agerate[i]/agecount[i]-mean;
		}
		cout<<endl;
		for(i=0;i<21;i++)
			cout<<occupationcount[i]<<"\t"<<occupationrate[i]/occupationcount[i]<<"\t";
		cout<<endl;

		userinfo<<malerate/malecount<<"\t"<<femalerate/femalecount<<endl;
		for(i=0;i<7;i++)
			userinfo<<agerate[i]/agecount[i]<<"\t";
		userinfo<<endl;
		for(i=0;i<21;i++)
			userinfo<<occupationrate[i]/occupationcount[i]<<"\t";
		userinfo<<endl;
		userinfo.close();
	}


	void analysize(vector<testSetNode> All)
	{
		std::ofstream analysize("analysize.txt");
		int size=All.size();
		int userID[]={2,10,19,25,33,44,56,62,71,78,83,92,133,172,191,213,235,291,339,591,782,1013,1137,1522,1671};
		int itemID[]={1,11 ,18 ,23, 33, 41 ,49,55,65,76 , 71, 87, 89 ,104,147,197,238,320,442,672,754,1138,1459,2919,3781};
		double yearrate[25][8]={0};
		int yearcount[25][8]={0};
		double genderrate[25][2]={0};
		int gendercount[25][2]={0};
		double agerate[25][7]={0};
		int agecount[25][7]={0};
		double occupationrate[25][21]={0};
		int occupationcount[25][21]={0};
		int j=0;
		for(int i=0;i<size;i++)
		{
			int user=All[i].user;
			if(j<5&&user<userID[j])
				continue;
			if(user>userID[j]&&j<25)
			{
				j++;
				continue;
			}
			if(user>userID[j]&&j==25)
				break;
			if(userID[j]==user)
			{
				int item=All[i].item;
				int year=itemInfo[item].year;
				if(year<1961)
					continue;
				yearcount[j][((year-1)%100)/5-12]++;
				yearrate[j][((year-1)%100)/5-12]+=All[i].rate;			
			}			
		}
		cout<<"user -  year"<<endl;
		for(j=0;j<25;j++)
		{
			for(int i=0;i<8;i++)
			{
				cout<<yearrate[j][i]/yearcount[j][i]<<"\t";				
			}
			cout<<endl;
		}
		cout<<endl;
		analysize<<"user   year"<<endl;
		for(j=0;j<25;j++)
		{
			analysize<<userID[j]<<endl;
			for(int i=0;i<8;i++)
			{
				analysize<<yearrate[j][i]/yearcount[j][i]<<"\t";				
			}
			analysize<<endl;
		}
		analysize<<" "<<endl<<endl;

		for(i=0;i<size;i++)
		{
			int user=All[i].user;
			int item=All[i].item;
			for(j=0;j<25;j++)
				if(itemID[j]==item)
				{
					int age=userInfo[user].age;
					int gender;
					if(userInfo[user].gender)
						gender=1;
					else
						gender=0;
					int occupation=userInfo[user].occupation;
					int rate=All[i].rate;
					gendercount[j][gender]++;
					genderrate[j][gender]+=rate;
					agecount[j][age]++;
					agerate[j][age]+=rate;
					occupationcount[j][occupation]++;
					occupationrate[j][occupation]+=rate;
					break;
				}
		}
		cout<<"gender  item"<<endl;
		for(j=0;j<25;j++)
		{
			for(int i=0;i<2;i++)
			{
				cout<<genderrate[j][i]/gendercount[j][i]<<"\t";				
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<"age  item"<<endl;
		for(j=0;j<25;j++)
		{
			for(int i=0;i<7;i++)
			{
				cout<<agerate[j][i]/agecount[j][i]<<"\t";				
			}
			cout<<endl;
		}
		cout<<endl;
		cout<<"occupation  item"<<endl;
		for(j=0;j<25;j++)
		{
			for(int i=0;i<21;i++)
			{
				cout<<occupationrate[j][i]/occupationcount[j][i]<<"\t";				
			}
			cout<<endl;
		}
		cout<<endl;



		analysize<<"gender  item"<<endl;
		for(j=0;j<25;j++)
		{
			analysize<<itemID[j]<<endl;
			for(int i=0;i<2;i++)
			{
				analysize<<genderrate[j][i]/gendercount[j][i]<<"\t";				
			}
			analysize<<endl;
		}
		analysize<<endl;
		analysize<<"age  item"<<endl;
		for(j=0;j<25;j++)
		{
			analysize<<itemID[j]<<endl;
			for(int i=0;i<7;i++)
			{
				analysize<<agerate[j][i]/agecount[j][i]<<"\t";				
			}
			analysize<<endl;
		}
		analysize<<endl;
		analysize<<"occupation  item"<<endl;
		for(j=0;j<25;j++)
		{
			analysize<<itemID[j]<<endl;
			for(int i=0;i<21;i++)
			{
				analysize<<occupationrate[j][i]/occupationcount[j][i]<<"\t";				
			}
			analysize<<endl;
		}
		analysize<<endl;
		
	}

//predictRateTime
	float predictRateTime(int user, int item,int time,int timegapu,int timegapi,int dim)
{
	using namespace svd;
    int RuNum = rateMatrix[user].size(); //the num of items rated by user
    double ret; 
    
/*
	double temp=0;
	for(int kk=1;kk<dim+1;kk++)
	{
		temp+=ug[user][kk]*ul[item][kk]*uh[timegapu][kk];
	}
*/
	int age=userInfo[user].age;
	int occupation=userInfo[user].occupation;
	int year=itemInfo[item].year-1919;
	int gender=0;
	if(userInfo[user].gender)
		gender=1;
/*
	double pu[K_NUM+1]={0};
	double qi[K_NUM+1]={0};
	for(int i=1;i<K_NUM+1;i++)
	{
		pu[i]=p[user]+
*/
    if(RuNum > 1&&timegapu>=0&&timegapi>=0) {
        //ret = mean + bu[user] + bi[item] +  dot(p[user],q[item],dim)+svd::cfBias[user][item];
	
		ret = mean + bu[user] + bi[item] +//ba[age]+//bo[occupation]+bt[time-Present]+by[year]+
			dot(p[user],q[item],dim)+dot(tx[user],tz[timegapu],dim)+ dot(ty[timegapi],ts[item],dim) 
			+dot(ax[item],ay[userInfo[user].age],dim)+dot(ox[item],oy[occupation],dim)
				+dot(yx[user],yy[year],dim)+dot(gx[item],gy[gender],dim);
			//+dot(tx[user],tz[timegapu],dim)+ dot(ty[timegapi],ts[item],dim) ; bt[time-Present]+
	
    }
    //else ret  = mean+bu[user] + bi[item]+svd::cfBias[user][item];
	else 
		ret  = mean+bu[user] + bi[item]+ba[age]+bo[occupation]+dot(ax[user],ay[userInfo[user].age],dim)+dot(ox[user],oy[occupation],dim);//+bt[time-Present];
				//bt[time-Present]+dot(tx[user],tz[timegapu],dim)+ dot(ts[item],ty[timegapi],dim);
    if(ret < 1.0) ret = 1;
    if(ret > 5.0) ret = 5;
    return ret;
}

/*
**TIME AND CONTENT ATTRIBUTE
*/
	float predictRateTimeContent(int user, int item,int time,int timegapu,int timegapi,bool attribute[19],int dim)
{
	using namespace svd;
    int RuNum = rateMatrix[user].size(); //the num of items rated by user
    double ret; 
    if(timegapi<0)
		timegapi=0;
	if(timegapu<0)
		timegapu=0;
	
	double temp=0;
	int count=0;
	for(int kk=1;kk<dim+1;kk++)
	{
		if(attribute[kk])
		{
			temp+=ca[kk];
			count++;
		}
	}
	if(count!=0)
		temp/=(count);
	
    if(RuNum > 1&&timegapu<TIME_NUM+1&&timegapi<TIME_NUM+1) {
        //ret = mean + bu[user] + bi[item] +  dot(p[user],q[item],dim)+svd::cfBias[user][item];
		ret = mean + bu[user] + bi[item] + bt[time-Present]+ ba[userInfo[user].age]
			+dot(p[user],q[item],dim)+dot(tx[user],tz[timegapu],dim)+ dot(ty[timegapi],ts[item],dim) + temp;
			//+dot(tx[user],tz[timegapu],dim)+ dot(ty[timegapi],ts[item],dim) ;
    }
    //else ret  = mean+bu[user] + bi[item]+svd::cfBias[user][item];
	else ret  = mean+bu[user] + bi[item]+ba[userInfo[user].age]+
				bt[time-Present]+dot(tx[user],tz[timegapu],dim)+ dot(ts[item],ty[timegapi],dim);
    if(ret < 1.0) ret = 1;
    if(ret > 5.0) ret = 5;
    return ret;
}
/*
**RMSE TIME
*/
	double RMSEProbeTime(vector<testSetNode>& probeSet,int dim)
	{
		int probeSize = probeSet.size();
		float pRate,err;
		long double rmse = 0;
		
		for(int i = 0; i < probeSize; ++i) {
			
			int tui=probeSet[i].time;
			int tgapu=tu[probeSet[i].user]-tui;
			int tgapi=ti[probeSet[i].item]-tui;
			pRate = predictRateTime(probeSet[i].user,probeSet[i].item,tui,tgapu,tgapi,dim);      //predict rate
			//cout<<pRate<<"    "<<probeSet[i].rate<<"    "<<probeSet[i].user<<"    "<<probeSet[i].item<<endl;
			err = pRate-probeSet[i].rate;
			rmse += err*err;
		}
		rmse = sqrt( rmse / probeSize);
		cout<<"RMSE:"<<rmse<<" probeNum:"<<probeSize<<endl;
		return rmse;
	}
/*
**RMES TIME CONTENT ATTRIBUTE
*/
	double RMSEProbeTimeContent(vector<testSetNode>& probeSet,int dim)
	{
		int probeSize = probeSet.size();
		float pRate,err;
		long double rmse = 0;
		
		for(int i = 0; i < probeSize; ++i) {
			
			int tui=probeSet[i].time;
			int tgapu=tu[probeSet[i].user]-tui;
			int tgapi=ti[probeSet[i].item]-tui;
			pRate = predictRateTimeContent(probeSet[i].user,probeSet[i].item,tui,tgapu,tgapi,
				itemInfo[probeSet[i].item].attribute,dim);      //predict rate
			//cout<<pRate<<"    "<<probeSet[i].rate<<"    "<<probeSet[i].user<<"    "<<probeSet[i].item<<endl;
			err = pRate-probeSet[i].rate;
			rmse += err*err;
		}
		rmse = sqrt( rmse / probeSize);
		cout<<"RMSE:"<<rmse<<" probeNum:"<<probeSize<<endl;
		return rmse;
	}

	double RMSEProbeAll(vector<testSetNode>& probeSet,int dim)
	{
		int probeSize = probeSet.size();
		float pRate,err;
		long double rmse = 0;
		
		for(int i = 0; i < probeSize; ++i) {
			
			int tui=probeSet[i].time;
			int tgapu=tu[probeSet[i].user]-tui;
			int tgapi=ti[probeSet[i].item]-tui;
			pRate = predictRateTime(probeSet[i].user,probeSet[i].item,tui,tgapu,tgapi,dim);      //predict rate
			//cout<<pRate<<"    "<<probeSet[i].rate<<"    "<<probeSet[i].user<<"    "<<probeSet[i].item<<endl;
			err = pRate-probeSet[i].rate;
			rmse += err*err;
		}
		double beta1=0.025;
		{
			for(int k=1;k<USER_NUM+1;k++)
				rmse+=beta1*bu[k]*bu[k];
		}
		{
			for(int k=1;k<ITEM_NUM+1;k++)
				rmse+=beta1*bi[k]*bi[k];
		}
		{
			for(int k=1;k<USER_NUM+1;k++)
				for(int kk=1;kk<K_NUM+1;kk++)
					rmse+=beta1*p[k][kk]*p[k][kk];
		}
		{
			for(int k=1;k<ITEM_NUM+1;k++)
				for(int kk=1;kk<K_NUM+1;kk++)
					rmse+=beta1*q[k][kk]*q[k][kk];
		}
		return rmse;
	}

    void model(int dim, float  alpha1, float alpha2, float beta1, float beta2,
               int maxStep=200,double slowRate=1,bool isUpdateBias=true)
    {
        cout << "begin initialization: " << endl;
        //loadCFBias(cfBias, "../knn/cfBias1");
		loadItemContentInfo(CONTENT_SET,itemInfo, RATE_SP);
		loadUserInfo(USER_SET,userInfo, RATE_SP);
        loadRating(TRAINING_SET,rateMatrix,RATE_SP);  //load training set
        loadProbe(PROBE_SET,probeRow,RATE_SP);   //load test set
		loadProbe(ALL_SET,All,RATE_SP);   //load test set
		
		getTheEarlyTime();
		//analysizeItemInfo();
		
		cout<<"time\t"<<Early<<"\t"<<Present<<"\t"<<Early-Present<<endl;
        mean = setMeanRating(USER_NUM,rateMatrix); //calculate the mean
		//analysizeItemInfo(All);
		//analysizeUserInfo(All);
		analysize(All);
		//getItemMean();         //calculate the mean of eauh item
        int i,u,j,k;
        
        srand((unsigned)time(0)); 
 //       initialBais(); //initialize bu and bi
        
        initialPQ(ITEM_NUM, USER_NUM,K_NUM); //intialize the matrix of user uharacter(P) and the matrix of item uharacter(Q) 
		
		cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // the predict value of user u to item i
        double preRmse = 1000000000000.0; //ÓÃÓÚ¼ÇÂ¼ÉÏÒ»¸örmse£¬×÷ÎªÖÕÖ¹Ìõ¼þµÄÒ»ÖÖ£¬Èç¹ûrmseÉÏÉýÁË£¬ÔòÍ£Ö¹
                                          //use to record the previous rmse of test set and make as the terminal condition
                                          //if the rmse of test begin to increase, then break
        double nowRmse = 0.0;																																 //alpha1=0.0045;alpha2=0.0045;
        cout <<"begin testRMSEProbe(): " << endl;
//        RMSEProbe(probeRow,K_NUM);
		RMSEProbeTime(probeRow,K_NUM);
		//RMSEProbeLast(probeRow,K_NUM);
        //main loop
        for(int step = 0; step < maxStep; ++step){  //only iterate maxStep times
            long double rmse = 0.0;
            int n = 0;

            for( u = 1; u < USER_NUM+1; ++u) {   //process every user (Ñ­»·´¦ÀíÃ¿Ò»¸öÓÃ»§)
        //        getP(u, ITEM_NUM,K_NUM);
		//		double sum[K_NUM+1]={0};
				int RuNum = rateMatrix[u].size(); //the num of items rated by user(ÓÃ»§u´ò¹ý·ÖµÄitemÊýÄ¿)
                float sqrtRuNum = 0.0;
                if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
                   
                for(i=0; i < RuNum; ++i) 
				{// process every item rated by user u(Ñ­»·´¦Àíu´ò·Ö¹ýµÄÃ¿Ò»¸öitem)
                    int itemI = rateMatrix[u][i].item;
                    short rui = rateMatrix[u][i].rate; //real rate
					int tui=rateMatrix[u][i].time;
					int tgapu=tu[u]-tui;
					int tgapi=ti[itemI]-tui;
					int uage=userInfo[u].age;
					int uoccupation=userInfo[u].occupation;
					int iyear=itemInfo[itemI].year-1919;
					int ugender=0;
					if(userInfo[u].gender)
						ugender=1;
          //          double bui = mean + bu[u] + bi[itemI]+bt[tui-Present]+
			//			dot(tx[u],tz[tgapu],K_NUM)+ dot(ty[tgapi],ts[itemI],K_NUM);
        //              pui = predictRate(u,itemI,dim);
                    pui = predictRateTime(u,itemI,tui,tgapu,tgapi,dim);
        //            pui = predictRateTimeContent(u,itemI,tui,tgapu,tgapi,itemInfo[itemI].attribute,dim);
                    float eui = rui - pui;
                    
                    //if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
					if(0){
                        cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"    "<<bu[u]<<"    "<<bi[itemI]<<"    "<<mean<<endl;
                        //printArray(q[itemI],p[u],K_NUM+1);
                        exit(1);
                    }
                    rmse += eui * eui; ++n;
                    if(n % 10000000 == 0)cout<<"step:"<<step<<"    n:"<<n<<" dealed!"<<endl;
         /*           
                    for(k=1;k<K_NUM+1;++k)
						sum[k]+=eui*q[itemI][k];
          */          
                    for( k=1; k< K_NUM+1; ++k) {
                        //double tempPu = p[u][k];
                        p[u][k] += alpha2 * (eui*q[itemI][k] - beta2*p[u][k]);
						
                        q[itemI][k] += alpha1 * (eui*p[u][k] - beta1*q[itemI][k]);
						
						tx[u][k]+=alpha2 * (eui*tz[tgapu][k] - beta2*tx[u][k]);
						tz[tgapu][k]+=alpha2 * (eui*tx[u][k] - beta2*tz[tgapu][k]);

						ts[itemI][k]+=alpha2 * (eui*ty[tgapi][k] - beta2*ts[itemI][k]);
						ty[tgapi][k]+=alpha2 * (eui*ts[itemI][k] - beta2*ty[tgapi][k]);

						ax[itemI][k]+=alpha2 * (eui*ay[uage][k] - beta2*ax[itemI][k]);
						ay[uage][k]+=alpha2 * (eui*ax[itemI][k] - beta2*ay[uage][k]);

						ox[itemI][k]+=alpha2 * (eui*oy[uoccupation][k] - beta2*ox[itemI][k]);
						oy[uoccupation][k]+=alpha2 * (eui*ox[itemI][k] - beta2*oy[uoccupation][k]);

						yx[u][k]+=alpha2 * (eui*yy[iyear][k] - beta2*yx[u][k]);
						yy[iyear][k]+=alpha2 * (eui*yx[u][k] - beta2*yy[iyear][k]);

						gx[itemI][k]+=alpha2 * (eui*gy[ugender][k] - beta2*gx[itemI][k]);
						gy[ugender][k]+=alpha2 * (eui*gx[itemI][k] - beta2*gy[ugender][k]);
		//				p[u][k] =ug[u][k]+ alpha2 * (eui*ul[itemI][k]*uh[tgapu][k] - beta2*ug[u][k]);
		//				q[itemI][k] =ul[itemI][k]+ alpha2 * (eui*ug[u][k]*uh[tgapu][k] - beta2*ul[itemI][k]);
		//				uh[tgapu][k] +=alpha2 * (eui*ug[u][k]*ul[itemI][k] - beta2*uh[tgapu][k]);
                    }
					if(1) {
                        bu[u] += alpha1 * (eui - beta1 * bu[u]);
                        bi[itemI] += alpha1 * (eui - beta1 * bi[itemI]);
						bt[tui-Present]+=alpha1 * (eui - beta1 * bt[tui-Present]);
						ba[uage]+=alpha1 * (eui - beta1 * ba[uage]);
						bo[uoccupation]+=alpha1 * (eui - beta1 * bo[uoccupation]);
						by[iyear]+=alpha1 * (eui - beta1 * by[iyear]);
		/*				
						bool *attr=itemInfo[itemI].attribute;
						int count=0;
						for(int kk=0;kk<19;kk++)
							if(attr[kk])
								count++;
						for( kk=0;kk<19;kk++)
						{
							if(attr[kk])
							{
								ca[kk]+=alpha1 * (eui/count - beta1 * ca[kk]);
							}
						}
		*/
                    }
					
                }
/*
				for(i=0; i < RuNum; ++i) 
				{
					int itemI = rateMatrix[u][i].item;
					short rui = rateMatrix[u][i].rate; //real rate
					int tui=rateMatrix[u][i].time;
					int tgapu=tu[u]-tui;
					int tgapi=ti[itemI]-tui;
                   // double bui = mean + bu[u] + bi[itemI]+bt[tui-Present]+
					//	dot(tx[u],tz[tgapu],dim)+ dot(ty[tgapi],ts[itemI],dim);
					double bui = mean + bu[u] + bi[itemI];
					for( k=1; k< K_NUM+1; ++k)
						x[itemI][k]+=alpha2*(sqrtRuNum*(rui-bui)*sum[k]-beta2*x[itemI][k]);   //rui-bui//cfBias[u][itemI
						//x[itemI][k]+=alpha2*(sqrtRuNum*(predictRateBias(u, itemI,i, K_NUM))*sum[k]-beta2*x[itemI][k]);
				}
				for(i=1;i<ITEM_NUM+1; ++i) 
				{
					for( k=1; k< K_NUM+1; ++k)
						y[i][k]+=alpha2*(sum[k]/sqrt(ITEM_NUM)-beta2*y[i][k]);
				}
*/
            }
			
            //nowRmse =  sqrt( rmse / n);
			/*
            nowRmse=RMSEProbeAll(All,K_NUM);
			cout << step << "\t" << nowRmse <<'\t'<< preRmse<<"     n:"<<n<<endl;
            if( nowRmse >= preRmse && step >= 20) break; //if the rmse of test set begin to increase, then break
            else
                preRmse = nowRmse;
            */
 //           RMSEProbe(probeRow,K_NUM);  // uheck rmse of test set 
			RMSEProbeTime(probeRow,K_NUM);
//time content
//			RMSEProbeTimeContent(probeRow,K_NUM);
            //RMSEProbeLast(probeRow,K_NUM);
            alpha1 *= slowRate;    //gradually reduce the learning rate(Öð²½¼õÐ¡Ñ§Ï°ËÙÂÊ)
            alpha2 *= slowRate;
        }
//        RMSEProbe(probeRow,K_NUM);  //  uheck rmse of test set 
		RMSEProbeTime(probeRow,K_NUM);
//		RMSEProbeTimeContent(probeRow,K_NUM);
        //RMSEProbeLast(probeRow,K_NUM);
		return;
    }

	
};

/**
 * predict the rate
 */
float predictRate(int user, int item,int dim)
{
	using namespace svd;
    int RuNum = rateMatrix[user].size(); //the num of items rated by user
    double ret; 
    
    if(RuNum > 1) {
        //ret = mean + bu[user] + bi[item] +  dot(p[user],q[item],dim)+svd::cfBias[user][item];
		ret = mean + bu[user] + bi[item] + dot(p[user],q[item],dim);
			
    }
    //else ret  = mean+bu[user] + bi[item]+svd::cfBias[user][item];
	else ret  = mean+bu[user] + bi[item];
    if(ret < 1.0) ret = 1;
    if(ret > 5.0) ret = 5;
    return ret;
}


#endif // SVD_SVDBASE_CPP_ 




