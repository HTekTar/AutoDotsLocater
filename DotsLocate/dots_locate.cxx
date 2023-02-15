#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <opencv4/opencv2/opencv.hpp>
#include <utility>

using namespace cv;

void show_image(Mat &img, const char* name){
  namedWindow(name, WINDOW_AUTOSIZE );
  imshow(name, img);
  waitKey(0);
}

void write_file(std::vector<Point2f> posVec, char sep = ','){
  std::ofstream csvOut; 
  csvOut.open("output.csv");
  csvOut << "x"<<sep<<"y\n";
  for(auto p: posVec){
    csvOut<< p.x<<sep<<p.y<<"\n";
  }
}

void save_image(Mat &img){
  imwrite("output.jpg", img);
}

std::vector<Point2f> getSubPixCentroids(Mat &gray, std::vector<Point2f> &centroids, float d = 0.25f){
  Mat inv;
  bitwise_not(gray, inv);
  std::vector<Point2f> subPixCentroid(centroids.size());
  for( int i = 0; i<centroids.size(); i++)
  {
    float x = centroids[i].x;
    float y = centroids[i].y;
    int I = 0;
    float x_c, y_c;
    Mat patch;
    int I_c;

    int n = static_cast<int>((1 - d)/d);
    x_c = centroids[i].x - 0.5 + (d/2);
    for(int j=0; j<n; j++){
      y_c = centroids[i].y - 0.5 + (d/2);
      for(int k=0; k<n; k++){
        x_c += j*d;
        y_c += k*d;
        getRectSubPix(inv, cv::Size(1, 1), Point2f(x_c, y_c), patch);
        I_c = patch.at<uchar>(Point2f(0, 0));
        if(I_c > I){
          x = x_c;
          y = y_c;
          I = I_c;
        }
      }
    }
    subPixCentroid[i] = Point2f(x, y);
  }
  return subPixCentroid;
}


std::vector<Point2f> getHolesCentroids(Mat &src, float d = 0.25f, int w = 4){
  Mat gray, blurred, equalized, thresh, cannyOut;

  cvtColor( src, gray, COLOR_BGR2GRAY );
  // GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
  equalizeHist(gray, equalized);
  GaussianBlur(equalized, blurred, cv::Size(5, 5), 0);
  threshold(gray, thresh, 100, 255, THRESH_BINARY);
  GaussianBlur(thresh, blurred, cv::Size(5, 5), 0);
  // medianBlur(equalized, blurred, 5);
  
  Canny( blurred, cannyOut, 30, 40, 3 );


  std::vector<std::vector<Point> > contours;
  std::vector<Vec4i> hierarchy;

  findContours( cannyOut, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  
  std::vector<std::vector<Point>> circles;
  std::vector<Vec4i> circleHierarchy;
  for(int i=0; i< contours.size(); i++){
    double a = contourArea(contours[i], true);
    double p = arcLength(contours[i], true);
    double circularity = 4*CV_PI*a / (p*p);
    
    if(a > 0.0 && circularity > 0.2 ){
      circles.push_back(contours[i]);
      circleHierarchy.push_back(hierarchy[i]);
      // std::cout<<a<<":"<<p<<":"<<circularity<<std::endl;
    }
  }
  std::cout<<"dots count: "<<circles.size()<<std::endl;
  std::vector<Moments> moments_(circles.size());
  std::vector<Point2f> centroids(circles.size());
  for( int i = 0; i<moments_.size(); i++){ 
    moments_[i] = moments( circles[i], false ); 
    centroids[i] = Point( moments_[i].m10/moments_[i].m00 , moments_[i].m01/moments_[i].m00 );
  }

  return getSubPixCentroids(blurred, centroids);
}

std::vector<std::pair<float, float>> locateCentroids(const char* image, float d, int w){
  Mat src = imread(image);
  auto centroids = getHolesCentroids(src);
  std::vector<std::pair<float, float>> result(centroids.size());
  for(int i=0; i<centroids.size(); i++){
    result[i] = std::pair<float, float>(centroids[i].x, centroids[i].y);
  }
  return result;
}

Mat getTestImage(Size sz, std::vector<Vec3i> posVec){
  Mat canv(sz, CV_8UC3, Scalar(150,150,150));

  for( int i = 0; i<posVec.size(); i++ )
  {
    Scalar color = Scalar(167,151,0);
    circle( canv, Point2f(posVec[i][0], posVec[i][1]), posVec[i][2], Scalar(0, 0, 0), -1);
  }
  // cv::Mat noise(canv.size(),canv.type());
  // float m = (1,1,3);
  // float sigma = (1,5,5);
  // cv::randn(noise, m, sigma);
  // canv += noise;

  return canv;
}

void run_test(int side, int numDots, int radius = 1){
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(1,side); 

  std::vector<Vec3i> posVec(numDots);
  for(int i=0; i<numDots; i++){
    int x = dist6(rng);
    int y = dist6(rng);

    posVec[i] = Vec3i(x, y, radius);
  }

  Mat testIm = getTestImage(Size(side, side), posVec);
  auto centroids = getHolesCentroids(testIm);
  Mat drawing(testIm.size(), CV_8UC3, Scalar(255,255,255));
  for( int i = 0; i<centroids.size(); i++ )
  {
    drawMarker(testIm, centroids[i], Scalar(0,0,255));
    circle(testIm, centroids[i], 5, Scalar(255, 0, 0));
  }
  write_file(centroids);
  save_image(testIm);
  // show_image(testIm, "test");
}

void run_file(const char* imFile, float d, int w){
  Mat src = imread(imFile);
  auto centroids = getHolesCentroids(src, d, w);

  Mat drawing(src.size(), CV_8UC3, Scalar(255,255,255));
  for( int i = 0; i<centroids.size(); i++ )
  {
    Scalar color = Scalar(167,151,0); // B G R values
    drawMarker(src, centroids[i], color);
  }
  // show_image(src, "Holes");
  write_file(centroids);
  save_image( src);
}


