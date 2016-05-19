#include <opencv2/opencv.hpp>
#include <stdio.h>

#include "yuv.hpp"

#include "MSSSIM.hpp"
#include "PSNR.hpp"
#include "PSNRHVS.hpp"
#include "SSIM.hpp"
#include "VIFP.hpp"

using namespace std;
int main(int argc, char* argv[]){
	char* file1;
	char* file2;
	char* roi_c;
	int roi_area[4] = { 0,0,1,1 };
	cv::Size frame_size(1,1);
	int no_frames = 1;


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
					roi_area[idx++] = atoi(num_s.c_str());
				}
			}else if(string(argv[i])=="-help"){
				cout<<"-i1 : input file 1"<<endl;
				cout<<"-i2 : input file 2"<<endl;
				cout<<"-w : width of frame"<<endl;
				cout<<"-h : height of frame"<<endl;
				cout<<"-r : region to calculate the metric for, x,y,width,height"<<endl;
				cout<<"Metrics: PSNR,SSIM,PSNR-HVSM,VIFP,MS-SSIM"<<endl;

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
	cv::Mat result(no_frames,5,CV_32FC1);

	do{
		loader1.YUV_read(frame_handler1);
		loader2.YUV_read(frame_handler2);

		cv::Mat roi1, roi2;
		frame_handler1.y(cv::Rect(roi_area[0], roi_area[1], roi_area[2], roi_area[3])).convertTo(roi1,CV_32F);
		frame_handler2.y(cv::Rect(roi_area[0], roi_area[1], roi_area[2], roi_area[3])).convertTo(roi2,CV_32F);

		PSNR psnr_metric(roi1.rows,roi1.cols);
		SSIM ssim_metric(roi1.rows,roi1.cols);
		PSNRHVS psnrhvsm_metric(roi1.rows,roi1.cols);
		MSSSIM msssim_metric(roi1.rows,roi1.cols);
		VIFP vifp_metric(roi1.rows,roi1.cols);

		result.at<float>(idx_frame,0) = psnr_metric.compute(roi1, roi2);
		result.at<float>(idx_frame,1) = ssim_metric.compute(roi1, roi2);
		result.at<float>(idx_frame,2) = psnrhvsm_metric.compute(roi1, roi2);
		result.at<float>(idx_frame,3) = vifp_metric.compute(roi1, roi2);
		result.at<float>(idx_frame,4) = 0;//msssim_metric.compute(roi1, roi2);		//something wrong with this one

		cout<<"Frame"<<idx_frame<<":"<<
				result.at<float>(idx_frame,0)<<","<<
				result.at<float>(idx_frame,1)<<","<<
				result.at<float>(idx_frame,2)<<","<<
				result.at<float>(idx_frame,3)<<","<<
				result.at<float>(idx_frame,4)<<
				endl;

	}while( ++idx_frame<no_frames);

	//cout<<mean(result)[0]<<endl;
	return 0;
}
