#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
using namespace std;

#define width 512
#define height 512
#define x 5
#define y 5
#define sig 1
#define pi 3.14

//gaussian low pass filter
void GaussianMask(double**filter) {
	double sum = 0;
	//2d gaussian filter
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			filter[i][j] = exp(-(pow((j - x / 2), 2) + pow((i - y / 2), 2)) / (2 * sig * sig)) / (2 * pi * sig * sig);
			sum += filter[i][j];
		}
	}
	//filter value revise sum=1
	double cor = 1 / sum;
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			filter[i][j] *= cor;
		}
	}
	
}
//image downsampling
void DownSampling(unsigned char** r, unsigned char** g, unsigned char** b, unsigned char** rD, unsigned char** gD, unsigned char** bD) {
	//256*256 downsampling
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			rD[i][j] = r[2 * i + 1][2 * j + 1];
			gD[i][j] = r[2 * i + 1][2 * j + 1];
			bD[i][j] = r[2 * i + 1][2 * j + 1];
		}
	}
}
//image upsampling
void UpSampling(unsigned char** r, unsigned char** g, unsigned char** b, unsigned char** rU, unsigned char** gU, unsigned char** bU) {
	//temp array for padding
	unsigned char** rTemp = new unsigned char* [height + 9];
	unsigned char** gTemp = new unsigned char* [height + 9];
	unsigned char** bTemp = new unsigned char* [height + 9];
	for (int i = 0; i < height + 9; i++) {
		rTemp[i] = new unsigned char[width + 9]{ 0, };
		gTemp[i] = new unsigned char[width + 9]{ 0, };
		bTemp[i] = new unsigned char[width + 9]{ 0, };
	}
	//integer-pel
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			rTemp[2 * i + 6][2 * j + 6] = r[i][j];
			gTemp[2 * i + 6][2 * j + 6] = g[i][j];
			bTemp[2 * i + 6][2 * j + 6] = b[i][j];
		}
	}
	//horizontal padding
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < 3; j ++) {
			rTemp[2 * i + 6][2 * j] = rTemp[2 * i + 6][6];
			gTemp[2 * i + 6][2 * j] = gTemp[2 * i + 6][6];
			bTemp[2 * i + 6][2 * j] = bTemp[2 * i + 6][6];
		}
		for (int j = 0; j < 2; j++) {
			rTemp[2 * i + 6][width + 6 + 2 * j] = rTemp[2 * i + 6][width + 4];
			gTemp[2 * i + 6][width + 6 + 2 * j] = gTemp[2 * i + 6][width + 4];
			bTemp[2 * i + 6][width + 6 + 2 * j] = bTemp[2 * i + 6][width + 4];
		}
	}
	//vertical padding
	for (int i = 0; i < width / 2; i++) {
		for (int j = 0; j < 3; j++) {
			rTemp[2 * j][2 * i + 6] = rTemp[6][2 * i + 6];
			gTemp[2 * j][2 * i + 6] = gTemp[6][2 * i + 6];
			bTemp[2 * j][2 * i + 6] = bTemp[6][2 * i + 6];
		}
		for (int j = 0; j < 2; j++) {
			rTemp[height + 6 + 2 * j][2 * i + 6] = rTemp[height + 4][2 * i + 6];
			gTemp[height + 6 + 2 * j][2 * i + 6] = gTemp[height + 4][2 * i + 6];
			bTemp[height + 6 + 2 * j][2 * i + 6] = bTemp[height + 4][2 * i + 6];
		}
	}
	//horizontal half-pel
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			rTemp[2 * i + 6][2 * j + 5] = (int)((11 * (rTemp[2 * i + 6][2 * j] + rTemp[2 * i + 6][2 * j + 10]) - 43 * (rTemp[2 * i + 6][2 * j + 2] + rTemp[2 * i + 6][2 * j + 8]) + 160 * (rTemp[2 * i + 6][2 * j + 4] + rTemp[2 * i + 6][2 * j + 6])) / 256);
			gTemp[2 * i + 6][2 * j + 5] = (int)((11 * (gTemp[2 * i + 6][2 * j] + gTemp[2 * i + 6][2 * j + 10]) - 43 * (gTemp[2 * i + 6][2 * j + 2] + gTemp[2 * i + 6][2 * j + 8]) + 160 * (gTemp[2 * i + 6][2 * j + 4] + gTemp[2 * i + 6][2 * j + 6])) / 256);
			bTemp[2 * i + 6][2 * j + 5] = (int)((11 * (bTemp[2 * i + 6][2 * j] + bTemp[2 * i + 6][2 * j + 10]) - 43 * (bTemp[2 * i + 6][2 * j + 2] + bTemp[2 * i + 6][2 * j + 8]) + 160 * (bTemp[2 * i + 6][2 * j + 4] + bTemp[2 * i + 6][2 * j + 6])) / 256);
		}
	}
	//vertical half-pel
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			rTemp[2 * i + 5][2 * j + 6] = (int)((11 * (rTemp[2 * i][2 * j + 6] + rTemp[2 * i + 10][2 * j + 6]) - 43 * (rTemp[2 * i + 2][2 * j + 6] + rTemp[2 * i + 8][2 * j + 6]) + 160 * (rTemp[2 * i + 4][2 * j + 6] + rTemp[2 * i + 6][2 * j + 6])) / 256);
			gTemp[2 * i + 5][2 * j + 6] = (int)((11 * (gTemp[2 * i][2 * j + 6] + gTemp[2 * i + 10][2 * j + 6]) - 43 * (gTemp[2 * i + 2][2 * j + 6] + gTemp[2 * i + 8][2 * j + 6]) + 160 * (gTemp[2 * i + 4][2 * j + 6] + gTemp[2 * i + 6][2 * j + 6])) / 256);
			bTemp[2 * i + 5][2 * j + 6] = (int)((11 * (bTemp[2 * i][2 * j + 6] + bTemp[2 * i + 10][2 * j + 6]) - 43 * (bTemp[2 * i + 2][2 * j + 6] + bTemp[2 * i + 8][2 * j + 6]) + 160 * (bTemp[2 * i + 4][2 * j + 6] + bTemp[2 * i + 6][2 * j + 6])) / 256);
		}
	}
	//diagonal half-pel horizontal padding
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < 3; j++) {
			rTemp[2 * i + 5][2 * j] = rTemp[2 * i + 5][6];
			gTemp[2 * i + 5][2 * j] = gTemp[2 * i + 5][6];
			bTemp[2 * i + 5][2 * j] = bTemp[2 * i + 5][6];
		}
		for (int j = 0; j < 2; j++) {
			rTemp[2 * i + 5][width + 6 + 2 * j] = rTemp[2 * i + 5][width + 4];
			gTemp[2 * i + 5][width + 6 + 2 * j] = gTemp[2 * i + 5][width + 4];
			bTemp[2 * i + 5][width + 6 + 2 * j] = bTemp[2 * i + 5][width + 4];
		}
	}
	//diagonal half-pel(horizontal)
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			rTemp[2 * i + 5][2 * j + 5] = (int)((11 * (rTemp[2 * i + 5][2 * j] + rTemp[2 * i + 5][2 * j + 10]) - 43 * (rTemp[2 * i + 5][2 * j + 2] + rTemp[2 * i + 5][2 * j + 8]) + 160 * (rTemp[2 * i + 5][2 * j + 4] + rTemp[2 * i + 5][2 * j + 6])) / 256);
			gTemp[2 * i + 5][2 * j + 5] = (int)((11 * (gTemp[2 * i + 5][2 * j] + gTemp[2 * i + 5][2 * j + 10]) - 43 * (gTemp[2 * i + 5][2 * j + 2] + gTemp[2 * i + 5][2 * j + 8]) + 160 * (gTemp[2 * i + 5][2 * j + 4] + gTemp[2 * i + 5][2 * j + 6])) / 256);
			bTemp[2 * i + 5][2 * j + 5] = (int)((11 * (bTemp[2 * i + 5][2 * j] + bTemp[2 * i + 5][2 * j + 10]) - 43 * (bTemp[2 * i + 5][2 * j + 2] + bTemp[2 * i + 5][2 * j + 8]) + 160 * (bTemp[2 * i + 5][2 * j + 4] + bTemp[2 * i + 5][2 * j + 6])) / 256);
		}
	}
	//result value copy(512*512)
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			rU[i][j] = rTemp[i + 5][j + 5];
			gU[i][j] = gTemp[i + 5][j + 5];
			bU[i][j] = bTemp[i + 5][j + 5];
		}
	}
	//free memory
	for (int i = 0; i < height + 9; i++) {
		delete[] rTemp[i];
		delete[] gTemp[i];
		delete[] bTemp[i];
	}
}
//anti-aliasing
void AntiAliasing(unsigned char** r, unsigned char** g, unsigned char** b, unsigned char** rR, unsigned char** gR, unsigned char** bR, double** filter) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			double rSum = 0, gSum = 0, bSum = 0;
			//convolution gaussian filter
			for (int k = 0; k < y; k++) {
				for (int l = 0; l < x; l++) {
					rSum += r[i + k][j + l] * filter[k][l];
					gSum += g[i + k][j + l] * filter[k][l];
					bSum += b[i + k][j + l] * filter[k][l];
				}
			}
			rR[i][j] = (int)rSum;
			gR[i][j] = (int)gSum;
			bR[i][j] = (int)bSum;
		}
	}
}

