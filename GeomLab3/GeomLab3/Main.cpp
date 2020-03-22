#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include <iostream>
#include <fstream>
#include <istream>
#include <iomanip>

#include<string> 
#include<vector> 
#include<set> 
#include<queue> 


#include <functional>
#include <algorithm>
#include <utility>


using namespace std;

const int UCHAR_SIZE = sizeof(unsigned char);

class pnmBWColor {

public:

	unsigned char color = 0;

	pnmBWColor() {

	}

	pnmBWColor(const pnmBWColor& c) {
		color = c.color;
	}

	pnmBWColor(unsigned char black) {
		color = black;
	}

	~pnmBWColor() {

	}

	void setColor(unsigned char r) {
		color = r;
	}

	void inverseColor(unsigned char depth = 255) {
		color = depth - color;
	}

	
	void gamma(double gamma = 1, size_t depth = 255, bool reverse = false) {
		if (!reverse) {
			gammaDecode(color, gamma, depth);
		} else {
			gammaEncode(color, gamma, depth);
		}
	}

	void srgb(size_t depth = 255, bool reverse = false) {

		if (!reverse) {
			srgbDecode(color, depth);
		} else {
			srgbEncode(color, depth);
		}

	}

	static void gammaEncode(unsigned char& a, double gamma = 1.0, size_t depth = 255) {
		a = depth * pow(double(a) / depth, 1.0 / gamma);
	}

	static void gammaDecode(unsigned char& a, double gamma = 1.0, size_t depth = 255) {
		a = depth * pow(double(a) / depth, gamma);
	}

	static void srgbEncode(unsigned char& a, size_t depth = 255) {
		double r = double(a) / depth;

		if (r <= 0.0031308) {
			a = depth * (12.92 * r);
		}
		else {
			a = depth * ((211.0 * pow(r, 5.0 / 12) - 11) / 200);
		}
	}

	static void srgbDecode(unsigned char& a, size_t depth = 255) {
		double r = double(a) / depth;

		if (r <= 0.04045) {
			a = depth * (25 * r / 323);
		}
		else {
			a = depth * pow((200.0 * r + 11.0) / 211, 12.0 / 5);
		}
	}


};




typedef vector<vector<int>> intMatrix;
typedef vector<vector<pnmBWColor*>> pnmMatrix;
typedef vector<char> chars;




class pnmBWImage {

public:

	int width = 0; // y
	int height = 0; // x
	int depth = 255; // d
	int gamma = -1; // gamma

	pnmMatrix m;
	chars errorEncounter;

	pnmMatrix copy;


	pnmBWImage(size_t w, size_t h, pnmBWColor color = pnmBWColor(255)) {

		m = pnmMatrix(h, vector<pnmBWColor*>(w, nullptr));

		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				m[j][i] = new pnmBWColor(color);
			}
		}

		width = w;
		height = h;

	}

	~pnmBWImage() {

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				delete m[i][j];
			}
		}

	}

	pnmBWImage(string filename, int g = -1) {

		if (!errorEncounter.empty()) {
			return;
		}

		FILE* file = fopen(filename.c_str(), "rb");
		if (!(file != NULL)) {
			errorEncounter.push_back(1);
			return;
		}

		char p1, p2 = ' ';
		int w = 0, h = 0, d = 0;

		fscanf(file, "%c%c\n%i %i\n%i\n", &p1, &p2, &w, &h, &d);

		width = w;
		height = h;
		depth = d;
		gamma = g;

		if (((w <= 0) || (h <= 0))) {
			errorEncounter.push_back(1);
			return;
		}

		m = pnmMatrix(h, vector<pnmBWColor*>(w, nullptr));

		if (p1 != 'P') {
			errorEncounter.push_back(1);
			return;
		}
		if (!((p2 == '5'))) {
			errorEncounter.push_back(1);
			return;
		}
		if (!(d == 255)) {
			errorEncounter.push_back(1);
			return;
		}

		unsigned char t;
		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				size_t r = fread(&t, UCHAR_SIZE, 1, file);
				if (r != UCHAR_SIZE) {
					errorEncounter.push_back(1);
					return;
				}
				m[j][i] = new pnmBWColor(t);
			}
		}

		fclose(file);

		correction(gamma, false);

	}

	void print(string filename) {

		if (!errorEncounter.empty()) {
			return;
		}

		correction(gamma, true);

		FILE* file = fopen(filename.c_str(), "wb");
		if (!(file != NULL)) {
			errorEncounter.push_back(1);
			return;
		}

		fprintf(file, "P5\n");
		fprintf(file, "%i %i\n%i\n", width, height, depth);

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				unsigned char t = m[j][i]->color;
				fwrite(&(t), sizeof(unsigned char), 1, file);

			}
		}

		fclose(file);

	}


	unsigned char colorToCurrentScheme(unsigned char c) {
		pnmBWColor color(c);
		if (gamma == -1) {
			color.srgb(depth, true);
		}
		else {
			color.gamma(gamma, depth, true);
		}
		return color.color;
	}

