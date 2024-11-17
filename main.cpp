#include <iostream>
#include <opencv2/opencv.hpp>

int main(void)
{
    // VideoCapture类用于读取视频流
    //  原型：class cv::VideoCapture : public VideoCaptureBase
    //  VideoCapture(const String& filename)：通过文件名打开视频文件

    cv::VideoCapture cap("../vd/video.mp4");
    if (cap.isOpened() == false)
    {
        std::cout << "Cannot open video." << std::endl;
        return -1;
    }

    // // 青色BGR范围（不好用）
    // cv::Scalar lowerBound(180, 150, 0);
    // cv::Scalar upperBound(255, 255, 150);

    // 定义青色的 HSV 范围
    cv::Scalar lowerBound(80, 150, 50);   // 青色的下限值 (H, S, V)
    cv::Scalar upperBound(100, 255, 255); // 青色的上限值 (H, S, V)

    cv::Mat frame, hsvFrame, mask, result; // 原始帧 掩膜 结果

    while (1)
    {
        // getTickCount：返回int64类型的当前时间，单位为毫秒
        // 原型：int64 cv::getTickCount(void)
        int64 start = cv::getTickCount();
        cap.read(frame);
        if (frame.empty() == true)
        {
            std::cout << "Video ends." << std::endl;
            break;
        }

        // 将 BGR 图像转换为 HSV 图像
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // inRange函数用于在HSV图像中提取出指定颜色范围的图像
        // 原型：void cv::inRange(InputArray src, InputArray lowerb, InputArray upperb, OutputArray dst)
        cv::inRange(hsvFrame, lowerBound, upperBound, mask); // 绘制mask掩膜

        // 开运算:先腐蚀后膨胀
        // getStructuringElement：用于创建结构单位（矩形：cv::MORPH_RECT，圆形：cv::MORPH_ELLIPSE，椭圆：cv::MORPH_ELLIPSE）
        // 原型：Mat cv::getStructuringElement(int shape, Size ksize, Point anchor = Point(-1,-1))
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        // morphologyEx函数用于形态学处理，包括开运算、闭运算、形态学梯度、顶帽、黑帽等。
        // 原型：void cv::morphologyEx(InputArray src, OutputArray dst, int op, InputArray kernel, Point anchor = Point(-1,-1),   int iterations = 1, int borderType = BORDER_CONSTANT, const Scalar& borderValue = morphologyDefaultBorderValue())
        // int op表示操作类型，可以为MORPH_OPEN：开运算，表示先腐蚀后膨胀。
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

        // 轮廓检测
        // contours是vector<vector<Point>>类型，其中每个vector<Point>表示一个轮廓，Point表示轮廓上的一个点。
        std::vector<std::vector<cv::Point>> contours; // contours储存所有的轮廓
        // findContours函数用于查找图像中的轮廓，并将它们存储在contours中。
        // 原型：void cv::findContours(InputOutputArray image, OutputArrayOfArrays contours, int mode, int method, Point offset = Point())
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 绘制矩形框
        for (const auto &contour : contours)
        {
            // 轮廓的面积
            double area = cv::contourArea(contour);
            if (area > 1000)
            {
                cv::Point2f center;
                float radius;
                // minEnclosingCircle用于计算给定轮廓的最小外接圆
                // 原型：void cv::minEnclosingCircle(InputArray points, Point2f& center, float& radius);
                cv::minEnclosingCircle(contour, center, radius);

                // arcLength：用于计算轮廓或曲线周长的函数。它的主要用法是通过轮廓点计算其周长或长度。
                // 原型：double cv::arcLength(InputArray curve, bool closed);
                double length = cv::arcLength(contour, true);

                // 计算圆形度
                float circularity = (4 * CV_PI * area) / (length * length);

                if (circularity > 0.60 && radius > 10) // 淦，0.6也不好，怎么都不好
                {
                    // Rect类型表示矩形的位置和大小
                    // 成员变量：x, y, width, height   构造函数:Rect(int _x, int _y, int _width, int _height);
                    // boundingRect函数用于计算矩形的边界框,返回Rect类型
                    cv::Rect boundingRect = cv::boundingRect(contour);

                    // rectangle函数用于在图像上绘制矩形
                    // 原型：void cv::rectangle(InputOutputArray img, const Rect& rect, const Scalar& color, int thickness = 1, int lineType = 8, int shift = 0);
                    cv::rectangle(frame, boundingRect, cv::Scalar(0, 0, 255), 2);

                    int64 end = cv::getTickCount();                       // 结束时的时钟周期数
                    double time = (end - start) / cv::getTickFrequency(); // 周期数/频率=时间（秒）
                    std::string timeText = "Time:" + std::to_string(time) + "s";
                    // putText用于在图像上绘制文本
                    // 原型：void cv::putText(InputOutputArray img, const String& text, Point org, int fontFace, double fontScale, const Scalar& color, int thickness = 1, int lineType = 8, bool bottomLeftOrigin = false)
                    cv::putText(frame, timeText, cv::Point(boundingRect.x, boundingRect.y - 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
                }
            }
        }

        cv::imshow("frame", frame);
        cv::imshow("mask", mask);
        char key = cv::waitKey(30);
        if (key == 'q')
        {
            std::cout << "Quit." << std::endl;
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}