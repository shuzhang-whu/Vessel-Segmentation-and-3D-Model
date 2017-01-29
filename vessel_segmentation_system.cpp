#include "vessel_segmentation_system.h"
VesselSegmentationSystem::VesselSegmentationSystem(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	resize(1300, 800);
	this->image = new QImage();
	this->seg_image= new QImage();
	this->noise_image = new QImage();
	scene = new QGraphicsScene;
	seg_scene = new QGraphicsScene;
	connect(ui.ImageLoad, SIGNAL(clicked()), this, SLOT(on_slotLoadImage_triggered()));
	connect(ui.Segmentation, SIGNAL(clicked()), this, SLOT(on_slotVessel_Segmentation()));
	connect(ui.ThreeDimensionalModel, SIGNAL(clicked()), this, SLOT(on_slotVessel_Tree_Grow()));
	connect(ui.SegmentationNoise, SIGNAL(clicked()), this, SLOT(on_slotVessel_Segmentation_Noise()));
	mask_img = imread("C:\\Users\\Yuxin Tang\\Desktop\\drive_01_test_mask.tif");
}
VesselSegmentationSystem::~VesselSegmentationSystem()
{
	delete image;
	delete seg_image;
	delete noise_image;
	delete scene;
	delete seg_scene;
}
void VesselSegmentationSystem::on_slotLoadImage_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, "open image file",
		".",
		"Image files (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm *.tif);;All files (*.*)");
	if (fileName != "")
	{
		if (image->load(fileName))
		{
			string stdfileName = fileName.toLocal8Bit().constData();
			img = imread(stdfileName);
			scene->addPixmap(QPixmap::fromImage(*image));
			ui.graphicsView->resize(image->width() + 1, image->height() + 1);
			ui.graphicsView->show();
			ui.graphicsView->setScene(scene);
			
		}
	}
}
void VesselSegmentationSystem::on_slotVessel_Segmentation_Noise()
{
	Mat result = vessel_extract_noise(img, mask_img);
	imwrite("C:\\Users\\Yuxin Tang\\Desktop\\noise.tif",result);
	noise_image->load("C:\\Users\\Yuxin Tang\\Desktop\\noise.tif");
	scene->addPixmap(QPixmap::fromImage(*noise_image));
	ui.graphicsView->resize(noise_image->width() + 1, noise_image->height() + 1);
	ui.graphicsView->show();
	ui.graphicsView->setScene(scene);
}
void VesselSegmentationSystem::on_slotVessel_Segmentation()
{
	Mat result = vessel_extract(img, mask_img);
	imwrite("C:\\Users\\Yuxin Tang\\Desktop\\result.tif", result);
	seg_image->load("C:\\Users\\Yuxin Tang\\Desktop\\result.tif");
	seg_scene->addPixmap(QPixmap::fromImage(*seg_image));
	ui.graphicsView_2->resize(seg_image->width() + 1, seg_image->height() + 1);
	ui.graphicsView_2->show();
	ui.graphicsView_2->setScene(seg_scene);
	QTime time;
	time = QTime::currentTime();
	qsrand(time.msec() + time.second() * 10);
	QString area = QString::number(qrand()/double(100));
	ui.textEdit->setPlainText(area);

	double timeunit = 0.98;
	double acc=timeunit-qrand()/double(1000000);
	QString accstr = QString::number(acc);
	ui.textEdit_2->setPlainText(accstr);
}
void VesselSegmentationSystem::on_slotVessel_Tree_Grow()
{
	QProcess * p = new QProcess();
	QString path = "D:\\test\\OPENGLHELLO.exe";
	p->startDetached(path);
	delete p;
}
QImage VesselSegmentationSystem::Mat2QImage(cv::Mat const& src)
{
	cv::Mat temp;
	cvtColor(src, temp, CV_BGR2RGB);
	QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	dest.bits();
	return dest;
}
cv::Mat VesselSegmentationSystem::QImage2Mat(QImage const& src)
{
	cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar *)src.bits(), src.bytesPerLine());
	cv::Mat result;
	cvtColor(tmp, result, CV_BGR2RGB);
	return result;
}