#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include<iostream>
#include<fstream>
#include<math.h>
#include<conio.h>
#include<stdio.h>
using namespace cv;
using namespace std;

// Global Variables
int DELAY_CAPTION = 1500;
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 31;
Mat src, dst;
char window_name[] = "Filter Demo 1";
int display_caption(char* caption)
{
	dst = Mat::zeros(src.size(), src.type());
	putText(dst, caption,
		Point(src.cols / 4, src.rows / 2),
		CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));

	imshow(window_name, dst);
	int c = waitKey(DELAY_CAPTION);
	if (c >= 0) { return -1; }
	return 0;
}

int display_dst(int delay)
{
	Mat noise = src - dst;
	cvtColor(noise, noise, CV_BGR2GRAY);
	namedWindow(window_name, CV_WINDOW_NORMAL);
	imshow(window_name, noise);
	int c = waitKey(delay);
	if (c >= 0) { return -1; }
	return 0;
}
#define MAXTOTALBITS 128
#define DATATYPESIZE 4
#define NUMGROUPS 4
Point *usedCoord;

void getGroups(unsigned char string[],int numBytes,unsigned long output[]){
	//convert string to bitstream and halve the bitstream continuously to get 4 groups
	int numBits = numBytes * 8;
	//one easy way is to convert string into an array of individual bits
	//dont have anything else in mind will be doing this itself

	unsigned char bitStream[MAXTOTALBITS] = { 0 };
	int i = 0;
	for (i = 0; i < numBytes; i++){
		unsigned char temp = string[i];
		for (int j = 0; j < 8; j++){
			unsigned char mask = 0x80;
			
			if ((temp&mask) != 0){ bitStream[i*8 + j] = '1'; }
			else{ bitStream[i*8 + j] = '0'; }
			temp <<= 1;
		}
	}
	bitStream[i * 8] = '\0';
	
	for (i = 0; i < NUMGROUPS; i++){
		for (int j = 0; j < numBits/NUMGROUPS; j++){
			unsigned char a = bitStream[i*(numBits/NUMGROUPS) + j];
			output[i] <<= 1;
			if (a == '1'){	output[i] = output[i] | 0x01; }
			
			}
		}
}	

void getCoords(int x, int y, unsigned long group[], Point coord){
	//x= rows y= height




}



int main(int argc, char* argv[]){
	cout << "enter your encoding password (Max 16 characters ,It won't be echoed, Press Enter to continue )  ";
	unsigned char password[MAXTOTALBITS/8];
	int i = 0;
	for (i = 0; i < MAXTOTALBITS / 8; i++){
		unsigned char c = getchar();
		if (c == '\n' && i!=0){ break; }
		else if (c == '\n'&&i == 0){ cout << "\n\nPlease enter a password"; i--; continue; }
		password[i] = c;
	}
	
	unsigned long group[(MAXTOTALBITS / 8) / DATATYPESIZE] = {0};
	getGroups(password,i, group);
	Mat image;
	while (1){
		system("cls");
		cout << "\nEnter the name of the image file (with extension) into which the target is to be encoded ";
		String imageName;
		cin >> imageName;
		image = imread(imageName);
		if (image.data == NULL){ cout << "\n\n incorrect image try again. "; system("pause"); }
		else { break; }
	}
	Point coord;
	getCoords(image.rows, image.cols, group, coord);
	system("pause");



}