void Padding(unsigned char** r, unsigned char** g, unsigned char** b) {
	//upper copy padding
	for (int i = 0; i < y / 2; i++) {
		int j = 0;
		for (j; j < x / 2; j++) {
			r[i][j] = r[y / 2][x / 2];
			g[i][j] = g[y / 2][x / 2];
			b[i][j] = b[y / 2][x / 2];
		}
		for (j; j < x / 2 + width; j++) {
			r[i][j] = r[y / 2][j];
			g[i][j] = g[y / 2][j];
			b[i][j] = b[y / 2][j];
		}
		for (j; j < x + width - 1; j++) {
			r[i][j] = r[y / 2][x / 2 + width - 1];
			g[i][j] = g[y / 2][x / 2 + width - 1];
			b[i][j] = b[y / 2][x / 2 + width - 1];
		}
	}
	//left right copy padding
	for (int i = y / 2; i < y / 2 + height; i++) {
		//left
		for (int j = 0; j < x / 2; j++) {
			r[i][j] = r[i][x / 2];
			g[i][j] = g[i][x / 2];
			b[i][j] = b[i][x / 2];
		}
		//right
		for (int j = x / 2 + width; j < x + width - 1; j++) {
			r[i][j] = r[i][x / 2 + width - 1];
			g[i][j] = g[i][x / 2 + width - 1];
			b[i][j] = b[i][x / 2 + width - 1];
		}
	}
	//down copy padding
	for (int i = y / 2 + height; i < y + height - 1; i++) {
		int j = 0;
		for (j; j < x / 2; j++) {
			r[i][j] = r[y / 2 + height-1][x / 2];
			g[i][j] = g[y / 2 + height-1][x / 2];
			b[i][j] = b[y / 2 + height-1][x / 2];
		}
		for (j; j < x / 2 + width; j++) {
			r[i][j] = r[y / 2 + height-1][j];
			g[i][j] = g[y / 2 + height-1][j];
			b[i][j] = b[y / 2 + height-1][j];
		}
		for (j; j < x + width - 1; j++) {
			r[i][j] = r[y / 2 + height-1][x / 2 + width - 1];
			g[i][j] = g[y / 2 + height-1][x / 2 + width - 1];
			b[i][j] = b[y / 2 + height-1][x / 2 + width - 1];
		}
	}
}
//image print
void print(unsigned char** r, unsigned char** g, unsigned char** b,int opt) {
	ofstream fout;
	int pWidth, pHeight;
	
	if (opt == 1) {//option 1->256*256 with anti-aliasing
		fout.open("./output/downsample_image_with_Anti_aliasing_filter.raw", ios::binary);
		pWidth = width / 2;
		pHeight = height / 2;
	}
	else if (opt == 2) {//option 2->256*256 without anti-aliasing
		fout.open("./output/downsample_image_without_Anti_aliasing_filter.raw", ios::binary);
		pWidth = width / 2;
		pHeight = height / 2;
	}
	else if (opt == 3) {//option 3->512*512 with anti-aliasing
		fout.open("./output/reconstructed_image_with_Anti_aliasing_filter.raw", ios::binary);
		pWidth = width;
		pHeight = height;
	}
	else if (opt == 4) {//option 4->512*512 without anti-aliasing
		fout.open("./output/reconstructed_image_without_Anti_aliasing_filter.raw", ios::binary);
		pWidth = width;
		pHeight = height;
	}
	unsigned char* wBuffer = new unsigned char[pWidth * pHeight * 3];
	//make write buffer
	for (int i = 0; i < pHeight; i++) {
		for (int j = 0; j < pWidth; j++) {
			int index = 3 * pHeight * i + 3 * j;
			wBuffer[index] = r[i][j];
			wBuffer[index+1] = g[i][j];
			wBuffer[index+2] = b[i][j];
		}
	}
	//write image file
	fout.write((char*)wBuffer, 3 * pHeight * pWidth);
	delete[] wBuffer;
	fout.close();
}

