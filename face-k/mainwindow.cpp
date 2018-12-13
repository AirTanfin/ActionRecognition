/// System and Kinect includes
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "inc/Kinect.h"
#include <vector>

/// OpenCV includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QTimer>

/// Namespace
using namespace cv;
using namespace std;

/// Global variables
static const int colorWidth = 1920;
static const int colorHeight = 1080;
static const int depthWidth = 512;
static const int depthHeight = 424;

/// Global variables for the Kinect Sensor
IKinectSensor* kinectSensor;
IColorFrameReader* colorFrameReader;
IDepthFrameReader* depthFrameReader;
RGBQUAD* colorRGBX;
RGBQUAD* depthRGBX;
QString filePath;
QString destDirectory = QDir::currentPath();

/// Method to release an interface (frames)
template<class Interface>
inline void SafeRelease(Interface *& interfaceToRelease) {
    if(interfaceToRelease != NULL) {
        interfaceToRelease->Release();
        interfaceToRelease = NULL;
    }
}

/// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    kinectSensor = NULL;
    colorFrameReader = NULL;
    depthFrameReader = NULL;
    colorRGBX = new RGBQUAD[colorWidth * colorHeight]; // Heap for the color data
    depthRGBX = new RGBQUAD[depthWidth * depthHeight]; // Heap for the depth data

    init(); // Initialize and test the Kinect sensor

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateKinectData()));
    timer->start(33); // Start the timer after 33ms
}

/// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

/// Initialize the Kinect sensor and the color frame reader
void MainWindow::init() {
    HRESULT hr;
    hr = GetDefaultKinectSensor(&kinectSensor); // Get the Kinect sensor
    if(FAILED(hr)) return;

    if(kinectSensor) {
        IColorFrameSource* colorFrameSource = NULL;
        IDepthFrameSource* depthFrameSource = NULL;
        hr = kinectSensor->Open(); // Open the sensor

        // Check color
        if(SUCCEEDED(hr)) { hr = kinectSensor->get_ColorFrameSource(&colorFrameSource); }
        if(SUCCEEDED(hr)) { hr = colorFrameSource->OpenReader(&colorFrameReader); }
        SafeRelease(colorFrameSource);

        // Check depth
        if(SUCCEEDED(hr)) { hr = kinectSensor->get_DepthFrameSource(&depthFrameSource); }
        if(SUCCEEDED(hr)) { hr = depthFrameSource->OpenReader(&depthFrameReader); }
        SafeRelease(depthFrameSource);
    }

    if(!kinectSensor || FAILED(hr)) return;
}

/// Update the Kinect data at each frame and update the preview
void MainWindow::updateKinectData() {

    if(!colorFrameReader) return;
    if(!depthFrameReader) return;

    IColorFrame* colorFrame = NULL;
    IDepthFrame* depthFrame = NULL;

    // Color
    HRESULT hr = colorFrameReader->AcquireLatestFrame(&colorFrame); // Get the last color frame from the sensor
    if(SUCCEEDED(hr)) {
        ColorImageFormat imageFormat = ColorImageFormat_None;
        UINT bufferSize = 0;
        RGBQUAD *colorBuffer = NULL;

        // Color frame
        if(SUCCEEDED(hr))
            hr = colorFrame->get_RawColorImageFormat(&imageFormat);

        if(SUCCEEDED(hr)) {
            if(imageFormat == ColorImageFormat_Bgra)
                hr = colorFrame->AccessRawUnderlyingBuffer(&bufferSize, reinterpret_cast<BYTE**>(&colorBuffer));
            else if (colorRGBX) {
                colorBuffer = colorRGBX;
                bufferSize = colorWidth * colorHeight * sizeof(RGBQUAD);
                hr = colorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(colorBuffer), ColorImageFormat_Bgra);
            }
            else
                hr = E_FAIL;
        }

        // Display what the Kinect see
        if(SUCCEEDED(hr)) {
            BYTE* bytepImage = reinterpret_cast<BYTE*>(colorBuffer);
            Mat out = Mat(colorHeight, colorWidth, CV_8UC4, reinterpret_cast<void*>(bytepImage)); // Transform the color frame in OpenCV matrix

            // Display the color frame on the interface
            QImage color(out.data, out.cols, out.rows, out.step, QImage::Format_RGB32);
            this->ui->color->setPixmap(QPixmap::fromImage(color).scaledToWidth(this->ui->color->width()));

            out.release(); // Release the matrix (free the memory)
        }
    }
    SafeRelease(colorFrame); // Release the color frame (free the memory)

    // Depth
    hr = depthFrameReader->AcquireLatestFrame(&depthFrame); // Get the last color frame from the sensor
    if(SUCCEEDED(hr)) {
        UINT depthSize = depthWidth * depthHeight;
        UINT16 depthData[depthWidth * depthHeight];
        hr = depthFrame->CopyFrameDataToArray(depthSize, depthData);

        if(SUCCEEDED(hr)){
            // Transform the depth frame in OpenCV matrix
            Mat* depthOut = new Mat(depthHeight, depthWidth, CV_8U);
            for(unsigned int i = 0; i < depthSize; i++) {
                uchar depthDat = depthData[i];
                depthOut->at<byte>(i) = depthDat;
            }

            // Display the depth frame on the interface
            QImage depth(depthOut->data, depthOut->cols, depthOut->rows, depthOut->step, QImage::Format_Grayscale8);
            this->ui->depth->setPixmap(QPixmap::fromImage(depth).scaledToWidth(this->ui->depth->width()));

            depthOut->release(); // Release the matrix (free the memory)
        }
    }
    SafeRelease(depthFrame); // Release the depth frame (free the memory)
}
