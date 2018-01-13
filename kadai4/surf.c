/*
  SURF feature extraction
*/

#include <cv.h>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

inline int random_int(int v)
{
  return (int)(random()/((double)RAND_MAX+1)*v);
}

static CvScalar colors={0,0,255};

static int scales[]={30,23,18,15,12,0};

int main(int argc, char** argv)
{
    int idx=0,ext=0,bin=0,color=0;
    int thre=500,show=0,grid=0,rand=0,crop=0,resize=0;
    char *fout=NULL;
    FILE *fp,*fbin=NULL,*fbin2=NULL;;

    char fout0[255],fout1[255],fout2[255],fout3[255],*ptr;
 
    struct timeval tv;
    struct timezone tz;
   
    gettimeofday(&tv,&tz);
    srandom(tv.tv_usec);

    CvMemStorage* storage  = cvCreateMemStorage(0);
    CvMemStorage* storage2 = cvCreateMemStorage(0);

    if( argc<=1 ){
        fprintf( stderr,"Usage: surf [<img_filename>]\n"
                        "       -d      : show an image in a window\n"
                        "       -y      : color SURF (Y Cb Cr) (64*d dim / 128*d dim)\n"
		        "       -s {px} : resize so that the long side gets N pixels (only shrinking)\n"
		        "       -c {px} : crop N pixels on four sides\n"
                        "       -g {num}: sample grid points with every {num} pixels\n"
	                "       -r {num}: sample {num} random points\n"
                        "       -t {thre}: set Hessian threshold (default:500)\n" 
		        "       -e {num} : 0:normal (64dim, default), 1:extended (128dim) 2:both\n"
		        "       -o {file}: output file\n"
		        "       -b      : output binary file (.desc(float(4byte)*64d)/.desc128(float(4byte)*128d))\n"
        );
        exit(-1);
    }
    while (argv[idx+1][0]=='-'){
      idx++;
      switch(argv[idx][1]){
        case 'd':
          show=1;
          break;
        case 'y':
          color=1;
          break;
        case 's':
          resize=atoi(argv[++idx]);
          break;
        case 'c':
          crop=atoi(argv[++idx]);
          break;
        case 't':
          thre=atoi(argv[++idx]);
          break;
        case 'g':
          grid=atoi(argv[++idx]);
	  if (grid>100) { fprintf(stderr,"err: grid is too large\n"); exit(1); }
          break;
        case 'r':
          rand=atoi(argv[++idx]);
          break;
        case 'o':
          fout=argv[++idx];
          break;
        case 'b':
	  bin=1;
          break;
        case 'e':
          if (!isdigit(argv[idx+1][0])) ext=1;
          else ext=atoi(argv[++idx]);
	  if (ext>=2) { ext=2; bin=1; }
          break;
      }
    }

    IplImage *image,*tmp,*tmp1,*tmp2,*image_cb,*image_cr;
    if (color){
      tmp = cvLoadImage (argv[idx+1], CV_LOAD_IMAGE_COLOR);
      image = cvCreateImage (cvGetSize (tmp), IPL_DEPTH_8U, 3);
      cvCvtColor(tmp,image,CV_BGR2YCrCb);
    }else
      image = cvLoadImage( argv[idx+1], CV_LOAD_IMAGE_GRAYSCALE );

    if( !image ){
        fprintf( stderr, "Can not load \nUsage: surf [<img_filename>]\n");
        exit(-1);
    }
    if (crop){
       tmp = image;
       image = cvCreateImage( cvSize(tmp->width-crop*2,tmp->height-crop*2), IPL_DEPTH_8U, tmp->nChannels);
       CvPoint2D32f center;
       center.x=tmp->width/2;
       center.y=tmp->height/2;
       cvGetRectSubPix( tmp, image, center);
    }
    if (resize && (image->width > resize || image->height > resize)){
       tmp = image;
       int hei,wid;
       if (tmp->width >= tmp->height){
	 wid=resize;
	 hei=tmp->height * resize / tmp->width;
       }else{
	 wid=tmp->width * resize / tmp->height;
	 hei=resize;
       }
       image = cvCreateImage( cvSize(wid,hei), IPL_DEPTH_8U, image->nChannels);
       cvResize(tmp, image, CV_INTER_AREA);
    }
    if (color){
      tmp=image;
      image    = cvCreateImage (cvGetSize (tmp), IPL_DEPTH_8U, 1);
      image_cb = cvCreateImage (cvGetSize (tmp), IPL_DEPTH_8U, 1);
      image_cr = cvCreateImage (cvGetSize (tmp), IPL_DEPTH_8U, 1);
      cvSplit(tmp, image, image_cb, image_cr, NULL);
    }
    // if (color && !grid) rand=image->height*image->width/10;

    CvSeq *imageKeypoints = 0, *imageDescriptors = 0, *imageDescriptors2 = 0;
    CvSeq *imageDescriptors_cb = 0, *imageDescriptors2_cb = 0;
    CvSeq *imageDescriptors_cr = 0, *imageDescriptors2_cr = 0;
    int i,j,bytes;
    CvSURFParams params1 = cvSURFParams(thre, 0);
    CvSURFParams params2 = cvSURFParams(thre, 1);  // extended

    double tt = (double)cvGetTickCount();

    if (grid){
      CvSURFPoint *pt;
      imageKeypoints=cvCreateSeq(0,sizeof(CvSeq),sizeof(CvSURFPoint),storage);
       int startx = (int)((image->width % grid) + grid )/2 -1;
       int starty = (int)((image->height % grid) + grid )/2 -1;
       int x,y,*s=scales;
       for(;*s>0;s++){
         for(y=starty;y<image->height;y+=grid){
           for(x=startx;x<image->width;x+=grid){
  	     pt=(CvSURFPoint*)malloc(sizeof(CvSURFPoint));
             pt->pt.x=x;
             pt->pt.y=y;
	     pt->size=*s;
             pt->dir=0;
   	     cvSeqPush(imageKeypoints,pt);
//	printf("[%d] %d %d %d %.2f\n",i,(int)(pt->pt.x), (int)(pt->pt.y), pt->size, pt->dir);
	   }
	 }
       }
    }else if (rand){
      CvSURFPoint *pt;
      imageKeypoints=cvCreateSeq(0,sizeof(CvSeq),sizeof(CvSURFPoint),storage);
      double rd1,rd2,rd3;
      for(i=0;i<rand;i++){
	pt=(CvSURFPoint*)malloc(sizeof(CvSURFPoint));
        pt->pt.x=random_int(image->width);
        pt->pt.y=random_int(image->height);
        rd1=random()/((double)RAND_MAX+1);
        rd2=random()/((double)RAND_MAX+1);
        rd3=random()/((double)RAND_MAX+1);
        pt->size = 9+ (int)((rd1*rd1*rd1+rd2*rd2*rd2+rd3*rd3*rd3)*33.0);   /* 9 -- 64 */
        pt->dir=0;
	cvSeqPush(imageKeypoints,pt);
//	printf("[%d] %d %d %d %.2f\n",i,(int)(pt->pt.x), (int)(pt->pt.y), pt->size, pt->dir);
      }
    }else{
      cvExtractSURF( image, NULL, &imageKeypoints, NULL, storage, params1, 0);
    }

    if (ext>=1)
      cvExtractSURF( image, NULL, &imageKeypoints, &imageDescriptors2, storage, params2, 1);
    if (ext==0 || ext==2)
      cvExtractSURF( image, NULL, &imageKeypoints, &imageDescriptors, storage, params1, 1);

    if (color){
      if (ext>=1){
        cvExtractSURF( image_cb, NULL, &imageKeypoints, &imageDescriptors2_cb, storage, params2, 1);
        cvExtractSURF( image_cr, NULL, &imageKeypoints, &imageDescriptors2_cr, storage, params2, 1);
      }
      if (ext==0 || ext==2){
        cvExtractSURF( image_cb, NULL, &imageKeypoints, &imageDescriptors_cb, storage, params1, 1);
        cvExtractSURF( image_cr, NULL, &imageKeypoints, &imageDescriptors_cr, storage, params1, 1);
      }
    }

    fprintf(stderr,"Number of Keypoints: %d\n", imageKeypoints->total);
    tt = (double)cvGetTickCount() - tt;
    fprintf(stderr,"Extraction time = %g ms\n", tt/(cvGetTickFrequency()*1000.));

    //	printf("%d %d %d %.2f %.2f\n",(int)(r->pt.x), (int)(r->pt.y), r->size, r->dir, r->hessian);
    if (show){
      IplImage* object_color;
      cvNamedWindow("Image with SURF Points", 1);
      object_color = cvCreateImage(cvGetSize(image), 8, 3);
      cvCvtColor( image, object_color, CV_GRAY2BGR );
      for( i = 0; i < imageKeypoints->total; i++ ){
        CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, i );
        CvPoint center;
        int radius;
        center.x = cvRound(r->pt.x);
        center.y = cvRound(r->pt.y);
        radius = cvRound(r->size*1.2/9.*2);
        cvCircle( object_color, center, radius, colors, 1, 8, 0 );
      }
      cvShowImage( "Image with SURF Points", object_color );
      cvWaitKey(0);
      cvDestroyWindow("Image with SURF Points");
    }

   if (fout){
      strcpy(fout0,fout);
      if (ptr=strstr(fout0,".key")) { *ptr='\0'; }
      sprintf(fout1,"%s.key",fout0);
      fprintf(stderr,"out file: %s\n",fout1);
      // fprintf(stderr,"bin:%d ext:%d\n",bin,ext);
      if ((fp=fopen(fout1,"w"))==NULL) { 
         fprintf(stderr,"Error: cannot write %s !\n",fout1);
	 exit(1);
      }
   }else{
     fp=stdout;
     strcpy(fout0,argv[idx+1]);
     if (ptr=strstr(fout0,".")) { *ptr='\0'; }
   }
   if (bin) {  
      if (ext>=1) { 
  	if (color){
          sprintf(fout2,"%s.cdesc128",fout0); 
          fprintf(stderr,"binary out file (128*%d*3 bytes): %s\n",sizeof(float),fout2);
        }else{
          sprintf(fout2,"%s.desc128",fout0); 
          fprintf(stderr,"binary out file (128*%d bytes): %s\n",sizeof(float),fout2);
	}
        fbin=fopen(fout2,"wb"); 
        if (fbin==NULL) { 
	  fprintf(stderr,"Error: cannot write %s !\n",fout2);
	  exit(1);
	}
      }
      if (ext==0 || ext==2) { 
        if (color){
          sprintf(fout2,"%s.cdesc",fout0); 
          fprintf(stderr,"binary out file (64*%d*3 bytes): %s\n",sizeof(float),fout2);
        }else{
          sprintf(fout2,"%s.desc",fout0); 
          fprintf(stderr,"binary out file (64*%d bytes): %s\n",sizeof(float),fout2);
	}
	fbin2=fopen(fout2,"wb");  
        if (fbin2==NULL) { 
	  fprintf(stderr,"Error: cannot write %s !\n",fout2);
	  exit(1);
	}
      }
   }
   
    if (ext==0) bytes=64;
    else bytes=128;
   
    for( i = 0; i < imageKeypoints->total; i++ )  {
        CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, i );
	float *v,*v2,*v_cb,*v2_cb,*v_cr,*v2_cr;
        if (ext==2 || ext==0){ 
          v = (float*)cvGetSeqElem( imageDescriptors, i );
          if (color){
  	    v_cb = (float*)cvGetSeqElem( imageDescriptors_cb, i );
  	    v_cr = (float*)cvGetSeqElem( imageDescriptors_cr, i );
	  }
        }
        if (ext>=1){
	  v2 = (float*)cvGetSeqElem( imageDescriptors2, i );
          if (color){
  	    v2_cb = (float*)cvGetSeqElem( imageDescriptors2_cb, i );
  	    v2_cr = (float*)cvGetSeqElem( imageDescriptors2_cr, i );
	  }
        }
	if (bin) {
//	  fprintf(fp,"%d %d %d %.2f %.2f\n",(int)(r->pt.x), (int)(r->pt.y), r->size, r->dir, r->hessian);
	  fprintf(fp,"%d %d %d %.5f\n",(int)(r->pt.x), (int)(r->pt.y), r->size, r->dir);
	  if (ext==2 || ext==0) {
             fwrite(v,sizeof(float),64,fbin2);
             if (color){
	       fwrite(v_cb,sizeof(float),64,fbin2);
	       fwrite(v_cr,sizeof(float),64,fbin2);
	     }
          }
	  if (ext>=1){
	    fwrite(v2,sizeof(float),128,fbin);
             if (color){
	       fwrite(v2_cb,sizeof(float),128,fbin);
	       fwrite(v2_cr,sizeof(float),128,fbin);
	     }
          }
	}else{
//	  fprintf(fp,"%d %d %d %.2f %.2f",(int)(r->pt.x), (int)(r->pt.y), r->size, r->dir, r->hessian);
	  fprintf(fp,"%d %d %d %.5f",(int)(r->pt.x), (int)(r->pt.y), r->size, r->dir);
	  if (ext==0) for(j=0;j<bytes;j++) fprintf(fp," %.6f",v[j]);
	  else for(j=0;j<bytes;j++) fprintf(fp," %.6f",v2[j]);
          if (color){
	    if (ext==0) {
	      for(j=0;j<bytes;j++) fprintf(fp," %.6f",v_cb[j]);
	      for(j=0;j<bytes;j++) fprintf(fp," %.6f",v_cr[j]);
	    }else{ 
	      for(j=0;j<bytes;j++) fprintf(fp," %.6f",v2_cb[j]);
	      for(j=0;j<bytes;j++) fprintf(fp," %.6f",v2_cr[j]);
	    }
	  }
          fprintf(fp,"\n");
	}
      }
    if (fout) { 
      fclose(fp); 
      if (bin && fbin) fclose(fbin); 
      if (bin && fbin2) fclose(fbin2);
    }

    return 0;
}