protected:
	void correction(double g = -1, bool reverse = false) {

		if (!errorEncounter.empty()) {
			return;
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (g == -1) {
					m[i][j]->srgb(depth, reverse);
				}
				else {
					m[i][j]->gamma(g, depth, reverse);
				}
			}
		}

	}


public:

	void horizontalGradient() {
		for (int i = 0; i < width; i++) {
			unsigned char c = round(255 * i / width);
			for (int j = 0; j < height; j++) {
				m[j][i]->setColor(c);
			}
		}
	}

	void copyColorPlus(int i, int j, int value) {
		if ((0 <= j) && (j < width)) {
			if ((0 <= i) && (i < height)) {
				copy[i][j]->color += value;
			}
		}
	}

	int bit = 8;

	unsigned char nextColor(unsigned char color) {
		return min(color + pow(2, 8 - bit), 255.0);
	}

	unsigned char prevColor(unsigned char color) {
		return max(color - pow(2, 8 - bit), 0.0);
	}


	void dither(int mode = 0, int b = 8) {

		if (!errorEncounter.empty()) {
			return;
		}

		bit = b;

		copy = pnmMatrix(height, vector<pnmBWColor*>(width));
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				//cout << int(m[i][j]->color) << " ";
				m[i][j]->color = 255 * round((m[i][j]->color) * float(float(pow(2, bit) - 1) / 255.0)) / (pow(2, bit) - 1);
				//cout << int(m[i][j]->color) << "\n";
				copy[i][j] = new pnmBWColor(m[i][j]->color);
			}
		}

		if (mode == 1) {
			ditherOrdered8x8();
		}

		if (mode == 2) {
			ditherRandom();
		}

		if (mode == 3) {
			ditherFloydSteinberg();
		}

		if (mode == 4) {
			ditherJarvisJudiceNinke();
		}

		if (mode == 5) {
			ditherSierra();
		}

		if (mode == 6) {
			ditherAtkinson();
		}

		if (mode == 7) {
			ditherHalftone();
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				m[i][j]->color = copy[i][j]->color;
				delete copy[i][j];
			}
		}

	}

	void ditherOrdered8x8() {

		const int order = 8;

		intMatrix pattern = {
			{0, 194, 48, 242, 12, 206, 60, 255},
			{129, 64, 178, 113, 141, 76, 190, 125},
			{32, 226, 16, 210, 44, 238, 28, 222},
			{161, 97, 145, 80, 174, 109, 157, 93},
			{8, 202, 56, 250, 4, 198, 52, 246},
			{137, 72, 186, 121, 133, 68, 182, 117},
			{40, 234, 24, 218, 36, 230, 20, 214},
			{170, 105, 153, 89, 165, 101, 149, 85},
		};

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int threshold = pattern[i % order][j % order];
				if (m[j][i]->color > threshold) {
					copy[j][i]->setColor(nextColor(copy[j][i]->color));
				}
				else {
					copy[j][i]->setColor(prevColor(copy[j][i]->color));
				}
			}
		}

	}

	void ditherRandom() {

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int threshold = rand() % 256;
				if (m[j][i]->color > threshold) {
					copy[j][i]->setColor(nextColor(copy[j][i]->color));
				}
				else {
					copy[j][i]->setColor(prevColor(copy[j][i]->color));
				}
			}
		}

	}

	void ditherFloydSteinberg() {

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				//unsigned char c = m[j][i]->color;
				//copy[j][i]->color = round(c / 256);
				//int quant_error = c - copy[j][i]->color;

				unsigned char c = m[j][i]->color;
				copy[j][i]->color = round(m[j][i]->color / 256);
				int quant_error = m[j][i]->color - round(m[j][i]->color / 256);

				copyColorPlus(j, i + 1, quant_error * 7 / 16);
				copyColorPlus(j + 1, i, quant_error * 5 / 16);
				copyColorPlus(j + 1, i + 1, quant_error * 1 / 16);
				copyColorPlus(j + 1, i - 1, quant_error * 3 / 16);

			}
		}

	}

	void ditherJarvisJudiceNinke() {

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				copy[j][i]->color = round(m[j][i]->color / 256);
				int quant_error = m[j][i]->color - round(m[j][i]->color / 256);

				copyColorPlus(j, i + 1, quant_error * 7 / 48);
				copyColorPlus(j, i + 2, quant_error * 5 / 48);
				copyColorPlus(j - 1, i + 1, quant_error * 5 / 48);
				copyColorPlus(j - 1, i + 2, quant_error * 3 / 48);
				copyColorPlus(j - 2, i + 1, quant_error * 3 / 48);
				copyColorPlus(j - 2, i + 2, quant_error * 1 / 48);
				copyColorPlus(j - 1, i, quant_error * 7 / 48);
				copyColorPlus(j - 2, i, quant_error * 5 / 48);
				copyColorPlus(j - 1, i - 1, quant_error * 5 / 48);
				copyColorPlus(j - 1, i - 2, quant_error * 3 / 48);
				copyColorPlus(j - 2, i - 1, quant_error * 3 / 48);
				copyColorPlus(j - 2, i - 2, quant_error * 1 / 48);

			}
		}

	}

	void ditherSierra() {

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				copy[j][i]->color = round(m[j][i]->color / 256);
				int quant_error = m[j][i]->color - round(m[j][i]->color / 256);

				copyColorPlus(j, i + 1, quant_error * 5 / 32);
				copyColorPlus(j, i + 2, quant_error * 3 / 32);
				copyColorPlus(j - 1, i + 1, quant_error * 1 / 8);
				copyColorPlus(j - 1, i + 2, quant_error * 1 / 16);
				copyColorPlus(j - 2, i + 1, quant_error * 1 / 16);
				copyColorPlus(j - 1, i, quant_error * 5 / 32);
				copyColorPlus(j - 2, i, quant_error * 3 / 32);
				copyColorPlus(j - 1, i - 1, quant_error * 1 / 8);
				copyColorPlus(j - 1, i - 2, quant_error * 1 / 16);
				copyColorPlus(j - 2, i - 1, quant_error * 1 / 16);

			}
		}

	}

	void ditherAtkinson() {

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {

				copy[j][i]->color = round(m[j][i]->color / 256);
				int quant_error = m[j][i]->color - round(m[j][i]->color / 256);

				copyColorPlus(j, i + 1, quant_error * 1 / 8);
				copyColorPlus(j, i + 2, quant_error * 1 / 8);
				copyColorPlus(j - 1, i + 1, quant_error * 1 / 8);
				copyColorPlus(j - 1, i, quant_error * 1 / 8);
				copyColorPlus(j - 2, i, quant_error * 1 / 8);
				copyColorPlus(j - 1, i - 1, quant_error * 1 / 8);

			}
		}

	}

	
	void ditherHalftone() {

		const int order = 4;

		intMatrix pattern = {
				{7 , 13,  11,   4},
				{ 12,  16,  14,   8 },
				{10,  15,   6,   2},
				{5,   9,   3,   1}
			};

		for (auto &v : pattern) {
			for (auto &u : v) {
				u = u * 256 / 17;
			}
		}


			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					int threshold = pattern[i % order][j % order];
					if (m[j][i]->color > threshold) {
						copy[j][i]->setColor(nextColor(copy[j][i]->color));
					}
					else {
						copy[j][i]->setColor(prevColor(copy[j][i]->color));
					}
				}
			}


		}



};












