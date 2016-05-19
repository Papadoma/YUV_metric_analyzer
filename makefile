all: YUV_metrics_analyzer

YUV_metrics_analyzer: main.o metrics.o yuv.o
	g++ main.o metrics.o yuv.o -o YUV_metrics_analyzer `pkg-config opencv --libs`

main.o: main.cpp
	g++ -c main.cpp 

metrics.o: metrics.cpp
	g++ -c metrics.cpp 

yuv.o: yuv.cpp
	g++ -c yuv.cpp
	
clean:
	rm *o YUV_metrics_analyzer