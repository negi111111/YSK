/**************************************************
 **************************************************/
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <stdio.h>
#include <dirent.h>

const int VWIDTH=320;
const int VHEIGHT=240;

/* mpegファイルのディレクトリ */

int main(int argc,char **argv) {
    // cvCalcOpticalFlowHS, cvCalcOpticalFlowLK
    CvCapture* capture = NULL; // 画像処理に必要なIplImage などの宣言
    CvVideoWriter *vw;
    IplImage *imgA = NULL; // 生画像
    IplImage *grayA = NULL; // グレー・スケール画像
    IplImage *grayA_old = NULL; // グレー・スケール画像(1 フレーム前)
    IplImage *velx = NULL; // オプティカル・フロー計算結果X 成分格納
    IplImage *vely = NULL; // オプティカル・フロー計算結果Y 成分格納
    int key; // キー入力格納
    int flag=0; // キャプチャ枚数のフラグ格納
    CvTermCriteria criteria;
    char str[256];
    CvFont font;
    int num=0;
    if (argc<3) 
     { printf("usage: %s {movie file (mpg,avi,flv)} {out.file}\n",argv[0]); exit(1); }

    // 動画ファイルをビデオキャプチャ変数に格納
    if (!(capture = cvCaptureFromFile(argv[1]))) {
        fprintf( stderr, "%s Not Found!\n", argv[1]);
        return -1;
    }
    printf("read OK\n");
    // 1 枚画像を取得し，imgA のさまざまな情報を取得
    imgA = cvQueryFrame(capture);
    int width = imgA->width;
    int height = imgA->height;
    int pix_sum = width*height;

    printf("get one frame\n");

    double fps=cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);
    double frames=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
    char codec[5]; //double *cd=(double *)codec;
    double cd=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
    codec[4]='\n';
    printf("fps: %f\n",fps);
    printf("frames: %f\n",frames);
    printf("codec: %f\n",cd);
    //    printf("codec: %s\n",codec);

    IplImage *dst = cvCreateImage( cvSize(VWIDTH,VHEIGHT),IPL_DEPTH_8U, 3);
    if (!(vw = cvCreateVideoWriter(argv[2],CV_FOURCC('M','P','4','2'),fps/2,cvSize(VWIDTH,VHEIGHT)))) {
      //    if (!(vw = cvCreateVideoWriter(argv[2],CV_FOURCC('M','P','4','2'),fps,cvSize(width,height)))) {
    //    if (!(vw = cvCreateVideoWriter(argv[2],-1,fps,cvSize(width,height)))) {
        fprintf( stderr, "%s Not Found!\n", argv[1]);
        return -1;
    }
    printf("write OK\n");

    cvInitFont(&font,CV_FONT_HERSHEY_TRIPLEX,0.5,0.5);
    //cvInitFont(&font,CV_FONT_HERSHEY_SCRIPT_COMPLEX,0.7,0.7);
    // 必要な画像領域の生成
    grayA =cvCreateImage(cvGetSize(imgA),IPL_DEPTH_8U,1);
    grayA_old=cvCreateImage(cvGetSize(imgA),IPL_DEPTH_8U,1);
    velx =cvCreateImage(cvGetSize(imgA),32,1);
    vely =cvCreateImage(cvGetSize(imgA),32,1);

    // ウィンドウの生成
    //    cvNamedWindow("OpticalFlow",CV_WINDOW_AUTOSIZE);

    for(;;) {
      num++;
        imgA = cvQueryFrame(capture); // 動画像の取得
      num++;
        imgA = cvQueryFrame(capture); // 動画像の取得
      num++;
        imgA = cvQueryFrame(capture); // 動画像の取得
        grayA->origin = imgA->origin; // キャプチャ画像(new)の原点情報(左下)にコピー
        grayA_old->origin = imgA->origin; // キャプチャ画像(old)の原点情報(左下)にコピー
        cvCvtColor(imgA,grayA,CV_BGR2GRAY); // キャプチャ画像をグレー・スケールに変換する

        // 1 回目の画像キャプチャ時の処理
        if(flag==0) {
            flag=1;
            cvCopy(grayA,grayA_old,NULL);
        }

        // 2 回目以降の画像キャプチャ時の処理
        else {
            // オプティカル・フローの計算(LK)
            cvCalcOpticalFlowLK(grayA_old,grayA,cvSize(11,11),velx,vely);

            // オプティカル・フローの計算(HS)
            //criteria = cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 64, 0.01);
            //cvCalcOpticalFlowHS (grayA_old,grayA, 0, velx, vely, 100.0, criteria);

            // オプティカル・フローの量を算出
            double opticalflow = 0.0;
            for (int i=0;i<height;i+=1) {
                for (int j=0;j<width;j+=1) {
                    double dx = (double)cvGetReal2D(velx,i,j);
                    double dy = (double)cvGetReal2D(vely,i,j);
                    dx /= (double) width;
                    dy /= (double) height;
                    double d = sqrt(dx*dx+dy*dy);
                    opticalflow += d;
                }
            }
	    //printf("[%3d] ",num);
            //printf("%f  \r", opticalflow / pix_sum);
            //if (num%10==0) printf("\n");
            // オプティカル・フローの値をキャプチャした画像に描画する(線)
            for (int i=0;i<height;i+=10) {
                for (int j=0;j<width;j+=10) {
                    int dx = (int)cvGetReal2D(velx,i,j);
                    int dy = (int)cvGetReal2D(vely,i,j);
                    cvLine(imgA,cvPoint(j,i),cvPoint(j+dx,i+dy),
                           CV_RGB(255,0,0),1,CV_AA,0);
                }
            }
        }
        if (num>=70 && num<=130) {
            for (int i=0;i<height;i++) 
              for (int j=0;j<width;j++) 
                for (int k=0;k<3;k++) {
		  int v=imgA->imageData[(j+i*width)*3+k]=
		    imgA->imageData[(j+i*width)*3+k]=255;
		}
        }
	cvResize(imgA,dst,CV_INTER_CUBIC); 
        snprintf (str, 64, "Hiroto in Tamagawa", num);
        cvPutText (dst, str, cvPoint (8, 18), &font, CV_RGB (0, 10, 255));
        snprintf (str, 64, "%03d [frame]", num);
        cvPutText (dst, str, cvPoint (8, 35), &font, CV_RGB (0, 10, 255));
        cvWriteFrame(vw,dst);
	//        cvShowImage("OpticalFlow",imgA); // キャプチャした画像の表示
        cvCopy(grayA,grayA_old,NULL); // キャプチャした画像(old)を保存
	//        key = cvWaitKey(10); // キー入力
        if(num>500) { // ESC で終了
            printf("ESC key Quit\n");
            break;
        }
    }

    // メモリ開放
    cvReleaseCapture(&capture);
    cvReleaseVideoWriter(&vw);
    //    cvDestroyWindow("Show");
    return(0); // 正常終了
}
