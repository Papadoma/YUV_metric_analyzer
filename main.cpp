/**
 * YUV metric analyzer
 * Author: Miltiadis Alexios Papadopoulos
 * Year: 2016
 * Parts of the software come from the VQMT software.
 */

#include <string>
#include <sstream>

namespace patch
{
template < typename T > std::string to_string( const T& n )
{
	std::ostringstream stm ;
	stm << n ;
	return stm.str() ;
}
}


#include <opencv2/opencv.hpp>
#include <stdio.h>

#include "yuv.hpp"

#include "MSSSIM.hpp"
#include "PSNR.hpp"
#include "PSNRHVS.hpp"
#include "SSIM.hpp"
#include "VIFP.hpp"

using namespace std;

//Global variable - Yeah, I know Im not supposed to do this!
char* file1;
char* file2;
cv::Rect mouse_selection(0,0,0,0);
int scale = 1;
bool mouse_click = false;
cv::Mat mask;
cv::Rect safe_area;



void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	int CTU_size = 64/scale;
	if  ( event == cv::EVENT_LBUTTONDOWN )
	{
		mouse_selection = cv::Rect(0,0,0,0);
		mouse_click = true;
		mouse_selection.x = round(x/CTU_size)*CTU_size;
		mouse_selection.y = round(y/CTU_size)*CTU_size;
		mask.setTo(cv::Scalar(0,0,0));

	}
	else if  ( event == cv::EVENT_LBUTTONUP )
	{
		if((x - mouse_selection.x)>0 && (y - mouse_selection.y)>0 && mouse_selection.x<mask.cols && mouse_selection.y<mask.rows){
			mask.setTo(cv::Scalar(0,0,0));
			mouse_selection.width = (int)round((float)(max(x - mouse_selection.x,CTU_size))/CTU_size)*CTU_size;
			mouse_selection.height = (int)round((float)(max(y - mouse_selection.y,CTU_size))/CTU_size)*CTU_size;
			mouse_selection = mouse_selection & safe_area;
			mask(mouse_selection) = cv::Scalar(0,0,255);


			mouse_click = false;
			mouse_selection.x *= scale;
			mouse_selection.y *= scale;
			mouse_selection.width *= scale;
			mouse_selection.height *= scale;

		}
	}
	else if  ( event == cv::EVENT_MOUSEMOVE && mouse_click)
	{
		if((x - mouse_selection.x)>0 && (y - mouse_selection.y)>0 && mouse_selection.x<mask.cols && mouse_selection.y<mask.rows){
			mask.setTo(cv::Scalar(0,0,0));

			//with rounding

			mouse_selection.width =  (int)round((float)(max(x - mouse_selection.x,CTU_size))/CTU_size)*CTU_size;
			mouse_selection.height = (int)round((float)(max(y - mouse_selection.y,CTU_size))/CTU_size)*CTU_size;
			mouse_selection = mouse_selection & safe_area;
			//cout<<mouse_selection<<endl;
			mask(mouse_selection) += cv::Scalar(0,0,255);


			//without rounding

			mouse_selection.width = (x - mouse_selection.x);
			mouse_selection.height = (y - mouse_selection.y);
			mouse_selection = mouse_selection & safe_area;
			//cout<<mouse_selection<<endl;
			mask(mouse_selection) += cv::Scalar(255,0,0);

		}
	}
}

void mouse_select(cv::Mat background, cv::Rect &selection){

	//Overlay grid
	cv::Mat grid(background.size(), CV_8UC3,cv::Scalar(0,0,0));
	for (int col=0;col<background.cols;col+=64){	//verticals
		line(grid, cv::Point(col,0), cv::Point(col,background.rows), cv::Scalar(255,255,255), 1, 8);
	}
	for (int row=0;row<background.rows;row+=64){	//horizontals
		line(grid, cv::Point(0,row), cv::Point(background.cols,row), cv::Scalar(255,255,255), 1, 8);
	}
	addWeighted(background, 1, grid, -0.5, 0.0, background);

	//Resize for fitting purposes
	if(background.cols>1600){
		scale = 2;
		resize(background, background, cv::Size(),0.5,0.5);
	}
	safe_area = cv::Rect(0,0,background.cols,background.rows);

	cv::namedWindow(string(file2)+" --- Select Area", 1);
	cv::setMouseCallback(string(file2)+" --- Select Area", CallBackFunc, NULL);
	mask =cv::Mat(background.size(),CV_8UC3,cv::Scalar(0,0,0));

	int key;
	do{
		cv::imshow(string(file2)+" --- Select Area", background + mask);
		key = cv::waitKey(1);
		//cout<<key<<endl;
	}while(key!=1048586);	//press enter
	if(mouse_selection.width>=64 && mouse_selection.height>=64)selection = mouse_selection;
	std::cout<<"Area selected:"<<selection<<endl;
}

