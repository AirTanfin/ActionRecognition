/// System and Kinect includes
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "inc/Kinect.h"
#include <vector>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>


/// OpenCV includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtWidgets>

/// Namespace
using namespace cv;
using namespace std;
using namespace QtCharts;

/// Global variables
static const int colorWidth = 1920;
static const int colorHeight = 1080;


/// Global variables for the Kinect Sensor
IKinectSensor* kinectSensor;
IColorFrameReader* colorFrameReader;
IBodyFrameReader* bodyFrameReader;
RGBQUAD* colorRGBX;
RGBQUAD* depthRGBX;
QString filePath;
QString destDirectory = QDir::currentPath();
boolean tracked;
Joint joints[JointType_Count];

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
    bodyFrameReader = NULL;
    colorRGBX = new RGBQUAD[colorWidth * colorHeight]; // Heap for the color data

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
        IBodyFrameSource* bodyFrameSource = NULL;
        hr = kinectSensor->Open(); // Open the sensor

        // Check color
        if(SUCCEEDED(hr)) { hr = kinectSensor->get_ColorFrameSource(&colorFrameSource); }
        if(SUCCEEDED(hr)) { hr = colorFrameSource->OpenReader(&colorFrameReader); }
        SafeRelease(colorFrameSource);

        //Check body
        if(SUCCEEDED(hr)) { hr = kinectSensor->get_BodyFrameSource(&bodyFrameSource); }
        if(SUCCEEDED(hr)) { hr = bodyFrameSource->OpenReader(&bodyFrameReader); }
        if( remove( "C:\\skeleton.csv" ) != 0 ) {perror( "Error deleting file" );}
        if( remove( "C:\\Users\\Rémi\\Desktop\\ActionRecognition\\data\\preds.csv" ) != 0 ) {perror( "Error deleting file" );}
        std::fstream outfile;
        outfile.open ("C:\\preds.csv", std::fstream::in | std::fstream::out | std::fstream::app);
        outfile << flush;
        outfile.close();
        std::fstream outfile2;
        outfile2.open ("C:\\Users\\Rémi\\Desktop\\ActionRecognition\\data\\skeleton.csv", std::fstream::in | std::fstream::out | std::fstream::app);
        outfile2 << flush;
        outfile2.close();
        string command = "python C:\\Users\\Rémi\\Desktop\\ActionRecognition\\main.py";
        std::system(command.c_str());

        SafeRelease(bodyFrameSource);

    }

    if(!kinectSensor || FAILED(hr)) return;
}

