#include <cv.h>
#include <highgui.h>
#include <ml.h>
#include <time.h>
#include <vector>
#include <list>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <stdio.h>
#include <iostream>
using namespace std;
int NUM=200;
int DIM=500;

int DATA_NUM=160;
int TEST_NUM=40;

vector< vector<double> > all_data(0, vector<double>(0));

int setBof(char *file){
	FILE *fp;
	char s[10000];	
	if ((fp = fopen(file, "r")) == NULL) {
		printf("file open error!!\n");
		return -1;
	}
	all_data.resize(NUM);
	for (int i=0; i<NUM; i++){
		all_data[i].resize(DIM); 
	}
	int n=0;
	while (fgets(s,10000, fp) != NULL) {
		string str=s;
		list<string> list_string;
		string delim (",");
		boost::split(list_string, str, boost::is_any_of(delim));
		int dim=0;
		BOOST_FOREACH(string s, list_string) {
			double h=atof(s.c_str());
			if(dim<DIM){
				all_data[n][dim]=h;
				dim++;
			}
        }
		n++;
		if(n==NUM)break;
	}
	fclose(fp);
	return 1;
}
int setVlad(char *file){
	DIM=16*64;
	FILE *fp;
	char s[10000];	
	if ((fp = fopen(file, "r")) == NULL) {
		printf("file open error!!\n");
		return -1;
	}
	all_data.resize(NUM);
	for (int i=0; i<NUM; i++){
		all_data[i].resize(DIM); 
	}
	int n=0,dim=0;
	while (fgets(s,10000, fp) != NULL) {
		if(s[0]=='/'){
			dim=0;
			n++;
			if(n==NUM)break;
			continue;	
		}
		string str=s;
		list<string> list_string;
		string delim (",");
		boost::split(list_string, str, boost::is_any_of(delim));
		BOOST_FOREACH(string s, list_string) {
			double h=atof(s.c_str());
			if(dim<DIM){
				all_data[n][dim]=h;
				dim++;
			}
        }
	}
	fclose(fp);
	return 1;
}
int setColor(char *file){
	DIM=64;
	return setBof(file);
}
int main (int argc, char **argv)
{
	int i, j, sv_num;
	CvSVM svm = CvSVM ();
	CvSVMParams param;
	CvTermCriteria criteria;
	CvRNG rng = cvRNG (time (NULL));
	//int set=0;
	if (argc<2){
		printf(" Usage:svm {data(0~6)} {set(0~4)}\n");
		printf("data:\n");
		printf(" 0: SURF-BOF\n");
		printf(" 1: SURFrandom-BOF\n");
		printf(" 2: SURFgrid-BOF\n");
		printf(" 3: SURF-VLAD\n");
		printf(" 4: SURFrandom-VLAD\n");
		printf(" 5: SURFgrid-VLAD\n");
		printf(" 6: color hist\n");
		exit(1);
	}
	//set=atoi(argv[2]);
	int dt=atoi(argv[1]);
	char f[7][20]={
		"BoF_e.txt",
		"BoF_r.txt",
		"BoF_g.txt",
		"VLAD_e.txt",
		"VLAD_r.txt",
		"VLAD_g.txt",
		"cl_hist.txt"
	};
	int res[200];
	if(dt<3){
		setBof(f[dt]);
	}else if(dt<6){
		setVlad(f[dt]);
	}else{
		dt=6;
		setColor(f[dt]);
	}
	cout << f[dt] << endl;
	for(int i=0;i<NUM;i++){
		if(i<100)res[i]=1;
		else res[i]=0;
	}
	for(int set=0;set<5;set++){
	int data[DATA_NUM],test[TEST_NUM];
	int dn=0,tn=0;
	for(int i=0;i<NUM;i++){
		if((i>=20*set && i<20*(set+1)) || (i>=20*set+100 && i<20*(set+1)+100)){
			test[tn]=i;
			tn++;
		}else{
			data[dn]=i;
			dn++;
		}
	}
	CvMat *data_mat = cvCreateMat(DATA_NUM, DIM, CV_32FC1 );
	CvMat *res_mat = cvCreateMat(DATA_NUM,1, CV_32SC1);
	
	criteria = cvTermCriteria (CV_TERMCRIT_EPS, 1000, FLT_EPSILON);
	param = CvSVMParams (CvSVM::C_SVC, CvSVM::RBF, 10.0, 8.0, 1.0, 10.0, 0.5, 0.1, NULL, criteria);

	for(int i=0;i<DATA_NUM;i++){
		int d=data[i];
		for(int j=0;j<DIM;j++){
			cvmSet(data_mat,i,j,all_data[d][j]);
		}
		res_mat->data.i[i]=res[d];
	}
	svm.train (data_mat, res_mat, NULL, NULL, param);

	int result[TEST_NUM];
	for(int i=0;i<TEST_NUM;i++){
		int t=test[i];
		CvMat *test = cvCreateMat(DIM,1, CV_32FC1 );
		for(int j=0;j<DIM;j++){
			test->data.fl[j]=all_data[t][j];
		}
		result[i]=svm.predict(test);
	}
	double tP=0,tN=0,fP=0,fN=0;
	for(int i=0;i<TEST_NUM;i++){
		int t=test[i];
		if(result[i]==res[t]){
			if(result[i]==1)tP++;
			else tN++;
		}else{
			if(result[i]==1)fP++;
			else fN++;
		}
	}
	double recall=tP/(tP+fN);
	double precision=tP/(tP+fP);
	double F=2*precision*recall/(precision+recall);
	double recognition=(tP+tN)/TEST_NUM;
	cout << "=====Number:" << set << "=====" << endl;
	cout << "テストデータ:" << endl;
	for(int i=0;i<TEST_NUM;i++){
		cout << test[i] << ",";
	}
	cout << endl;
	cout << "分類結果:" << endl;
	for(int i=0;i<TEST_NUM;i++){
		cout << result[i] << ",";
	}
	cout << endl;
	cout << "再現率:" << recall << endl;
	cout << "適合率:" << precision << endl;
	cout << "F値:" << F << endl;
	cout << "分類率:" << recognition << endl;
	}
	return 0;
}

