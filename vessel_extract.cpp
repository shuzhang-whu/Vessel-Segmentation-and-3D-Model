#include "vessel_extract.h"
Mat vessel_extract(Mat img, Mat mask_img)
{
	vector<Mat> image_channels;
	vector<Mat> mask_channels;
	int w = 15;
	int step = 2;
	split(img, image_channels);
	Mat gChannel = image_channels[1];
	split(mask_img, mask_channels);
	Mat mask_gChannel = mask_channels[1];
	gChannel = 255 - gChannel;
	Mat dilated = grow_boundary(gChannel, mask_gChannel);
	Mat dilated_double = Mat::zeros(mask_img.rows,mask_img.cols,CV_64FC1);
	Mat dilated_norm = Mat::zeros(mask_img.rows,mask_img.cols,CV_64FC1);
	dilated.convertTo(dilated_double, CV_64FC1, 1.0/255.0);
	dilated_norm = global_normalize(dilated_double,mask_gChannel);
	Mat line_response = Mat::zeros(mask_img.rows,mask_img.cols,CV_64FC1);
	Mat line_response_norm = Mat::zeros(mask_img.rows,mask_img.cols,CV_64FC1);
	int scales = 0;
	for(int line=1; line<16; line+=2) {
		line_response = get_lineresponse(dilated_double, line, w);
		line_response_norm = global_normalize(line_response,mask_gChannel);
		dilated_norm = dilated_norm + line_response_norm;
		scales++;
	}
	Mat avg_response = Mat::zeros(img.rows,img.cols,CV_64FC1);
	Mat vessels = Mat::zeros(img.rows,img.cols,CV_8UC1);
	avg_response = dilated_norm / (1+scales);
	double thresh = 0.56;

	ofstream Savefile("vessel_extract.txt");
	Savefile << "血管区域像素点为：\n";
	int count1 = 0;
	int count2 = 0;

	for (int i=0; i<img.rows; i++)
	for (int j = 0; j<img.cols; j++)
	{
		count1++;
		if (avg_response.at<double>(i, j) > thresh)
		{
			count2++;
			vessels.at<uchar>(i, j) = 255;
			Savefile << "(" << i << "," << j << ")" << "\n";
		}
	}
	double area = count2*1.0 / count1;
	Savefile << "血管提取成功！\n";
	Savefile << "血管区域面积为："<<area<<"\n";
	Savefile.close();
	cout << "血管提取成功！" << endl;
	cout << "血管区域面积为：" << area << endl;
	vessels=find_connected(vessels, 200);
	return vessels;
}

Mat get_lineresponse(Mat img, int linescale, int w)
{
	Mat avg_kernel(w, w, CV_64FC1, Scalar(1));
	avg_kernel = avg_kernel / (w*w);
	Mat avg_response;
	Mat linemask(linescale, linescale, CV_8UC1, Scalar(0));
	Mat linemask_double(linescale, linescale, CV_64FC1, Scalar(0));
	Mat line_strength;
	Mat line_response = (-200.0)*Mat::ones(img.rows, img.cols, CV_64FC1);
	filter2D(img, avg_response, img.depth(), avg_kernel);
	for (int theta=0; theta<180; theta+=15) {
		linemask = getLineMask(theta, linescale)*1.0;
		linemask.convertTo(linemask_double, CV_64FC1, 1.0 / 255.0);
		linemask_double = linemask_double / countNonZero(linemask_double);
		filter2D(img, line_strength, img.depth(), linemask_double);
		line_strength = line_strength - avg_response;
		for(int i=0; i<img.rows; i++)
			for (int j=0; j<img.cols; j++) {
				if(line_strength.at<double>(i,j) > line_response.at<double>(i,j))
					line_response.at<double>(i,j) = line_strength.at<double>(i,j);
			}
	}
	return line_response;
}

