#include <cv.h>
#include <highgui.h>

IplImage *markers = 0, *dsp_img = 0;

/* マウスイベント用コールバック関数 */
void
on_mouse (int event, int x, int y, int flags, void *param)
{
  int seed_rad = 20;
  static int seed_num = 0;
  CvPoint pt;

  // (1)クリックにより中心を指定し，円形のシード領域を設定する
  if (event == CV_EVENT_LBUTTONDOWN) {
    seed_num++;
    pt = cvPoint (x, y);
    cvCircle (markers, pt, seed_rad, cvScalarAll (seed_num), CV_FILLED, 8, 0);
    cvCircle (dsp_img, pt, seed_rad, cvScalarAll (255), 3, 8, 0);
    cvShowImage ("image", dsp_img);
  }
}

/* メインプログラム */
int
main (int argc, char **argv)
{
  int *idx, i, j;
  IplImage *src_img = 0, *dst_img = 0;

  // (2)画像の読み込み，マーカー画像の初期化，結果表示用画像領域の確保を行なう
  if (argc >= 2)
    src_img = cvLoadImage (argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
  if (src_img == 0)
    exit (-1);
  dsp_img = cvCloneImage (src_img);
  dst_img = cvCloneImage (src_img);

  markers = cvCreateImage (cvGetSize (src_img), IPL_DEPTH_32S, 1);
  cvZero (markers);

  // (3)入力画像を表示しシードコンポーネント指定のためのマウスイベントを登録する
  cvNamedWindow ("image", CV_WINDOW_AUTOSIZE);
  cvShowImage ("image", src_img);
  cvSetMouseCallback ("image", on_mouse, 0);
  cvWaitKey (0);

  // (4)watershed分割を実行する  
  cvWatershed (src_img, markers);

  // (5)実行結果の画像中のwatershed境界（ピクセル値=-1）を結果表示用画像上に表示する
  for (i = 0; i < markers->height; i++) {
    for (j = 0; j < markers->width; j++) {
      idx = (int *) cvPtr2D (markers, i, j, NULL);
      if (*idx == -1)
        cvSet2D (dst_img, i, j, cvScalarAll (255));
    }
  }

  cvNamedWindow ("watershed transform", CV_WINDOW_AUTOSIZE);
  cvShowImage ("watershed transform", dst_img);
  cvWaitKey (0);

  cvDestroyWindow ("watershed transform");
  cvReleaseImage (&markers);
  cvReleaseImage (&dsp_img);
  cvReleaseImage (&src_img);
  cvReleaseImage (&dst_img);

  return 1;
}

