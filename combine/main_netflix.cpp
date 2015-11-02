﻿/* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
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
 * 本程序的目的是实现koren在SIGKDD'08论文中的SVD++方法, 使用的数据是netflix的数据
 * The purpose of this program is to implement the asymmetric SVD method of koren's SIGKDD'08 paper,
 * using the netflix dataset.
 */

#include "../commonHeader.h"
#include "../common.cpp"
#include "../netflixBase.cpp"
#define USER_NUM 480189  //10K:943 1M:6040
#define ITEM_NUM 17770 //10K:1682 1M:3900
#define K_NUM  50
#define TRAINING_SET "../dataset/netflix/data_without_prob.txt"
#define PROBE_SET "../dataset/netflix/probe_real.txt"
#define RATE_SP ","  //rate Separator
#include "./svdplusplus_lfgNbr.cpp"

int main(int argc, char ** argv)
{
    time_t start,end;
    struct tm* startInfo;
    struct tm* endInfo;
    double duration;
    start = time(NULL);
    startInfo = localtime(&start);
    string startStr = asctime(startInfo);
    float alpha1 = 0.007;    //according suggestion of koren SIGKDD'08
    float alpha2 = 0.007;    //according suggestion of koren SIGKDD'08
    float alpha3 = 0.001;    //according suggestion of koren SIGKDD'08
    float beta1  = 0.005;     //according suggestion of koren SIGKDD'08
    float beta2  = 0.015;     //according suggestion of koren SIGKDD'08
    float beta3  = 0.015;     //according suggestion of koren SIGKDD'08
    //for(int i=0; i < 10; i++)
    {
        //    beta = i*0.001 + 0.002;
        //    cout << beta << endl;
        //void model(int dim, float  alpha1, float alpha2, float alpha3, float beta1, float beta2, float beta3,
        //         int maxStep=60,double slowRate=1,bool isUpdateBias=true)
        svd::model(K_NUM,alpha1,alpha2,alpha3,beta1,beta2,beta3,60,0.9);
    }
    end = time(NULL);
    duration = end-start;
    endInfo =   localtime(&end);
    cout << "start at" << startStr << ". And end at "<< asctime(endInfo) <<endl;
    cout << "duration:"<<duration <<" s!" <<endl;
    return 0;
}