int main(int argc, char* argv[]){

	char* roi_c;
	int roi_area[4] = { 0,0,64,64};
	cv::Size frame_size(1,1);
	int no_frames = 1;
	bool GUI = false;

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
				roi_area[idx] = atoi(s.c_str());
			}else if(string(argv[i])=="-gui"){
				GUI = true;
			}else if(string(argv[i])=="-help" || string(argv[i])=="-h"){
				cout<<"-i1 : original sequence"<<endl;
				cout<<"-i2 : distorted sequence"<<endl;
				cout<<"-w : width of frame"<<endl;
				cout<<"-h : height of frame"<<endl;
				cout<<"-r : region to calculate the metric for, entered as: x,y,width,height"<<endl;
				cout<<"-f : number of frames"<<endl;
				cout<<"-gui : enables gui. In this case, -r is not used."<<endl;
				cout<<"Metrics: PSNR,SSIM,PSNR-HVSM,VIFP,MS-SSIM"<<endl;
				cout<<"PSNR-HVSM & VIFP need multiple of 8 pixels regions, MS-SSIM multiple of 16."<<endl;
				cout<<"The levels of MS-SSIM go as 1:16, 2:32, 3:64, 4:128 and 5 for 256 and above."<<endl;
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

	cv::Rect selection(roi_area[0], roi_area[1], roi_area[2], roi_area[3]);
	do{
		loader1.YUV_read(frame_handler1);
		loader2.YUV_read(frame_handler2);

		cv::Mat roi1, roi2;
		if(GUI){
			cv::Mat background;
			cvtColor(frame_handler1.ycrcb,background,CV_YCrCb2BGR);
			mouse_select(background,selection);
			GUI = false;
		}

		//Small debugging window
		cv::Mat sample;
		cvtColor(frame_handler1.ycrcb(selection),sample,CV_YCrCb2BGR);
		string surtitle;
		surtitle = patch::to_string(selection.width)+"x"+patch::to_string(selection.height)+" from ("+patch::to_string(selection.x)+","+patch::to_string(selection.y)+")";
		imshow("Area selected: "+surtitle,sample);
		cv::waitKey(1);

		frame_handler1.y(selection).convertTo(roi1,CV_32F);
		frame_handler2.y(selection).convertTo(roi2,CV_32F);

		PSNR psnr_metric(roi1.rows,roi1.cols);
		SSIM ssim_metric(roi1.rows,roi1.cols);
		PSNRHVS psnrhvsm_metric(roi1.rows,roi1.cols);
		MSSSIM msssim_metric(roi1.rows,roi1.cols);
		VIFP vifp_metric(roi1.rows,roi1.cols);

		result.at<float>(idx_frame,0) = psnr_metric.compute(roi1, roi2);
		result.at<float>(idx_frame,1) = ssim_metric.compute(roi1, roi2);
		if(roi1.rows%8==0 && roi1.cols%8==0)result.at<float>(idx_frame,2) = psnrhvsm_metric.compute(roi1, roi2);else result.at<float>(idx_frame,2) = 0;
		if(roi1.rows%8==0 && roi1.cols%8==0)result.at<float>(idx_frame,3) = vifp_metric.compute(roi1, roi2);else result.at<float>(idx_frame,3) = 0;
		if(roi1.rows%16==0 && roi1.cols%16==0)result.at<float>(idx_frame,4) = msssim_metric.compute(roi1, roi2); else result.at<float>(idx_frame,4) = 0;

		cout<<"Frame"<<idx_frame<<":"<<
				result.at<float>(idx_frame,0)<<","<<
				result.at<float>(idx_frame,1)<<","<<
				result.at<float>(idx_frame,2)<<","<<
				result.at<float>(idx_frame,3)<<","<<
				result.at<float>(idx_frame,4)<<
				endl;

	}while( ++idx_frame<no_frames);
	cv::Mat row_mean;
	reduce(result,row_mean, 0, CV_REDUCE_AVG);
	cout<<"Average:"<<row_mean<<endl;
	cv::waitKey(0);
	return 0;
}


