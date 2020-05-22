all: counter
counter:
	mkdir -p bin && \
		g++ main.cpp -o bin/counter \
		-I/usr/include/opencv4/ \
		-lopencv_core \
		-lopencv_highgui \
		-lopencv_imgproc \
		-lopencv_videoio
debug:
	mkdir -p bin && \
	g++ main.cpp -g -o bin/counter \
	-I/usr/include/opencv4/ \
	-lopencv_core \
	-lopencv_highgui \
	-lopencv_imgproc \
	-lopencv_videoio