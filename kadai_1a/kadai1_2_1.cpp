#include <cv.h>
#include <highgui.h>
 
using namespace cv;
 
int
main(int argc, char **argv)
{
  // (1)load a specified file and convert it into grayscale image
  const char *imagename = argc > 1 ? argv[1] : "/home/yanai-lab/tanno-r/kadai/lena/jpg";
  Mat src_img = imread(imagename);
  if(!src_img.data)
    return -1;
  Mat gray_img;
  cvtColor(src_img, gray_img, CV_BGR2GRAY);
   
  // (2)apply a fixed-level threshold to each pixel
  Mat bin_img, bininv_img, trunc_img, tozero_img, tozeroinv_img;
  threshold(gray_img, bin_img, 0, 255, THRESH_BINARY|THRESH_OTSU);
  threshold(gray_img, bininv_img, 0, 255, THRESH_BINARY_INV|THRESH_OTSU);
  threshold(gray_img, trunc_img, 0, 255, THRESH_TRUNC|THRESH_OTSU);
  threshold(gray_img, tozero_img, 0, 255, THRESH_TOZERO|THRESH_OTSU);
  threshold(gray_img, tozeroinv_img, 0, 255, THRESH_TOZERO_INV|THRESH_OTSU);
 
  // (3)apply an adaptive threshold to a grayscale image
  Mat adaptive_img;
  adaptiveThreshold(gray_img, adaptive_img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 8);
 
  // (4)show source and destination images
  namedWindow("Source", CV_WINDOW_AUTOSIZE);
  namedWindow("Gray", CV_WINDOW_AUTOSIZE);
  /*namedWindow("Binary", CV_WINDOW_AUTOSIZE);
  namedWindow("Binary Inv", CV_WINDOW_AUTOSIZE);
  namedWindow("Trunc", CV_WINDOW_AUTOSIZE);
  namedWindow("ToZero", CV_WINDOW_AUTOSIZE);
  namedWindow("ToZero Inv", CV_WINDOW_AUTOSIZE);
  namedWindow("Adaptive", CV_WINDOW_AUTOSIZE);*/
  
  imshow("Source", src_img);
  imshow("Gray", gray_img);
  /*imshow("Binary", bin_img);
  imshow("Binary Inv", bininv_img);
  imshow("Trunc", trunc_img);
  imshow("ToZero", tozero_img);
  imshow("ToZero Inv", tozeroinv_img);
  imshow("Adaptive", adaptive_img);*/
  
  waitKey(0);
 
  return 0;
}
