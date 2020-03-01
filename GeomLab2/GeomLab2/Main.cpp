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

void srgbEncode(unsigned char & a, size_t depth = 255) {
	double r = double(a) / depth;

	if (r <= 0.0031308) {
		a = depth * (12.92 * r);
	} else {
		a = depth * ((211.0 * pow(r, 5.0 / 12) - 11) / 200);
	}
}

void srgbDecode(unsigned char& a, size_t depth = 255) {
	double r = double(a) / depth;

	if (r <= 0.04045) {
		a = depth * (25 * r / 323);
	}
	else {
		a = depth * pow((200.0 * r + 11.0)/211, 12.0/5);
	}
}

void gammaEncode(unsigned char& a, double gamma = 1.0, size_t depth = 255) {
	a = depth * pow(double(a) / depth, 1.0 / gamma);
}

void gammaDecode(unsigned char& a, double gamma = 1.0, size_t depth = 255) {
	a = depth * pow(double(a) / depth, gamma);
}



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
		if (reverse) {
			gammaDecode(color, gamma, depth);
		} else {
			gammaEncode(color, gamma, depth);
		}
	}

	void srgb(size_t depth = 255, bool reverse = false) {

		if (reverse) {
			srgbDecode(color, depth);
		}
		else {
			srgbEncode(color, depth);
		}

	}

};



void progresser(size_t& previous_step, size_t& percentage, size_t& step) {

	if (percentage >= 100) { return; }

	previous_step = (previous_step + 1) % step;
	if (previous_step == 0) {
		cerr << "\b\b\b\b\b\b\b";
		for (int mmm = 0; mmm < 3; mmm++) {
			if (mmm < percentage % 3) {
				cerr << ".";
			}
			else {
				cerr << " ";
			}
		}
		percentage++;
		cerr << " " << ((percentage < 10) ? " " : "") << percentage << "%";
	}

	if (percentage > 99) {
		cerr << "\n";
	}

}

typedef vector<vector<pnmBWColor*>> pnmMatrix;
typedef enum { P5, P6 } pnmFormat;
typedef vector<char> chars;




class pnmImage {

public:

	int width = 0; // y
	int height = 0; // x
	int depth = 255; // d

	pnmFormat f = P5;
	pnmMatrix m;
	chars errorEncounter;

	set<pnmBWColor*> manuallyEdited;

	pnmImage(size_t w, size_t h, pnmBWColor color = pnmBWColor(255), pnmFormat format = P6) {

		m = pnmMatrix(h, vector<pnmBWColor*>(w, nullptr));

		for (int j = 0; j < h; j++) {
			for (int i = 0; i < w; i++) {
				m[j][i] = new pnmBWColor(color);
			}
		}

		width = w;
		height = h;
		f = format;

	}

