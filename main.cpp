#include"vessel_segmentation_system.h"
#include <QtWidgets/QApplication>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VesselSegmentationSystem w;
	w.show();
	return a.exec();
}
