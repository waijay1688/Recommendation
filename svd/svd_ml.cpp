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
 * 本程序的目的是实现koren在SIGKDD'08论文中的方法，svd model, the dataset is movielens
 * The purpose of this program is to implement the in the SVD method of koren's SIGKDD'08 paper,
 * using the movielens dataset.
 *
 */
 
#include "../commonHeader.h"
#include "../common.cpp"
#include "../mlBase.cpp"
#define USER_NUM 6040 //10K:943 1M:6040
#define ITEM_NUM 3952 //10K:1682 1M:3900
#define TIME_NUM 113   //the tiem gap 0 185     897  1795  449
#define ATTRIBUTE_NUM 19
#define OCCUPATION_NUM 21
#define AGE_NUM 7
#define YEAR_NUM 82
#define GENDER_NUM 2
#define K_NUM  60     //dimension
#define TRAINING_SET "C:/Users/hptoxic/Downloads/ml-1m/u2.base" //training set E:\TDDOWNLOAD\ml-100k\ml-100k  //C:\Users\hptoxic\Downloads\ml-1m//../dataset/movielens/u1.base
#define PROBE_SET "C:/Users/hptoxic/Downloads/ml-1m/u2.test"  //test set
#define CONTENT_SET "C:/Users/hptoxic/Downloads/ml-1m/movies.dat"
#define USER_SET "C:/Users/hptoxic/Downloads/ml-1m/users.dat"
#define ALL_SET "C:/Users/hptoxic/Downloads/ml-1m/u.all"

//#define TRAINING_SET "E:/TDDOWNLOAD/ml-10m/ml-10M100K/ratings.dat.train" //training set 
//#define PROBE_SET "E:/TDDOWNLOAD/ml-10m/ml-10M100K/ratings.dat.test"  //test set
#define RATE_SP "	"  //rate Separator
//#define RATE_SP ","  //rate Separator
#include "./svdBase.cpp"

int main(int argc, char ** argv)
{
    time_t start,end;
    struct tm * timeStartInfo;
    struct tm * timeEndInfo;
    double duration; 
    start = time(NULL);
    timeStartInfo = localtime(&start);
    string timeStartStr = asctime(timeStartInfo);
    float alpha1 = 0.005;  //0.0045according to the paper of "a guide to SVD for CF" 0.0035  0.015
    float alpha2 = 0.005;  //0.0045according to the paper of "a guide to SVD for CF"
    float beta1 = 0.035;   //0.015 according to the paper of "a guide to SVD for CF"
    float beta2 = 0.035;   //according my own experiment,beta=0.05 is very good ,the RMSE of movielens(1M) test 
    //can reach 0.868692  at step=44
    
    //for(int i=0; i < 10; i++)
    {
        //alpha = i*0.0005 + 0.0025;
        //cout << alpha << endl;
        //void model(int dim, float  alpha1, float alpha2, float beta1, float beta2,
        // int maxStep=60,double slowRate=1,bool isUpdateBias=true)
        svd::model(K_NUM,alpha1,alpha2,beta1,beta2,200,0.99,true);
    }
    end = time(NULL);
    duration = (end-start);
    timeEndInfo = localtime(&end);
    cout << "Start at "<<timeStartStr<<", and end at "<< asctime(timeEndInfo);
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
