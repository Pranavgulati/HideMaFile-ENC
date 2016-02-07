#include "Methods.h"
#define PI 3.14159265359
Methods::Methods() {
}

Methods::~Methods() {
}


Point2f Methods::calculateCOG(Image* input) {
    Point2f COG = Point2f(0, 0);
    uchar *data = input->img->data;
    int widthstep = input->img->step;
    int sign_pixel_count = 0;

    for (int i = input->y_upper + 1; i < input->y_lower - 1; i++) {
        for (int j = input->x_left + 1; j < input->x_right - 1; j++) {
            if (data[(i * widthstep) + j] == 0) {
                COG.y += i;
                COG.x += j;
                sign_pixel_count += 1;
            }
        }
    }
    if (sign_pixel_count != 0) {
        COG.x /= sign_pixel_count;
        COG.y /= sign_pixel_count;
    } else {
        COG.y = (input->y_lower + input->y_upper) / 2;
        COG.x = (input->x_left + input->x_right) / 2;
    }

	input->cog = COG;
    input->sign_pixel_count = sign_pixel_count;
	return input->cog;
}

void Methods::calculateCOG(vector<Image> *result) {
    unsigned long count = 0;
    Image temp_img;
	for (count = 0; count < result->size(); count++) {
        temp_img = result->at(count);
        Methods::calculateCOG(&temp_img);
		
        result->at(count).cog = temp_img.cog;
        result->at(count).sign_pixel_count = temp_img.sign_pixel_count;
				
    }
	
}
void Methods::CropImage(Image  *input, int sensitivity){
	Point cropu = Point(0, 0);
	Point cropl = Point(0, 0);

	uchar *data = input->img->data;
	int widthstep = input->img->step;
	int sign_pixel_count = 0, idle = 0, prev = 5000;
	//cout << "hi!\n" << input->y_lower << "<-rows\n" << input->x_right << "<-cols\n"<<widthstep;
	for (int i = input->y_upper + 1; i < input->y_lower - 1; i++) {
		int flag = 0; sign_pixel_count = 0;
		for (int j = input->x_left + 1; j < input->x_right - 1; j++) {
			if (data[(i * widthstep) + j] == 0) {
				cropu.y = i;
				idle = j - prev;
				prev = j;
				if (idle >= sensitivity){ sign_pixel_count = 0; }
				sign_pixel_count++;
				if (sign_pixel_count >= 6){ flag = 1; break; }
			}
		}
		if (flag == 1){ break; }
	}
	idle = 0; prev = 5000;
	for (int i = input->y_lower - 1; i > input->y_upper + 1; i--) {
		int flag = 0; sign_pixel_count = 0;
		for (int j = input->x_left + 1; j < input->x_right - 1; j++) {
			if (data[(i * widthstep) + j] == 0) {
				cropl.y = i;
				idle = j - prev;
				prev = j;
				if (idle >= sensitivity){ sign_pixel_count = 0; }
				sign_pixel_count++;
				if (sign_pixel_count >= 6){ flag = 1; break; }
			}
		}
		if (flag == 1){ break; }
	}
	idle = 0; prev = 5000;
	for (int j = input->x_left + 1; j < input->x_right - 1; j++) {
		int flag = 0; sign_pixel_count = 0;
		for (int i = input->y_upper + 1; i < input->y_lower - 1; i++) {
			if (data[(i * widthstep) + j] == 0) {
				cropu.x = j;
				idle = i - prev;
				prev = i;

				if (idle >= sensitivity){ sign_pixel_count = 0; }
				sign_pixel_count++;
				if (sign_pixel_count >= 4){ flag = 1; break; }
			}
		}
		if (flag == 1){ break; }
	}
	idle = 0; prev = 0;
	for (int j = input->x_right - 1; j > input->x_left + 1; j--) {
		int flag = 0; sign_pixel_count = 0;
		for (int i = input->y_upper + 1; i < input->y_lower - 1; i++) {
			if (data[(i * widthstep) + j] == 0) {
				cropl.x= j;
				idle = i - prev;
				prev = i;
				if (idle >= sensitivity){ sign_pixel_count = 0; }
				sign_pixel_count++;
				if (sign_pixel_count >= 4){ flag = 1; break; }
			}
		}
		if (flag == 1){ break; }
	}

	Mat(*input->img,Rect(cropu,cropl)).copyTo(*input->img);
		input->y_upper = 0;
		input->y_lower = cropl.y-cropu.y;
		input->x_left = 0;
		input->x_right = cropl.x-cropu.x;
		//cout << "done\n";
}

void rotateByAngle(Mat &dst, double angle)
{
	Point2f pt(dst.cols / 2., dst.rows / 2.);
	Mat r = getRotationMatrix2D(pt, angle, 1);
	warpAffine(dst, dst, r, Size(dst.cols, dst.rows), INTER_CUBIC, BORDER_CONSTANT, Scalar(255, 255, 255, 0));

}

