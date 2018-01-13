/*
  Bag-of-features 生成のサンプルコード
  入力：JPEGファイル名のリスト
  出力：各JPEGファイルに対応するBoFベクトル
*/

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int DIM_SURF=128;     /* SURF特徴の次元数 */
const int MAX_SURF=1000000; /* 特徴の最大数 */
const int MAX_IMG =10000;   /* 画像の最大枚数 */
const int ITERATION=100;    /* k-Meansの最大反復回数 */
const int CODEBOOK_SIZE=500;/* コードブックのサイズ */
const int GRID=10;          /* grid samplingのピクセル間隔 */
const int scales[]={30,23,18,15,12,0}; /* grind samplingでの抽出スケール */

int main(int argc, char** argv)
{
    int num_img=0,num_surf=0;
    char fn[256];
    FILE *fp;
    int   i,j,hist[CODEBOOK_SIZE];
    int   *img_id   =(int*)malloc((MAX_SURF+1)*sizeof(int));
    int   *vword_idx=(int*)malloc(MAX_SURF*sizeof(int));
    float *desc     =(float*)malloc(MAX_SURF*DIM_SURF*sizeof(float)),*v;
    
    CvMat clusters, data;
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSURFParams  params = cvSURFParams(500, 1); /* 128-dim Extended SURF を指定 */
    CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
    CvSURFPoint *pt=(CvSURFPoint*)malloc(sizeof(CvSURFPoint));
    pt->dir=0;
   
    if (argc<=1) { 
       fprintf(stderr,"usage: generate_bof {img_list}\n"); exit(1); 
    }
    if (!(fp=fopen(argv[1],"r"))) {
       fprintf(stderr,"not found %s\n",argv[1]); exit(1);
    }
    while(fgets(fn,255,fp)){
      fn[strlen(fn)-1]='\0';
      IplImage* image = cvLoadImage( fn, CV_LOAD_IMAGE_GRAYSCALE );
      if (!image) continue;
      num_img++;
      /* Grid sampling の grid pointsのリストの作成 */
      imageKeypoints=cvCreateSeq(0,sizeof(CvSeq),sizeof(CvSURFPoint),storage);
      int startx = (int)((image->width % GRID) + GRID )/2 -1;
      int starty = (int)((image->height % GRID) + GRID )/2 -1;
      int x,y;
      int const *s=scales;
      for(;*s>0;s++){
        for(y=starty;y<image->height;y+=GRID){
          for(x=startx;x<image->width;x+=GRID){
            pt->pt.x=x; pt->pt.y=y; pt->size=*s;
   	    cvSeqPush(imageKeypoints,pt);
	  }
	}
      }
      /* 設定した grid pointsのリストを用いて imageDescriptors を抽出 */
      /* ExtractSURFの最後のパラメータを0にすると通常のFast HessianによるSURFとなる．*/
      cvExtractSURF( image, NULL, &imageKeypoints, &imageDescriptors, storage, params, 1);
      if (imageKeypoints->total + num_surf > MAX_SURF) break;  
      printf("[%d] Reading %s (#surf points:%d) \n",num_img,fn,imageKeypoints->total);
 
      for(i=0;i<imageKeypoints->total;i++)  {
        v = (float*)cvGetSeqElem( imageDescriptors, i );
	for(j=0;j<128;j++) desc[DIM_SURF*num_surf+j]=v[j]*1000;
        img_id[num_surf++]=num_img;
      }
      cvClearSeq(imageKeypoints); 
      cvClearSeq(imageDescriptors);
      cvReleaseImage(&image);
    }
    fclose(fp);
    img_id[num_surf]=0;
    printf("number of images: %d\nnumber of SURF descriptors: %d\n",num_img,num_surf);

    cvInitMatHeader(&clusters, num_surf, 1, CV_32SC1, vword_idx, CV_AUTOSTEP );
    cvInitMatHeader(&data, num_surf, DIM_SURF, CV_32FC1, desc, CV_AUTOSTEP);

    /* k-means によるコードブックの生成 */
    printf("Running k-means clustering ! \n");
    cvKMeans2(&data, CODEBOOK_SIZE, &clusters, cvTermCriteria(
    CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, ITERATION, 0.01 ),1,0,cv::KMEANS_PP_CENTERS,0,0);

    /* 得られた各SURF特徴点に対するクラスタ番号を使って BoFベクトルを生成 */
    bzero(hist,sizeof(int)*CODEBOOK_SIZE);
    for(i=0;i<num_surf && img_id[i]>0;i++){
      hist[vword_idx[i]]++;
      if (img_id[i]!=img_id[i+1]){
	printf("[%d] ",img_id[i]);
	for(j=0;j<CODEBOOK_SIZE;j++)
	  printf("%d ",hist[j]);
	printf("\n");
	bzero(hist,sizeof(int)*CODEBOOK_SIZE);
      }
    }
    free(img_id); free(vword_idx); free(desc); free(pt);
}

