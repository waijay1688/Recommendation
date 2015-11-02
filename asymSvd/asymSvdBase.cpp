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
 * asymSvdBase.cpp contains some common functions of asymmetric-svd model.
 * this model is decribed in the page 5 of koren's SIGKDD'08 paper.
 */
#ifndef ASYMSVD_ASYMSVDBASE_CPP_
#define ASYMSVD_ASYMSVDBASE_CPP_
namespace svd{
    //use some global variables, store the parameter bu, bi, p, q,y
    double bu[USER_NUM+1] = {0};       // the user bias in the baseline predictor
    double bi[ITEM_NUM+1] = {0};       // the item bias in the baseline predictor
    float buBase[USER_NUM+1] = {0};
    float biBase[ITEM_NUM+1] = {0};       //stored and unchanged bias of user and item
    
    int buNum[USER_NUM+1] = {0};       //num of user ratings
    int biNum[ITEM_NUM+1] = {0};       //num of item ratings
    
    double pu[USER_NUM+1][K_NUM+1] = {0};   //store complete character of user
    double q[ITEM_NUM+1][K_NUM+1] = {0};   // item character Matrix
    double  x[ITEM_NUM+1][K_NUM+1];          //explicit weight x
    double y[ITEM_NUM+1][K_NUM+1] = {0};   //implicit weight y
    float mean = 0;                         //mean of all ratings
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);   //store training set
    vector<testSetNode> probeRow;                            //store test set
    
    //initialize the bias bu and bi, the method in the page 2 of koren's TKDD'09 paper
    void initialBais()
    {
        using namespace svd;
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
            biBase[i] = bi[i];
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
            buBase[i] = bu[i];
        }
    }
    
    //intialize the matrix of explicit character(X), the matrix of item character(Q) and implicit matrix Y
    void initialPQ(int itemNum, int userNum,int dim)
    {
        using namespace svd;
        //int i;
        //@TODO should do some optimization to the initialization
        for(int i = 1; i < itemNum+1; ++i){
            setRand(x[i],dim,0); 
            setRand(q[i],dim,0);
            setRand(y[i],dim,0); 
        }
    }
    
    void model(int dim, float  alpha1, float alpha2, float beta1, float beta2,
               int maxStep=60,double slowRate=1,bool isUpdateBias=true)
    {
        cout << "begin initialization: " << endl;
        loadRating(TRAINING_SET,rateMatrix,RATE_SP);  //load training set
        loadProbe(PROBE_SET,probeRow,RATE_SP);   //load test set
        mean = setMeanRating(USER_NUM,rateMatrix); //calculate the mean
        int i,u,j,k;
        
        srand((unsigned)time(0)); 
        //initialBais(); //initialize bu and bi
        
        initialPQ(ITEM_NUM, USER_NUM,K_NUM); //intialize the matrix of user character(P) and the matrix of item character(Q) 
        
        //initialize pu
        cout <<"begin compute first pu: " << endl;
        for( u = 1; u < USER_NUM+1; ++u) {   // process every user 
            int RuNum = rateMatrix[u].size(); // process every item rated by user u
            float sqrtRuNum = 0.0;
            if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
               
            //calculate pu
            for( k=1; k< K_NUM+1; ++k) {
                float sumx = 0.0;
                float sumy = 0.0;
                   
                for(j=0; j < RuNum; ++j) {
                    int itemI = rateMatrix[u][j].item;
                    short rui =  rateMatrix[u][j].rate;
                    float bui = mean + buBase[u] + biBase[itemI];
                    sumx += (rui-bui) * x[itemI][k];
                    sumy += y[itemI][k];
                }
                pu[u][k] = sqrtRuNum*(sumx + sumy);
            }
        }
        cout <<"end compute first pu! " << endl;
        
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        float pui = 0.0 ; //  the predict value of user u to item i
        double preRmse = 1000000000000.0; //use to record the previous rmse of test set and make as the terminal condition
                                          //if the rmse of test begin to increase, then break
        double nowRmse = 0.0;
        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe(probeRow,K_NUM);
        //main loop
        for(int step = 0; step < maxStep; ++step){  //only iterate maxStep times
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //process every user 
                int RuNum = rateMatrix[u].size(); //process every item rated by user u
                float sqrtRuNum = 0.0;
                if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
                
                
                //calculate pu
                for( k=1; k< K_NUM+1; ++k) {
                    float sumx = 0.0;
                    float sumy = 0.0;
                       
                    for(j=0; j < RuNum; ++j) {
                        int itemI = rateMatrix[u][j].item;
                        short rui =  rateMatrix[u][j].rate;
                        float bui = mean + buBase[u] + biBase[itemI];
                        sumx += (rui-bui) * x[itemI][k];
                        sumy += y[itemI][k];
                    }
                    pu[u][k] = sqrtRuNum*(sumx + sumy);
                }
                   
                float sum[K_NUM+1] = {0};   //store the intermedia variables
                   
                for(i=0; i < RuNum; ++i) {// process every item rated by user u
                    int itemI = rateMatrix[u][i].item;
                    short rui = rateMatrix[u][i].rate; //real rate
                    //double bui = mean + bu[u] + bi[itemI];
                    double bui = mean + buBase[u] + biBase[itemI];
                    pui = predictRate(u,itemI,dim);  //predict rate
                    
                    float eui = rui - pui;
                    
                    //if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
					if(0){
                        cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"    "<<bu[u]<<"    "<<bi[itemI]<<"    "<<mean<<endl;
                        exit(1);
                    }
                    rmse += eui * eui; ++n;
                    if(n % 10000000 == 0)cout<<"step:"<<step<<"    n:"<<n<<" dealed!"<<endl;
                    
                    if(isUpdateBias) {
                        bu[u] += alpha1 * (eui - beta1 * bu[u]);
                        bi[itemI] += alpha1 * (eui - beta1 * bi[itemI]);
                    }
                    
                    for( k=1; k< K_NUM+1; ++k) {
                        sum[k] = sum[k]+ eui*q[itemI][k];
                        q[itemI][k] += alpha2 * (eui*pu[u][k] - beta2*q[itemI][k]);
                    }
                }
                
                for(i=0; i < RuNum; ++i) {//
                    int itemI = rateMatrix[u][i].item;
                    short rui = rateMatrix[u][i].rate; //real rate
                    for( k=1; k< K_NUM+1; ++k) {
                           
                        x[itemI][k] += alpha2 * (sqrtRuNum*sum[k]*(rui-mean-buBase[u]-biBase[itemI]) - beta2*x[itemI][k]);
                        y[itemI][k] += alpha2 * (sqrtRuNum*sum[k] - beta2*y[itemI][k]);
                        /*
                          x[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k]*(rui-bui) - beta*x[itemI][k]);
                          y[itemI][k] += alpha * (eui*sqrtRuNum*q[itemI][k] - beta*y[itemI][k]);
                        */
                    }
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            if( nowRmse >= preRmse && step >= 3) break; //if the rmse of test set begin to increase, then break
            else
                preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<"     n:"<<n<<endl;
            RMSEProbe(probeRow,K_NUM);;  // check test set rmse
            
            alpha1 *= slowRate;    //gradually reduce the learning rate
            alpha2 *= slowRate;
        }
        RMSEProbe(probeRow,K_NUM);  // check the rmse of test set
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
    if(RuNum >= 1) {
        ret = mean + bu[user] + bi[item] +  dot(pu[user],q[item],dim);
    }
    else ret  = mean+bu[user] + bi[item];
    if(ret < 1.0) ret = 1;
    if(ret > 5.0) ret = 5;
    return ret;
}
#endif // ASYMSVD_ASYMSVDBASE_CPP_ 
