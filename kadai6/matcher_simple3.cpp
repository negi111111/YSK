/*

matcher_simple.cpp

特徴点マッチング

*/
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;
float eucred(float d1[],float d2[]);

static void help()
{
    printf("\nThis program demonstrates using features2d detector, descriptor extractor and simple matcher\n"
            "Using the SURF desriptor:\n"
            "\n"
            "Usage:\n matcher_simple <image1> <image2> <sikiiti>\n");
}

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        help();
        return -1;
    }

    Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
    float thresh=atof(argv[3]);
    if(img1.empty() || img2.empty())
    {
        printf("Can't read one of the images\n");
        return -1;
    }

    // detecting keypoints
    SurfFeatureDetector detector(400);
    vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(img1, keypoints1);
    detector.detect(img2, keypoints2);

    // computing descriptors
    SurfDescriptorExtractor extractor;
    Mat descriptors1, descriptors2;
    extractor.compute(img1, keypoints1, descriptors1);
    extractor.compute(img2, keypoints2, descriptors2);

    int row1= descriptors1.rows;

    int row2= descriptors2.rows;

    int col=  descriptors1.cols;
    vector<DMatch> matches;
////修正部分
     int i,j,r,index,k=0;
      float res[row2],min1,min2,rate;
     float match[row1];
    for(i=0;i<row1;i++){
	float *d1=descriptors1.ptr<float>(i);
	min1=10; min2=11; index=0;
	for(j=0;j<row2;j++){
		float *d2=descriptors2.ptr<float>(j);
		res[j]=eucred(d1,d2);//ユークリッド距離
		if(res[j]<min1){min2=min1; min1=res[j]; index=j;}//最小値とその次を記録していく
	}
	rate=min1/min2;
	if(min1*thresh<min2) {//閾値で除去
		if(rate<0.8){
			matches.push_back(DMatch(i,index,min1)); k++;//比率で除去
			}
		}
	//cout <<min1 << " t " <<thresh << " s " << min1*thresh << " aa " << min2 << index<<" ra "<<rate<< endl;
	}
	cout << k << endl;

///修正部分

///追加部分

/*
    // matching descriptors
    BFMatcher matcher(NORM_L2);
    matcher.match(descriptors1, descriptors2, matches);
    cout << matches[0].trainIdx << endl;
*/
    

    // drawing the results
    namedWindow("matches", 1);
    Mat img_matches;
    drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
    imwrite("output.jpg", img_matches);

    return 0;
}
float eucred(float d1[],float d2[])
{
     	  int q;
     	  float a,b,c,res;
	  c=0;
     	  for(q=0;q<128;q++){
		a=d1[q]-d2[q];
		b=a*a;
		c+=b;
     	  }
	res=sqrt(c);
     	  return res;
}