int main() {
	ifstream fin;
	fin.open("./input/lena_color_512.raw", ios::binary);//open file
	
	if (fin) {
		fin.seekg(0, fin.end);
		int length = fin.tellg();//total rgb number
		fin.seekg(0, fin.beg);

		unsigned char* buffer = new unsigned char[length];//read buffer
		fin.read((char*)buffer, length);
		fin.close();

		//antialiasing padding buffer
		unsigned char** rBufferAnti = new unsigned char* [height + y - 1];
		unsigned char** gBufferAnti = new unsigned char* [height + y - 1];
		unsigned char** bBufferAnti = new unsigned char* [height + y - 1];
		
		for (int i = 0; i < height + y - 1; i++) {
			rBufferAnti[i] = new unsigned char[width + x - 1];
			gBufferAnti[i] = new unsigned char[width + x - 1];
			bBufferAnti[i] = new unsigned char[width + x - 1];
		}

		//antialiasing result buffer
		unsigned char** rBufferAntiResult = new unsigned char* [height + y - 1];
		unsigned char** gBufferAntiResult = new unsigned char* [height + y - 1];
		unsigned char** bBufferAntiResult = new unsigned char* [height + y - 1];

		for (int i = 0; i < height; i++) {
			rBufferAntiResult[i] = new unsigned char[width];
			gBufferAntiResult[i] = new unsigned char[width];
			bBufferAntiResult[i] = new unsigned char[width];
		}
		//initialize
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int index = i * width + j;
				rBufferAnti[i + (y / 2)][j + (x / 2)] = (int)(buffer[3 * index]);
				gBufferAnti[i + (y / 2)][j + (x / 2)] = (int)(buffer[3 * index + 1]);
				bBufferAnti[i + (y / 2)][j + (x / 2)] = (int)(buffer[3 * index + 2]);
			}
		}
		//gaussian filter
		double** filter = new double* [y];
		for (int i = 0; i < y; i++) {
			filter[i] = new double[x];
		}
		//set gaussian filter
		GaussianMask(filter);
		//buffer padding
		Padding(rBufferAnti, gBufferAnti, bBufferAnti);
		//Anti-aliasing filter
		AntiAliasing(rBufferAnti, gBufferAnti, bBufferAnti, rBufferAntiResult, gBufferAntiResult, bBufferAntiResult, filter);
		//free memory
		for (int i = 0; i < height + y - 1; i++) {
			delete[] rBufferAnti[i];
			delete[] gBufferAnti[i];
			delete[] bBufferAnti[i];
		}
		//down sampling buffer
		unsigned char** rBufferDown = new unsigned char* [height / 2];
		unsigned char** gBufferDown = new unsigned char* [height / 2];
		unsigned char** bBufferDown = new unsigned char* [height / 2];

		for (int i = 0; i < height / 2; i++) {
			rBufferDown[i] = new unsigned char[width / 2];
			gBufferDown[i] = new unsigned char[width / 2];
			bBufferDown[i] = new unsigned char[width / 2];
		}
		
		//downsamping image with anti-aliasing
		DownSampling(rBufferAntiResult, gBufferAntiResult, bBufferAntiResult, rBufferDown, gBufferDown, bBufferDown);
		for (int i = 0; i < height; i++) {
			delete[] rBufferAntiResult[i];
			delete[] gBufferAntiResult[i];
			delete[] bBufferAntiResult[i];
		}
		//print downsampling image
		print(rBufferDown, gBufferDown, bBufferDown, 1);
		
		
		//upsampling buffer
		unsigned char** rBufferUp = new unsigned char* [height];
		unsigned char** gBufferUp = new unsigned char* [height];
		unsigned char** bBufferUp = new unsigned char* [height];

		for (int i = 0; i < height; i++) {
			rBufferUp[i] = new unsigned char[width];
			gBufferUp[i] = new unsigned char[width];
			bBufferUp[i] = new unsigned char[width];
		}
		//upsampling image with anti-aliasing
		UpSampling(rBufferDown, gBufferDown, bBufferDown, rBufferUp, gBufferUp, bBufferUp);
		//print upsampling image
		print(rBufferUp, gBufferUp, bBufferUp,3);
		
		//get MSE
		double MSE=0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				MSE += pow((buffer[width * i + 3 * j] - rBufferUp[i][j]), 2);
				MSE += pow((buffer[width * i + 3 * j + 1] - gBufferUp[i][j]), 2);
				MSE += pow((buffer[width * i + 3 * j + 2] - bBufferUp[i][j]), 2);
			}
		}
		printf("MSE reconstructed image with anti-aliasing filter : %f\n", MSE / (height * width * 3));

		//non antialiasing buffer
		unsigned char** rBuffer = new unsigned char* [height];
		unsigned char** gBuffer = new unsigned char* [height];
		unsigned char** bBuffer = new unsigned char* [height];
		for (int i = 0; i < height; i++) {
			rBuffer[i] = new unsigned char[width];
			gBuffer[i] = new unsigned char[width];
			bBuffer[i] = new unsigned char[width];
		}
		//initialize
		fin.read((char*)buffer, length);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int index = i * width + j;
				rBuffer[i][j] = (int)buffer[3 * index];
				gBuffer[i][j] = (int)buffer[3 * index + 1];
				bBuffer[i][j] = (int)buffer[3 * index + 2];
			}
		}
		//downsampling without anti-aliasing
		DownSampling(rBuffer, gBuffer, bBuffer, rBufferDown, gBufferDown, bBufferDown);
		for (int i = 0; i < height; i++) {
			delete[] rBuffer[i];
			delete[] gBuffer[i];
			delete[] bBuffer[i];
		}
		//print downsampling image
		print(rBufferDown, gBufferDown, bBufferDown, 2);
		//upsampling without anti-aliasing
		UpSampling(rBufferDown, gBufferDown, bBufferDown, rBufferUp, gBufferUp, bBufferUp);
		//print upsampling image
		print(rBufferUp, gBufferUp, bBufferUp, 4);
		
		//get MSE
		MSE = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				MSE += pow((buffer[width * i + 3 * j] - rBufferUp[i][j]), 2);
				MSE += pow((buffer[width * i + 3 * j + 1] - gBufferUp[i][j]), 2);
				MSE += pow((buffer[width * i + 3 * j + 2] - bBufferUp[i][j]), 2);
			}
		}
		printf("MSE reconstructed image without anti-aliasing filter : %f\n", MSE / (height * width * 3));
		
		//free memory
		delete[] buffer;

		for (int i = 0; i < height/2; i++) {
			delete[] rBufferDown[i];
			delete[] gBufferDown[i];
			delete[] bBufferDown[i];
		}
		for (int i = 0; i < height; i++) {
			delete[] rBufferUp[i];
			delete[] gBufferUp[i];
			delete[] bBufferUp[i];
		}

	}
}