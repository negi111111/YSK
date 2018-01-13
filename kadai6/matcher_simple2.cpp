#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <iostream>
#define DIM 128
using namespace cv;
using namespace std;
static void help()
{
	printf("\nThis program demonstrates using features2d detector, descriptor extractor and simple matcher\n"
			"Using the SURF desriptor:\n"
			"\n"
			"Usage:\n matcher_simple <image1> <image2> (dist limit)\n");
}
int main(int argc, char** argv)
{
	if(argc < 3)
	{
		help();
		return -1;
	}
	double dist_lim=0.8;
	if(argc>3)dist_lim=atof(argv[3]);

	Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
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

	// matching descriptors
	BFMatcher matcher(NORM_L2);
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);
	
	std::vector<DMatch>::iterator it = matches.begin();
	while (it != matches.end()){
		int f=it->queryIdx,s=it->trainIdx;
		Mat m1=descriptors1.row(f);
		Mat m2u=descriptors2(Range(0,s), Range(0,DIM));
		Mat m2d=descriptors2(Range(s+1,descriptors2.size().height), Range(0,DIM));
		vector<DMatch> mtu,mtd;
		double dis;
		int idx;
		if(s==0){
			matcher.match(m1, m2d, mtd);
			dis=mtd[0].distance;
			idx=mtd[0].trainIdx;
		}else if(s<descriptors2.size().height-1){
			matcher.match(m1, m2u, mtu);
			matcher.match(m1, m2d, mtd);
			dis=mtu[0].distance;
			idx=mtu[0].trainIdx;
			if(dis>mtd[0].distance){
				dis=mtd[0].distance;
				idx=mtd[0].trainIdx;
			}
		}else{
			matcher.match(m1, m2u, mtu);
			dis=mtu[0].distance;
			idx=mtu[0].trainIdx;
		}
		double d=it->distance/dis;
		//cout << f <<","<< idx <<":"<< dis << ","  << d << endl;
		if (d>=dist_lim){
			it = matches.erase(it);
		}else {
			++it;
		}
		//++it;
	}

	for(int i=0;i<matches.size();i++){
		cout << matches[i].queryIdx <<"," 
		<< matches[i].trainIdx << ":"
		<< matches[i].distance << endl;		
	}

	// drawing the results
	namedWindow("matches", 1);
	Mat img_matches;
	drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
	imwrite("matches.jpg",img_matches);
	imshow("matches", img_matches);
	waitKey(0);

	return 0;
}
