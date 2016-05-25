#!/bin/sh
#This is a shell script

#Initiate encoding of specific sequence - All done parallel
for sequence in /media/sf_VirtualBoxShared/randomaccess/bricksbushesstatic/*.yuv						#Iterate through every .yuv file in the SourceSequences folder
do
	 sequence=${sequence##*/}							#This just gives us the file's name without the path
	 filename=${sequence%.yuv}							#This just gives us the file's name without the .yuv

	./YUV_metrics_analyzer -i1 '/media/sf_VirtualBoxShared/BricksBushesStatic60fps.yuv' -i2 '/media/sf_VirtualBoxShared/randomaccess/bricksbushesstatic/'${sequence} -w 1920 -h 1080 -f 300 -r 0,0,64,64 -gui -multiple | tee './logs/'${sequence}'.txt'

done

