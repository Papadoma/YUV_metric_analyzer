#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "metrics.hpp"

using namespace std;
int main(int argc, char* argv[]){
	char* file1;
	char* file2;
	char* roi_c;
	int roi[4] = { 0,0,1,1 };
	Size frame_size(1,1);
	int no_frames = 1;

	Quality_Metrics metric_calc;
	VideoCapture cap_file1;
	VideoCapture cap_file2;

	cap_file1.set(CV_CAP_PROP_FPS ,60);
	cap_file2.set(CV_CAP_PROP_FPS ,60);

	enum metrics{
		PSNR,
		MSSIM,
		MS_SSIM,
		PSNR_HVS_M,
	};
	int metric;

	if(argc<3){
		cout<<"Usage is -i1 <file1> -i2 <file2> -c <psnr,ssim,etc>"<<endl;
		cout<<"Use -help for help."<<endl;
		cin.get();
		exit(0);
	}else{
		for(int i = 1; i < argc; i++) {
			if(string(argv[i])=="-i1"){
				file1 = argv[i + 1];
			}else if(string(argv[i])=="-i2"){
				file2 = argv[i + 1];
			}else if(string(argv[i])=="-c"){
				metric = atoi(argv[i + 1]);
			}else if(string(argv[i])=="-w"){
				frame_size.width = atoi(argv[i + 1]);
				cap_file1.set(CV_CAP_PROP_FRAME_WIDTH ,frame_size.width);
				cap_file2.set(CV_CAP_PROP_FRAME_WIDTH ,frame_size.width);
			}else if(string(argv[i])=="-h"){
				frame_size.height = atoi(argv[i + 1]);
				cap_file1.set(CV_CAP_PROP_FRAME_HEIGHT,frame_size.height);
				cap_file2.set(CV_CAP_PROP_FRAME_HEIGHT,frame_size.height);
			}else if(string(argv[i])=="-f"){
				no_frames = atoi(argv[i + 1]);
			}else if(string(argv[i])=="-r"){
				roi_c = argv[i + 1];
				size_t pos = 0;
				string s(roi_c),num_s;
				int idx = 0;
				while ((pos = s.find(",")) != string::npos) {
					num_s = s.substr(0, pos);
				    s.erase(0, pos + string(",").length());
				    roi[idx++] = atoi(num_s.c_str());
				}


			}else if(string(argv[i])=="-help"){
				cout<<"-i1 : input file 1"<<endl;
				cout<<"-i2 : input file 2"<<endl;
				cout<<"-w : width of frame"<<endl;
				cout<<"-h : height of frame"<<endl;
				cout<<"-r : region to calculate the metric for, x,y,width,height"<<endl;
				cout<<"-c : metric chosen, one of the following:"<<endl;
				cout<<"   0 : PSNR"<<endl;
				cout<<"   1 : MSSIM (average SSIM of frame)"<<endl;
				cout<<"   2 : MS-SSIM"<<endl;
				cout<<"   3 : PSNR-HVS-M"<<endl;
			}
		}
	}



	cap_file1.open((string(file1)));
	cap_file2.open((string(file2)));

	int idx_frame=0;
	while(cap_file1.isOpened() && cap_file2.isOpened() && idx_frame++<no_frames){
		Mat f1, f2;
		cap_file1>>f1;
		cap_file2>>f2;
		imshow("file 1",f1);
		cv::waitKey(0);
	}

	return 1;
}