Point2f Methods::calculateCOG(Mat input){

Point2f COG = Point2f(0, 0);
uchar *data = input.data;
int widthstep = input.step;
int sign_pixel_count = 0;

for (int i = 1; i < input.rows; i++) {
	for (int j = 1; j < input.cols; j++) {
		if (data[(i * widthstep) + j] == 0) {
			COG.y += i;
			COG.x += j;
			sign_pixel_count += 1;
		}
	}
}
if (sign_pixel_count != 0) {
	COG.x /= sign_pixel_count;
	COG.y /= sign_pixel_count;
}
else {
	COG.y = input.rows / 2;
	COG.x = input.cols / 2;
}

return COG;




}


void Methods::signAlign(Mat dst)
{
	
	//find angle 
	//Image(Mat*image, int yu, int yl, int xl, int xr, Point2f temp);
	Mat temp1,temp2;
	temp1 = dst(Rect(0, 0, dst.cols / 2, dst.rows));
	temp2 = dst(Rect(dst.cols / 2, 0, dst.cols / 2, dst.rows));
	Image temp_image1(&temp1, 0, temp1.rows, 0, temp1.cols, Point2f(0, 0));
	Image temp_image2(&temp2, 0, temp2.rows, 0, temp2.cols, Point2f(0, 0));
	cout << "BEFORE\n";
	cout << temp_image1.x_left << "-Upper  down-" << temp_image1.y_lower<<"   ";
	cout << temp_image2.x_left << "-Upper  down-" << temp_image2.y_lower;
	CropImage(&temp_image1, 20);
	CropImage(&temp_image2, 20);
	cout << "\nAFTER\n";
	cout << temp_image1.x_left << "-Upper  down-" << temp_image1.y_lower << "   ";
	cout << temp_image2.x_left << "-Upper  down-" << temp_image2.y_lower << "\n\n";
	//float angle1=calculateAngle(Point2f(temp_image1.x_left, temp_image1.y_upper), Point2f(temp_image2.x_left, temp_image2.y_upper));
	float angle2 = calculateAngle(Point2f(0, temp_image1.y_lower), Point2f(1, temp_image2.y_lower));
	namedWindow("temp1", CV_WINDOW_NORMAL);
	namedWindow("temp2", CV_WINDOW_NORMAL);
	imshow("temp1", temp1);
	imshow("temp2", temp2);
	waitKey(0);
	cout << angle2<<"\n\n";
	double angle = angle2;
	rotateByAngle(dst, angle);

}

int Methods::preprocess(Mat *bin_image, vector<Image> *output,int doc) {
	Image temp_image(bin_image , 0, bin_image->rows, 0, bin_image->cols,Point2f(0,0));
	if (bin_image->channels()> 2){ cvtColor(*bin_image, *bin_image, CV_BGR2GRAY); }
	if (doc==1){
	adaptiveThreshold(*bin_image, *bin_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 51, 45);}
	else{
		adaptiveThreshold(*bin_image, *bin_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 13, 23);
	}
		int sensitivity = 4;
		CropImage(&temp_image, sensitivity);
		if (temp_image.x_right < 10 || temp_image.y_lower < 10){  return 0; }
		resize(*bin_image, *bin_image, Size(200, 100), 0, 0);
		//rotate here and then resize--1-11-2015
	//	signAlign(*bin_image);

		temp_image.y_lower = 100;
		temp_image.x_right = 200;
		temp_image.y_upper = 0;
		temp_image.x_left = 0;
		temp_image.imgCOG = calculateCOG(&temp_image);
		sampleCOG = temp_image.imgCOG;
		output->push_back(temp_image);
		return 1;
}

void Methods::partitionImage(Mat *bin_image, int n_partition,vector<Image> *result) {
	Image temp_image = result->at(0);
    vector<Image> sub_images;
    sub_images.push_back(temp_image);
    Point2f temp_COG;
    unsigned long count = 0;
    while (sub_images.size() < n_partition) {
        temp_image = sub_images.front();
		
        Methods::calculateCOG(&temp_image);
        temp_COG = temp_image.cog;
        sub_images.at(0).cog = temp_COG;
		sub_images.push_back(Image(bin_image, temp_image.y_upper, temp_COG.y, temp_image.x_left, temp_COG.x,sampleCOG));
        sub_images.push_back(Image(bin_image, temp_image.y_upper, temp_COG.y, temp_COG.x, temp_image.x_right,sampleCOG));
        sub_images.push_back(Image(bin_image, temp_COG.y, temp_image.y_lower, temp_image.x_left, temp_COG.x,sampleCOG));
        sub_images.push_back(Image(bin_image, temp_COG.y, temp_image.y_lower, temp_COG.x, temp_image.x_right,sampleCOG));
        sub_images.erase(sub_images.begin());
        //count++;
    }
    //    cout << "outt" << endl;
    result->swap(sub_images);
    //    cout << "size: " << sub_images.size() << " " << result->size() << endl;
}