	~pnmImage() {

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				delete m[i][j];
			}
		}

	}

	pnmImage(string filename) {

		if (!errorEncounter.empty()) {
			return;
		}

		cerr << "READING FILE " << filename << "\n";

		FILE* file = fopen(filename.c_str(), "rb");
		if ((file != NULL)) {}
		else {
			cerr << "failed\n";
			errorEncounter.push_back(1);
			return;
		}

		char p1, p2 = ' ';
		int w = 0, h = 0, d = 0;

		fscanf(file, "%c%c\n%i %i\n%i\n", &p1, &p2, &w, &h, &d);

		width = w;
		height = h;
		depth = d;

		if (((w <= 0) || (h <= 0))) {
			cerr << "Empty image.";
			errorEncounter.push_back(1);
			return;
		}

		m = pnmMatrix(h, vector<pnmBWColor*>(w, nullptr));

		cerr << "consistency check\n";
		if (p1 != 'P') {
			cerr << "Format Error.";
			errorEncounter.push_back(1);
			return;
		}
		if (!((p2 == '5') )) {
			cerr << "Only P5 is supported.";
			errorEncounter.push_back(1);
			return;
		}
		if (!(d == 255)) {
			cerr << "Depth is not 255.";
			errorEncounter.push_back(1);
			return;
		}
		cerr << "ok\n";


		size_t percentage = 0;
		size_t step = width * height / 100;
		size_t previous_step = 0;
		cerr << "processing...  0%";

		if (p2 == '5') {
			f = P5;

			unsigned char t;
			for (int j = 0; j < h; j++) {
				for (int i = 0; i < w; i++) {

					progresser(previous_step, percentage, step);

					size_t r = fread(&t, UCHAR_SIZE, 1, file);
					if (r != UCHAR_SIZE) {
						cerr << "\nFile end was not reached.";
						errorEncounter.push_back(1);
						return;
					}

					m[j][i] = new pnmBWColor(t);
				}
			}

		}


		fclose(file);

	}

	void print(string filename) {

		if (!errorEncounter.empty()) {
			return;
		}

		cerr << "WRITING FILE " << filename << "\n";

		FILE* file = fopen(filename.c_str(), "wb");
		if ((file != NULL)) {}
		else {
			cerr << "failed\n";
			errorEncounter.push_back(1);
			return;
		}

		switch (f) {
		case P5:
			fprintf(file, "P5\n");
			break;
		}

		fprintf(file, "%i %i\n%i\n", width, height, depth);

		size_t percentage = 0;
		size_t step = width * height / 100;
		size_t previous_step = 0;
		cerr << "processing...  0%";

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				progresser(previous_step, percentage, step);

				if (f == P5) {
					unsigned char t = m[j][i]->color;
					fwrite(&(t), sizeof(unsigned char), 1, file);
				}

			}

		}

		fclose(file);

	}


	
	void correction(double g = -1, bool reverse = false) {

		if (!errorEncounter.empty()) {
			return;
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {

				if (g == -1) {
					m[i][j]->srgb(depth, reverse);
				} else {
					m[i][j]->gamma(g, depth, reverse);
				}

			}
		}


	}




	void drawLine(float x0, float y0, float x1, float y1, int lineColor = 0, float lineWidth = 1) {

		if (!errorEncounter.empty()) {
			return;
		}

		for (double i = lineWidth; i > 0; i--) {
			if (lineWidth <= 1) {
				drawSingularLine(x0, y0, x1, y1, lineColor * lineWidth);
			} else {
				drawSingularLine(x0 /*+ (lineWidth - i)*/, y0 + (lineWidth - i), x1 /*+ (lineWidth - i)*/, y1 + (lineWidth - i), lineColor);
				if (i != lineWidth) {
					bresenham(x0 /*+ (lineWidth - i)*/, y0 + (lineWidth - i - 1), x1 /*+ (lineWidth - i)*/, y1 + (lineWidth - i - 1), lineColor);
				}
			}
		}

	}

	void drawBWPoint(int x, int y, double t, float lineColor = 0) {
		if ((x >= width) || (y >= height) || (x < 0) || (y < 0)) {
			return;
		}
		m[y][x]->setColor(m[y][x]->color + (lineColor - m[y][x]->color) * t);
	}


	void bresenham(int x1, int y1, int x2, int y2, int color) {

		int m_new = 2 * (y2 - y1);
		int slope_error_new = m_new - (x2 - x1);

		for (int x = x1, y = y1; x <= x2; x++){

			drawBWPoint(x, y, 1, color);
			slope_error_new += m_new;

			if (slope_error_new >= 0) {
				y++;
				slope_error_new -= 2 * (x2 - x1);
			}
		}

	}

	int ipart(float x) {
		return int(std::floor(x));
	}

	float round(float x) {
		return std::round(x);
	}

	float fpart(float x) {
		return x - std::floor(x);
	}

	float rfpart(float x) {
		return 1 - fpart(x);
	}


	void drawSingularLine(float x0, float y0, float x1, float y1, int lineColor = 0) {

		int lineWidth = 1;
		//lineColor = 255 - lineColor;

		const bool steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			std::swap(x0, y0);
			std::swap(x1, y1);
		}
		if (x0 > x1) {
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		const float dx = x1 - x0;
		const float dy = y1 - y0;
		const float gradient = (dx == 0) ? 1 : dy / dx;

		int xpx11;
		float intery;
		{

			const float xend = round(x0);
			const float yend = y0 + gradient * (xend - x0);
			const float xgap = rfpart(x0 + 0.5);

			const int ypx11 = ipart(yend);
			xpx11 = int(xend);

			if (steep) {
				drawBWPoint(ypx11, xpx11, rfpart(yend) * xgap, lineColor);
				drawBWPoint(ypx11 + 1, xpx11, fpart(yend) * xgap, lineColor);
			}
			else {
				drawBWPoint(xpx11, ypx11, rfpart(yend) * xgap, lineColor);
				drawBWPoint(xpx11, ypx11 + 1, fpart(yend) * xgap, lineColor);
			}

			intery = yend + gradient;
		}

		int xpx12;
		{
			const float xend = round(x1);
			const float yend = y1 + gradient * (xend - x1);
			const float xgap = rfpart(x1 + 0.5);
			xpx12 = int(xend);
			const int ypx12 = ipart(yend);
			if (steep) {
				drawBWPoint(ypx12, xpx12, rfpart(yend) * xgap, lineColor);
				drawBWPoint(ypx12 + 1, xpx12, fpart(yend) * xgap, lineColor);
			}
			else {
				drawBWPoint(xpx12, ypx12, rfpart(yend) * xgap, lineColor);
				drawBWPoint(xpx12, ypx12 + 1, fpart(yend) * xgap, lineColor);
			}
		}

		if (steep) {
			for (int x = xpx11 + 1; x < xpx12; x++) {
				drawBWPoint(ipart(intery), x, rfpart(intery), lineColor);
				drawBWPoint(ipart(intery) + 1, x, fpart(intery), lineColor);
				intery += gradient;
			}
		}
		else {
			for (int x = xpx11 + 1; x < xpx12; x++) {
				drawBWPoint(x, ipart(intery), rfpart(intery), lineColor);
				drawBWPoint(x, ipart(intery) + 1, fpart(intery), lineColor);
				intery += gradient;
			}
		}

	}




};












int main(int argc, char* argv[]) {

	string fn, in, out;
	int bw_color = 255;
	float width;
	float x0, y0, x1, y1;
	float gamma = -1;


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
			bw_color = atof(argv[i]);
		}
		if (i == 4) {
			width = atof(argv[i]);
		}
		if (i == 5) {
			x0 = atof(argv[i]) - 1;
		}
		if (i == 6) {
			y0 = atof(argv[i]) - 1;
		}
		if (i == 7) {
			x1 = atof(argv[i]) - 1;
		}
		if (i == 8) {
			y1 = atof(argv[i]) - 1;
		}
		if (i == 9) {
			gamma = atof(argv[i]);
		}
	}


	pnmImage im(in);
	im.correction(gamma, false);
	im.drawLine(x0, y0, x1, y1, bw_color, width);
	im.correction(gamma, true);
	im.print(out);

	cerr << "\nCLEANING MEMORY\n";

	if (!im.errorEncounter.empty()) {
		return 1;
	}

	return 0;

}