Mat grow_boundary(Mat img, Mat mask_img, int erosionsize, int iterations)
{
	/* contour before erosion */
	Mat dilated = Mat::zeros(mask_img.rows,mask_img.cols,CV_8UC1);
	for(int i=0; i<mask_img.rows; i++)
		for (int j=0; j<mask_img.cols; j++) {
			if (i==0 || i==mask_img.rows-1)
				mask_img.at<uchar>(i,j) = 0;
			if (j==0 || j==mask_img.cols-1)
				mask_img.at<uchar>(i,j) = 0;
		}
		Mat erode_element = getStructuringElement(MORPH_ELLIPSE,Size(erosionsize,erosionsize));
		Mat erode_mask = mask_img.clone();
		erode(mask_img,erode_mask,erode_element);
		dilated = img.mul(erode_mask / 255);
		Mat oldmask = erode_mask.clone();
		int filter_rows[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
		int filter_cols[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
		Mat newmask = Mat::zeros(mask_img.rows,mask_img.cols,CV_8UC1);
		Mat outerborder = Mat::zeros(mask_img.rows,mask_img.cols,CV_8UC1);
		Mat temp = Mat::zeros(mask_img.rows,mask_img.cols,CV_8UC1);
		int pixelrow, pixelcol;
		int total, count;

		/* creat structure element "diamond" for dilation */
		Mat diamond(3,3,CV_8U,Scalar(1));
		diamond.at<uchar>(0,0)=0;
		diamond.at<uchar>(2,0)=0;
		diamond.at<uchar>(0,2)=0;
		diamond.at<uchar>(2,2)=0;

		/* iterative dilation */
		for (int i=0; i<iterations; i++) {
			outerborder = Mat::zeros(mask_img.rows,mask_img.cols,CV_8UC1);
			dilate(oldmask,newmask,diamond);
			outerborder = newmask - oldmask;
			for(int j=0; j<outerborder.rows; j++)
				for(int k=0; k<outerborder.cols; k++)
					if(outerborder.at<uchar>(j,k))
					{	
						total = 0;
						count = 0;
						for(int h=0; h<9; h++) {
							pixelrow = j + filter_rows[h];
							pixelcol = k + filter_cols[h];
							if (pixelrow < outerborder.rows && pixelrow >=0 \
								&& pixelcol < outerborder.cols && pixelcol >=0 \
								&& oldmask.at<uchar>(pixelrow, pixelcol)) {
									total = total + dilated.at<uchar>(pixelrow,pixelcol);
									count ++;
							}
						}
						dilated.at<uchar>(j,k) = total / count;
					}
					oldmask = newmask.clone();
		}
		return dilated;
}

Mat global_normalize(Mat img, Mat mask)
{
	Mat norm_img(img.rows, img.cols, CV_64FC1, Scalar(0));
	Scalar mean, stdDev;
	int used_pixels = countNonZero(mask);
	double diff;
	meanStdDev(img,mean,stdDev,mask);
	stdDev.val[0] = stdDev.val[0]*std::sqrt(used_pixels / (used_pixels-1.0));
	for(int i=0; i<norm_img.rows; i++)
		for(int j=0; j<norm_img.cols; j++) 
			if(mask.at<uchar>(i,j)) {
				diff = (img.at<double>(i,j) - mean.val[0]);
				if(diff > 0) {
					norm_img.at<double>(i,j) = diff / stdDev.val[0];
				}

			}
			return norm_img;
}

Mat getLinePts(Point pt0, Point pt1)//返回二维矩阵;
{
	int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
	if (pt0.x < pt1.x)
	{
		x0 = pt0.x;
		y0 = pt0.y;
		x1 = pt1.x;
		y1 = pt1.y;
	}
	else
	{
		x0 = pt1.x;
		y0 = pt1.y;
		x1 = pt0.x;
		y1 = pt0.y;
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int x = 0,y = 0;
	int maxi = 0, mini = 0, maxc = 0;
	maxc = (abs(dx)>abs(dy))?abs(dx):abs(dy);
	int ind = 0;//因为下标从0开始；
	uchar * dataPtr;
	Mat lineMask(maxc + 1, 2, CV_8U, Scalar(0));
	if (dx == 0)
	{
		x = x0;
		//if (dy < 0)
		{
			maxi = (y0>y1)?y0:y1;
			mini = (y0<y1)?y0:y1;
		}
		for ( y = mini; y <= maxi; y++)
		{
			dataPtr = lineMask.ptr<uchar>(ind);
			dataPtr[0] = y;
			dataPtr[1] = x;
			ind++;
		}
	}
	else if (abs(dy) > abs(dx))
	{
		//if (dy < 0)
		{
			maxi = (y0>y1)?y0:y1;
			mini = (y0<y1)?y0:y1;
		}
		for (y = mini; y <= maxi; y++)
		{
			x = ((dx + 0.0000)/dy + 0.0000) * (y - y0) + x0;
			dataPtr = lineMask.ptr<uchar>(ind);
			dataPtr[0] = y;
			dataPtr[1] = x;
			ind++;
		}
	}
	else
	{
		for (x = x0; x <= x1; x++)
		{
			y = ((dy + 0.0000) / dx + 0.0000) * (x - x0) + y0;
			dataPtr = lineMask.ptr<uchar>(ind);
			dataPtr[0] = y;
			dataPtr[1] = x;
			ind++;
		}
	}
	return lineMask;
}

Mat drawLine(Point pt0, Point pt1, Mat orgmask)
{
	Mat mask = orgmask.clone();
	int maxc = 0;
	int dx = pt0.x- pt1.x;
	int dy = pt0.y - pt1.y;
	maxc = (abs(dx)>abs(dy))?abs(dx):abs(dy);
	Mat tmpMat(maxc + 1, 2, CV_8U, Scalar(0));
	int tmp = 0, tmp1 = 0;
	tmpMat = getLinePts(pt0, pt1);
	uchar* dataPtr;
	uchar* dataPtr1;
	for(int i = 0; i < maxc + 1; i++)
	{
		dataPtr1 = tmpMat.ptr<uchar>(i);
		tmp = dataPtr1[0];
		dataPtr = mask.ptr<uchar>(tmp);
		tmp1 = dataPtr1[1];
		dataPtr[tmp1] = 255;
	}

	return mask;
}

Mat getBaseMask(int theta, int masksize)
{
	Mat mask(masksize,masksize,CV_8U,Scalar(0));
	int halfsize = (masksize - 1) / 2;
	int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	Point pt0, pt1;
	uchar* dataPtr;
	if(theta == 0)
	{
		dataPtr = mask.ptr<uchar>(halfsize);
		for(int i = 0; i < mask.cols; i++)
			dataPtr[i] = 255;
	}
	else if(theta == 90)
	{
		for(int i = 0; i < mask.rows; i++)
		{
			dataPtr = mask.ptr<uchar>(i);
			dataPtr[halfsize] =255;
		}
	}
	else
	{
		x0 = -halfsize;
		y0 = x0 * (tan(((float)theta + 0.0000)  / 180 * PI) + 0.0000);
		if (y0 < -halfsize)
		{
			y0 = -halfsize;
			x0 = y0 * ((1/(tan((float)theta * PI / 180) + 0.0000) + 0.0000));
		}
		x1 = halfsize;
		y1 = x1 * (tan((float)theta * PI / 180) + 0.0000);
		if (y1 > halfsize)
		{
			y1 = halfsize;
			x1 = y1 * ((1/(tan((float)theta * PI / 180) + 0.0000) + 0.0000));
		}

		pt0.y = halfsize - y0;
		pt0.x = halfsize + x0;
		pt1.y = halfsize - y1;
		pt1.x = halfsize + x1;
		mask = drawLine(pt0,pt1,mask);
	}
	return mask;
}

Mat rotatex(Mat mask)
{
	int h = 0, w = 0;
	h = mask.rows;
	w = mask.cols;
	Mat rotatedMask(h,w,CV_8U,Scalar(0));
	uchar* dataPtr, *dataPtr2;
	for(int i = 0; i < h; i++)
	{
		for(int j = 0; j < w; j++)
		{
			dataPtr = mask.ptr<uchar>(i);
			dataPtr2 = rotatedMask.ptr<uchar>(i);
			dataPtr2[j] = dataPtr[w - j - 1];//以y为对称轴交换矩阵；
		}
	}
	return rotatedMask;
}

Mat getLineMask(int theta, int masksize)
{
	Mat mask;
	Mat linemask;
	if(theta > 90)
	{
		mask = getBaseMask(180 - theta, masksize);
		linemask = rotatex(mask);
	}
	else
	{
		linemask = getBaseMask(theta, masksize);
	}
	return linemask;
}

Mat find_connected(Mat img, int area) {
	Mat label, stats, centroids;
	connectedComponentsWithStats(img, label, stats, centroids, 4, CV_32S);
	Mat oimg(img.size[0], img.size[1], CV_8UC1);
	oimg = 0;
	for (int i = 1; i < stats.size[0]; i++) {
		if (stats.at<int32_t>(i, CC_STAT_AREA) < area)
			continue;
		for (int x = stats.at<int32_t>(i, CC_STAT_TOP);
		x < stats.at<int32_t>(i, CC_STAT_TOP) + stats.at<int32_t>(i, CC_STAT_HEIGHT);
			x++) {
			for (int y = stats.at<int32_t>(i, CC_STAT_LEFT);
			y < stats.at<int32_t>(i, CC_STAT_LEFT) + stats.at<int32_t>(i, CC_STAT_WIDTH);
				y++) {
				if (label.at<uint32_t>(x, y) == i) {
					oimg.at<uchar>(x, y) = 255;
				}
			}
		}
		//        printf("handled %d, area %d\n", i, stats.at<int32_t>(i,CC_STAT_AREA));
	}
	return oimg;
}