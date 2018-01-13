/*
高速近似最近傍探索ライブラリ FLANN (Fast Library for Approximate Nearest Neighbour) 
*mather_simple.cppの BFMatcher matcher(NORM_L2); を FlannBasedMatcher matcher; に変更
*

*/

#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;
//add
using namespace std;

static void help()
{
    printf("\nThis program demonstrates using features2d detector, descriptor extractor and simple matcher\n"
            "Using the SURF desriptor:\n"
            "\n"
            "Usage:\n matcher_simple <image1> <image2>\n");
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        help();
        return -1;
    }

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
    
    //BFMatcher matcher(NORM_L2);を下に変更
    
    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    // drawing the results
    namedWindow("matches", 1);
    Mat img_matches;
    //drawMatches : 2つの画像から得られるキーポイント同士のマッチを，出力画像上に描画します
    drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
    	//img_matchesに結果を格納

/*
    img1 – 1番目の入力画像．
    keypoints1 – 1番目の入力画像から得られたキーポイント．
    img2 – 2番目の入力画像．
    keypoints2 – 2番目の入力画像から得られたキーポイント．
    matches – 1番目と2番目の画像間のマッチ．つまり， keypoints1[i] は，点 keypoints2[matches[i]] に対応します．
    outImg – 出力画像．出力画像に何が描画されるかは， flags の値に依存します．後述の，取り得る flags のビット値を参照してください．
    matchColor – マッチの色（線分と，それで接続されるキーポイント）． matchColor==Scalar::all(-1) の場合，ランダムに色が生成されます．
    singlePointColor – シングルキーポイント，つまり，マッチを持たないキーポイントの色（円）． singlePointColor==Scalar::all(-1) の場合，ランダムに色が生成されます．
    matchesMask – どのマッチが描画されるかを指定するマスク．これが空の場合は，すべてのマッチが描画されます．
    flags – flags の各ビットは，描画のプロパティを設定します．取り得る flags ビット値は， DrawMatchesFlags で定義されます．以下を参照してください．
*/

    imshow("matches", img_matches);
    
  
    
    
    waitKey(0);

    return 0;
}
