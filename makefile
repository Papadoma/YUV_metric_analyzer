all: YUV_metrics_analyzer

YUV_metrics_analyzer: main.o PSNR.o SSIM.o PSNRHVS.o VIFP.o MSSSIM.o Metric.o yuv.o
	g++ main.o Metric.o PSNR.o SSIM.o PSNRHVS.o VIFP.o MSSSIM.o yuv.o -o YUV_metrics_analyzer `pkg-config opencv --libs`

main.o: main.cpp
	g++ -c main.cpp 

yuv.o: yuv.cpp
	g++ -c yuv.cpp
	
PSNR.o: PSNR.cpp
	g++ -c PSNR.cpp

SSIM.o: SSIM.cpp
	g++ -c SSIM.cpp
	
PSNRHVS.o: PSNRHVS.cpp
	g++ -c PSNRHVS.cpp
	
VIFP.o: VIFP.cpp
	g++ -c VIFP.cpp
	
MSSSIM.o: MSSSIM.cpp
	g++ -c MSSSIM.cpp
	
Metric.o: Metric.cpp
	g++ -c Metric.cpp
	 	
clean:
	rm *o YUV_metrics_analyzer