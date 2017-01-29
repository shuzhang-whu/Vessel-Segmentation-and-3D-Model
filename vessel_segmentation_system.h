#ifndef VESSELSEGMENTATIONSYSTEM_H
#define VESSELSEGMENTATIONSYSTEM_H
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <QtWidgets/QMainWindow>
#include "ui_vesselsegmentationsystem.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qfiledialog.h"
#include "qimage.h"
#include "qmainwindow.h"
#include "vessel_extract.h"
#include "qprocess.h"
#include "qstring.h"
#include "qtimer.h"
#include "qtimezone.h"
#include "qtimeline.h"
class VesselSegmentationSystem : public QMainWindow
{
	Q_OBJECT
public:
	VesselSegmentationSystem(QWidget *parent = 0);
	~VesselSegmentationSystem();
private:
	Ui::VesselSegmentationSystemClass ui;
	QImage * image;
	QImage * seg_image;
	QImage * noise_image;
	QGraphicsScene * scene;
	QGraphicsScene * seg_scene;
	cv::Mat img;
	cv::Mat mask_img;
private:
	QImage Mat2QImage(cv::Mat const& src);
	cv::Mat QImage2Mat(QImage const& src);
private slots:
	void on_slotLoadImage_triggered();
	void on_slotVessel_Segmentation();
	void on_slotVessel_Segmentation_Noise();
	void on_slotVessel_Tree_Grow();
};

#endif // VESSELSEGMENTATIONSYSTEM_H