/// Update the Kinect data at each frame and update the preview
void MainWindow::updateKinectData() {

    if(!colorFrameReader) return;
    if(!bodyFrameReader) return;

    IColorFrame* colorFrame = NULL;
    IBodyFrame* bodyFrame = NULL;

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

    // Body
    hr = bodyFrameReader->AcquireLatestFrame(&bodyFrame); // Get the last body frame from the sensor
    if(SUCCEEDED(hr)) {
        IBody* body[BODY_COUNT]={0};
        bodyFrame->GetAndRefreshBodyData(_countof(body), body);
        for (int i = 0; i < BODY_COUNT; i++) {
            body[i]->get_IsTracked(&tracked);
            if (tracked) {
                hr = body[i]->GetJoints(JointType_Count, joints);
                if(SUCCEEDED(hr)){
                    std::ofstream outfile;
                    outfile.open("C:\\Users\\Rémi\\Desktop\\ActionRecognition\\data\\skeleton.csv", std::ios::app | std::ios::binary);
                    std::string buff1 = std::to_string(joints[0].Position.X);
                    std::string buff2 = std::to_string(joints[0].Position.Y);
                    std::string buff3 = std::to_string(joints[0].Position.Z);
                    outfile << buff1;
                    outfile << ";";
                    outfile << buff2;
                    outfile << ";";
                    outfile << buff3;
                    for (int j = 1; j < _countof(joints); ++j)
                    {
                        std::string buff1 = std::to_string(joints[j].Position.X);
                        std::string buff2 = std::to_string(joints[j].Position.Y);
                        std::string buff3 = std::to_string(joints[j].Position.Z);
                        outfile << ";";
                        outfile << buff1;
                        outfile << ";";
                        outfile << buff2;
                        outfile << ";";
                        outfile << buff3;
                    }
                    outfile << "\n";
                    outfile.close();
                }
                break;
            }
        }
   }

    if(SUCCEEDED(hr)) {
        ifstream file ( "C:\\Users\\Rémi\\Desktop\\ActionRecognition\\data\\preds.csv" );
        string lastLine;
        if(file.is_open()) {
            file.seekg(-1,ios_base::end);                // go to one spot before the EOF

            bool keepLooping = true;
            while(keepLooping) {
                char ch;
                file.get(ch);                            // Get current byte's data

                if((int)file.tellg() <= 1) {             // If the data was at or before the 0th byte
                    file.seekg(0);                       // The first line is the last line
                    keepLooping = false;                // So stop there
                }
                else if(ch == '\n') {                   // If the data was a newline
                    keepLooping = false;                // Stop at the current position.
                }
                else {                                  // If the data was neither a newline nor at the 0 byte
                    file.seekg(-2,ios_base::cur);        // Move to the front of that data, then to the front of the data before it
                }
            }
            getline(file,lastLine);
            file.close();
        }
        stringstream lastLineStream(lastLine);
        string value;
        string values[6]= {"Sitting down","Standing up","Reading","Staggering","Falling","Walking"};
        std::map<std::string, float> Map;
        std::map<std::string, int> Mapname;
        int count=0;

        while (getline(lastLineStream, value, ';'))
        {
            Map.insert (std::make_pair(values[count], atof(value.c_str())));
            Mapname.insert (std::make_pair(values[count], count));
            count++;
        }

        QPieSeries *series = new QPieSeries();
        series->append("Sitting down", Map.find("Sitting down")->second);
        series->append("Standing up", Map.find("Standing up")->second);
        series->append("Reading", Map.find("Reading")->second);
        series->append("Staggering", Map.find("Staggering")->second);
        series->append("Falling", Map.find("Falling")->second);
        series->append("Walking", Map.find("Walking")->second);

        float currentMax = 0;
        std::string arg_max = "unknown";
        for(auto it = Map.cbegin(); it != Map.cend(); ++it ) {

            if (it ->second > currentMax) {
                arg_max = it->first;
                currentMax = it->second;
            }
        }

        if (currentMax==0){
            QPieSeries *series2 = new QPieSeries();
            series2->append("unknown", 1);
            series2->setLabelsVisible();
            series2->setLabelsPosition(QPieSlice::LabelInsideHorizontal);
            QChart *chart = new QChart();
            chart->addSeries(series2);
            chart->setTitle("predicition");
            chart->legend()->hide();
            QChartView *chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            ui->gridLayout->addWidget(chartView,0,0);
        }

        else{
            QPieSlice *slice = series->slices().at(Mapname.find(arg_max)->second);
            slice->setExploded();
            slice->setLabelVisible();
            slice->setPen(QPen(Qt::darkGreen, 2));
            slice->setBrush(Qt::green);
            series->setLabelsVisible();
            series->setLabelsPosition(QPieSlice::LabelInsideHorizontal);
            QChart *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("predicition");
            chart->legend()->hide();
            QChartView *chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            ui->gridLayout->addWidget(chartView,0,0);
        }


        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        auto date = oss.str();
        std::string histostd =date+" prediction : "+arg_max+" "+to_string(currentMax*100)+"%";
        QString histo = QString::fromStdString(histostd);
        ui->textBrowser->append(histo);
    }

    SafeRelease(bodyFrame); // Release the depth frame (free the memory)



}
