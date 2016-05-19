#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "metrics.hpp"
#include "yuv.hpp"

using namespace std;
int main(int argc, char* argv[]){
	char* file1;
	char* file2;
	char* roi_c;
	int roi[4] = { 0,0,1,1 };
	Size frame_size(1,1);
	int no_frames = 1;

	Quality_Metrics metric_calc;


	enum metrics{
		PSNR,
		MSSIM,
		MS_SSIM,
		PSNR_HVS_M,
	};
	int metric=0;

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
			}else if(string(argv[i])=="-h"){
				frame_size.height = atoi(argv[i + 1]);
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

	FILE *cap_file1 = fopen(file1, "rb");
	FILE *cap_file2 = fopen(file2, "rb");

	if (!cap_file1)
	{
		cout<<"error: unable to open file:"<<file1<<endl;
		return 1;
	}
	if (!cap_file2)
	{
		cout<<"error: unable to open file:"<<file2<<endl;
		return 1;
	}

	struct YUV_loader::YUV_Capture frame_handler1;
	struct YUV_loader::YUV_Capture frame_handler2;

	YUV_loader loader1(cap_file1, frame_size.width, frame_size.height, frame_handler1);
	YUV_loader loader2(cap_file2, frame_size.width, frame_size.height, frame_handler2);

	int idx_frame=0;
	while( idx_frame++<no_frames){
		loader1.YUV_read(frame_handler1);
		loader2.YUV_read(frame_handler2);

		switch(metric){
		case 0:
			cout<<metric_calc.getPSNR(frame_handler1.ycrcb,frame_handler2.ycrcb)<<endl;
			break;
		case 1:
			cout<<metric_calc.getMSSIM(frame_handler1.ycrcb,frame_handler2.ycrcb)<<endl;
			break;
		case 2:
			break;
		}
	}

	cv::waitKey(0);
	return 0;
}