void Methods::showSubImages(vector<Image> *result, Mat *orig_img,String imagename) {
    Image temp_image;
     cout << "show subimages: " << result->size() << endl;
    for (unsigned int i = 0; i < result->size(); i++){
        temp_image = result->at(i);

		line(*temp_image.img, Point(temp_image.x_left, temp_image.y_lower), Point(temp_image.x_right, temp_image.y_lower), Scalar(0, 0, 0, 0), 1);
		line(*temp_image.img, Point(temp_image.x_left, temp_image.y_upper), Point(temp_image.x_right, temp_image.y_upper), Scalar(0, 0, 0, 0), 1);
		line(*temp_image.img, Point(temp_image.x_left, temp_image.y_lower), Point(temp_image.x_left, temp_image.y_upper), Scalar(0, 0, 0, 0), 1);
		line(*temp_image.img, Point(temp_image.x_right, temp_image.y_lower), Point(temp_image.x_right, temp_image.y_upper), Scalar(0, 0, 0, 0), 1);
		//line(*temp_image.img, Point(temp_image.cog.y, temp_image.cog.x), Point(temp_image.imgCOG.y,temp_image.imgCOG.x), Scalar(0, 0, 0));
		
    }
	imshow(imagename, *temp_image.img);
	waitKey(0);
}

float Methods::calculateDistance(Point2f pt1, Point2f pt2) {
    return sqrt(pow(pt1.x-pt2.x, 2) + pow(pt1.y-pt2.y, 2));
}

float Methods::calculateDistance(Point2f pt1) {
	return sqrt(pow(pt1.x, 2) + pow(pt1.y , 2));
}

void Methods::calculateCOGDistances(vector<Image>* sub_images) {
    for (int i = 0; i < sub_images->size(); i++) {
	
		sub_images->at(i).cog_distance = Methods::calculateDistance(sub_images->at(i).cog, sub_images->at(i).imgCOG);
		
    }
}



float Methods::calculateAngle(Point2f pt1, Point2f pt2) {
	Point2f vect; vect.x = pt1.x - pt2.x; vect.y = pt1.y - pt2.y;
	if (vect.y == 0) {
		return 90;
	}
	else {
		return (atan(vect.x / vect.y))*(180/PI);
	}
}

void Methods::calculateCOGAngles(vector<Image>* sub_images) {
	for (int i = 0; i < sub_images->size(); i++) {
		sub_images->at(i).cog_angle = Methods::calculateAngle(sub_images->at(i).cog, sub_images->at(i).imgCOG);
	}
}

float Methods::comparef(vector<Image>* test, vector<Image>* reference,float accuracy) {
	assert(test->size() == reference->size());
	Image temp_img;
	float match = 0;
	float pixsum=0;
	for (int i = 0; i < test->size(); i++) {
		temp_img = test->at(i);
		int diffdist = abs(temp_img.cog_distance - reference->at(i).cog_distance);
		float diffangle = abs(temp_img.cog_angle- reference->at(i).cog_angle);
		//------------------------------------------
		if (diffdist<accuracy*100 && diffangle<accuracy*50){ match++; }
		float tarea = (temp_img.y_lower - temp_img.y_upper)*(temp_img.x_right - temp_img.x_left);
		float rarea = (reference->at(i).y_lower - reference->at(i).y_upper)*(reference->at(i).x_right - reference->at(i).x_left);
		float tpix = temp_img.sign_pixel_count / tarea;
		float rpix =reference->at(i).sign_pixel_count / rarea;
		pixsum =pixsum+abs(tpix-rpix);
		
	}
	float result = (((match / reference->size()) * 100) + ((1 - (pixsum / reference->size())) * 100)) / 2;
	//imshow("test", *temp_img.img);
	//waitKey();
	return result;
}



void Methods::binarizeImage(Mat * input) {
    uchar *data = (uchar *) input->data;
    int widthstep = input->step;
    for (int i = 0; i < input->rows; i++) {
        for (int j = 0; j < input->cols; j++) {
            if (data[i * widthstep + j] < 225)
                data[i * widthstep + j] = 0;
            else
                data[i * widthstep + j] = 255;
        }
    }
}

void Methods::printMatrix(float **mat, int row, int col) {
    cout << "===========================" << endl;
    cout << "Printing Matrix" << endl;
    cout << "===========================" << endl;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

void Methods::printSubImage(vector<Image> *v) {
    Image temp_img;
    for (int i = 0; i < v->size(); i++) {
        temp_img = v->at(i);
       // cout << temp_img.y_upper << " , " << temp_img.y_lower << " , " << temp_img.x_left << " , " << temp_img.x_right << " , " << temp_img.sign_pixel_count << endl;
		cout << temp_img.cog_angle << "         , " << temp_img.cog_distance << " ,          " << temp_img.sign_pixel_count <<endl;
    } 
    cout << endl;
}

