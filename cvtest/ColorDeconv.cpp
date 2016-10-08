#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;

// Color Deconvolution: 
//------separate the RGB image into specific(HEB) stain channels
Mat ColorDeconv(Mat I){
	//=======STEP 1=======
	// rgbI <-- RGB channels of (I) with 3 channels & 64-bit precision
	Mat rgbI = I.clone();
	switch (rgbI.type())
	{
	case CV_8UC3:
		//rgbI.convertTo(rgbI, CV_64FC3);
		//rgbI /= 255.0;
		rgbI.convertTo(rgbI, CV_64FC3, 1.0 / 255);
		break;
	case CV_64FC3:break;
	case CV_32FC3:
		rgbI.convertTo(rgbI, CV_64FC3);
		break;
	default:
		cout << "Type of image doesn't belong to {8U,32F,64F}" << endl;
		break;
	}
//=======STEP 2=======
	// odI <-- optical density of (I)
	Mat odI;
	odI = rgbI + 1 / 255.0;
	log(odI, odI);
	odI = -(255 * odI) / log(255);
//=======STEP 3=======
	// M <-- stain matrix of HE
	Mat M = (Mat_<double>(3, 3) <<
		0.644211, 0.716556, 0.266844,
		0.092789, 0.954111, 0.283111,
		0.759199, 0, 0.921218);
	// normalize M by L2-NORM in every row
	// rowData: used to extract each row of (M)
	Mat rowData;
	for (int i = 0; i < 3; i++){
		rowData = M.rowRange(i, i + 1);
		normalize(rowData, rowData);
	}
	rowData.~Mat();
	// D <-- M^-1
	Mat D = M.inv();
//======STEP 4=======
	// odHEB <-- optical density of HEB image
	Mat odHEB;
	// odHEB <-- D^T * odI
	transform(odI, odHEB, D.t());

	/************* HERE IS ANOTHER BETA VERSION***********************
	// odHEB <-- optical density of HEB image
	Mat odHEB;

	// c_odRGB <-- 3-element array of 3 channels of optical density of RGB image
	Mat c_odRGB[3];
	split(odRGB, c_odRGB);
	// c_odHEB <-- 3-element array of 3 channels of optical density of HEB image
	//Mat c_odHEB[3];
	vector<Mat> c_odHEB;
	for (int c = 0; c < 3; c++){
		c_odHEB.push_back(D.at<double>(0, c)*c_odRGB[0]
			+ D.at<double>(1, c)*c_odRGB[1]
			+ D.at<double>(2, c)*c_odRGB[2]);
	}
	merge(c_odHEB, odHEB);
	imshow("hava a go", c_odHEB[0] / 255.0);
	waitKey(0);
	return 4;
	************************************************************/

//======STEP 5=======
	// hebI <-- HEB channels of image (I)
	Mat hebI;
	hebI = -(odHEB - 255)*log(255) / 255.0;
	exp(hebI, hebI);
//=======STEP 6=======
	// Before thresholding, convert (hebI) to 32-bit matrix
	// because cv::threshold can handle only 8-bit & 32-bit
	hebI.convertTo(hebI, CV_32FC3);
	threshold(hebI, hebI, 255, 255, THRESH_TRUNC);
	hebI = hebI + 0.5;
	Mat output;
	// here is no need to multiply 255
	hebI.convertTo(output, CV_8UC3);
//======RETURN=======
	return output;
}