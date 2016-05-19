all: YUV_metrics_analyzer

YUV_metrics_analyzer: main.o metrics.o
	g++ main.o metrics.o -o YUV_metrics_analyzer `pkg-config opencv --libs`

main.o: main.cpp
	g++ -c main.cpp 

metrics.o: metrics.cpp
	g++ -c metrics.cpp 


clean:
	rm *o YUV_metrics_analyzer