int main(int argc, char* argv[]) {

	string fn = "", in = "lena512.pgm", out = "lena.pgm";

	float gamma = -1;
	int gradient = 0;
	int mode = 0;
	int bit = 8;

	for (int i = 0; i < argc; i++) {
		if (i == 0) {
			fn = argv[i];
		}
		if (i == 1) {
			in = argv[i];
		}
		if (i == 2) {
			out = argv[i];
		}
		if (i == 3) {
			gradient = atoi(argv[i]);
		}
		if (i == 4) {
			mode = atof(argv[i]);
		}
		if (i == 5) {
			bit = atof(argv[i]);
		}
		if (i == 6) {
			gamma = atof(argv[i]);
		}
	}

	pnmBWImage im(in, gamma);
	if (gradient == 1) {
		im.horizontalGradient();
	}
	im.dither(mode, bit);
	im.print(out);

	if (!im.errorEncounter.empty()) {
		cerr << "Some errors encountered.";
		return 1;
	}

	// Testing all modes and bits
	//for (int i = 1; i <= 7; i++) {

	//	for (int j = 1; j <= 8; j++) {
	//		pnmBWImage im(in, gamma);
	//		im.dither(i, j);
	//		im.print("pics/" + to_string(i) + "_bit" + to_string(j) + "_" + out);

	//		if (!im.errorEncounter.empty()) {
	//			cerr << "Some errors encountered.";
	//			return 1;
	//		}
	//	}

	//}

	return 0;

}
