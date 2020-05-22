#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp> 


void StartCallback(int state, void * aux){
    *(bool*) aux = !*(bool*) aux;
}

void ResetCallback(int state, void * counter){
    *(int*) counter = 0;
}
int main(int argc, char * argv[])
{
    cv::VideoCapture camera(cv::CAP_V4L);

    if (!camera.isOpened()){
        std::cerr << "FAIL" << std::endl;
    }

    std::cout << "Got camera" << std::endl;
    auto fps = camera.get(cv::CAP_PROP_FPS);
    auto frame_h = static_cast<int>(camera.get(cv::CAP_PROP_FRAME_HEIGHT));
    auto frame_w = static_cast<int>(camera.get(cv::CAP_PROP_FRAME_WIDTH));
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "dims: " << frame_h << " " << frame_w << std::endl;

    // window
    std::string wnd_main = "camera";
    std::string wnd_crop = "crop";
    cv::namedWindow(wnd_main);
    cv::namedWindow(wnd_crop);
    
    int crop_x = 0, crop_y = 0;
    int crop_w = 100;
    cv::createTrackbar("crop x", wnd_main, &crop_x, frame_w);
    cv::createTrackbar("crop y", wnd_main, &crop_y, frame_h);
    cv::createTrackbar("crop w", wnd_main, &crop_w, std::min(frame_h, frame_w));

    cv::Mat frame;
    double count = 0.0;
    auto clock = std::chrono::steady_clock();
    auto t0 = clock.now();
    double avg_spf = 0.0;

    bool record_stats = false;
    int counter = 0;
    cv::createButton("start", StartCallback, (void*) &record_stats);
    cv::createButton("reset", ResetCallback, (void*)&counter);
    // std::vector<float> stats;
    // stats.reserve(1000);
    bool state_on = true;
    while (camera.read(frame)){
        auto t1 = clock.now();
        std::chrono::duration<double> diff = t1 - t0;
        t0 = t1;
        count += 1.0;
        avg_spf = (diff.count()  + avg_spf * (count - 1.0)) / count;

        cv::Rect roi(std::min(crop_x, frame_w - crop_w), std::min(crop_y, frame_h - crop_w), crop_w, crop_w);
        cv::Mat crop;
        frame(roi).copyTo(crop);
        if (record_stats){
            cv::Mat crop_gray;
            cv::cvtColor(crop, crop_gray, cv::COLOR_BGR2GRAY);
            auto v = cv::mean(crop_gray / 255.0);
            
            if (state_on){
                if (v[0] <= 0.96){
                    state_on = false;
                    ++counter;
                }
            }
            else {
                if (v[0] >= 0.98){
                    state_on = true;
                    ++counter;
                }
            }

            // stats.push_back(v[0]);
            cv::addText(
                frame, std::to_string(0.5 * counter), cv::Point(20, 20), 
                cv::fontQt("Ubuntu Mono", 25, cv::Scalar(0))
            );
        }
        cv::imshow(wnd_main, frame);
        cv::imshow(wnd_crop, crop);
        char c = (char) cv::waitKey(10);
        if (c == 27) break; // Esc
    }
    // bool first = true;
    // for (auto v: stats){
    //     if (first){
    //         std::cout << v;
    //         first = false;
    //     }
    //     else {
    //         std::cout << ", " << v;
    //     }
    // }
}