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


#include <iostream>
#include <cmath>
#include <cfenv>

#pragma STDC FENV_ACCESS ON



using namespace std;

const int UCHAR_SIZE = sizeof(unsigned char);

class baseColor {

public:

	double red = 1; // 0 ... 1
	double green = 1; // 0 ... 1
	double blue = 1; // 0 ... 1

	baseColor() {

	}
	
	baseColor(baseColor * c) {
		red = c->red;
		green = c->green;
		blue = c->blue;
	}

	baseColor(double r, double g, double b) : red(r), green(g), blue(b) {

	}

	static float HueToRGB(float v1, float v2, float vH) {
		if (vH < 0)
			vH += 1;

		if (vH > 1)
			vH -= 1;

		if ((6 * vH) < 1)
			return (v1 + (v2 - v1) * 6 * vH);

		if ((2 * vH) < 1)
			return v2;

		if ((3 * vH) < 2)
			return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

		return v1;
	}

	void fromHSLToRGB() {

		double H = this->red  * 360.0;
		double S = this->green ;
		double L = this->blue ;

		double r = 0;
		double g = 0;
		double b = 0;

		if (S == 0){
			r = g = b = (L * 255.0);
		} else {
			double v1, v2;
			double hue = (float)H / 360.0;

			v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
			v1 = 2 * L - v2;

			r = HueToRGB(v1, v2, hue + (1.0f / 3));
			g = HueToRGB(v1, v2, hue);
			b = HueToRGB(v1, v2, hue - (1.0f / 3));
		}

		this->red = r ;
		this->green = g ;
		this->blue = b;

	}

	void fromRGBToHSL() {

		double r = this->red ;
		double g = this->green;
		double b = this->blue;

		double max_c = max(r, max(g,b));
		double min_c = min(r, min(g, b));

		double h, s, l = (max_c + min_c) / 2;

		if (max_c == min_c) {
			h = 0;
			s = 0; // achromatic
		} else {
			double d = max_c - min_c;
			s = (l > 0.5) ? (d / (2 - max_c - min_c)) : (d / (max_c + min_c));

			if (max_c == r) {
				h = (g - b) / d + ((g < b) ? 6 : 0);
			}
			if (max_c == g) {
				h = (b - r) / d + 2;
			}
			if (max_c == b) {
				h = (r - g) / d + 4;
			}

			h /= 6;
		}

		this->red = h / 360;
		this->green = s ;
		this->blue = l ;



	}


	void fromHSVToRGB() {

		double fH = red * 360;
		double fS = green;
		double fV = blue;

		double fR = 0;
		double fG = 0;
		double fB = 0;

		float fC = fV * fS;
		float fHPrime = fmod(fH / 60.0, 6);
		float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
		float fM = fV - fC;

		if (0 <= fHPrime && fHPrime < 1) {
			fR = fC;
			fG = fX;
			fB = 0;
		}
		else if (1 <= fHPrime && fHPrime < 2) {
			fR = fX;
			fG = fC;
			fB = 0;
		}
		else if (2 <= fHPrime && fHPrime < 3) {
			fR = 0;
			fG = fC;
			fB = fX;
		}
		else if (3 <= fHPrime && fHPrime < 4) {
			fR = 0;
			fG = fX;
			fB = fC;
		}
		else if (4 <= fHPrime && fHPrime < 5) {
			fR = fX;
			fG = 0;
			fB = fC;
		}
		else if (5 <= fHPrime && fHPrime < 6) {
			fR = fC;
			fG = 0;
			fB = fX;
		}
		else {
			fR = 0;
			fG = 0;
			fB = 0;
		}

		fR += fM;
		fG += fM;
		fB += fM;

		this->red = fR;
		this->green = fG;
		this->blue = fB;

	}

	void fromRGBtoHSV() {

		double fH = 0;
		double fS = 0;
		double fV = 0;

		double fR = red;
		double fG = green;
		double fB = blue;

		float fCMax = max(max(fR, fG), fB);
		float fCMin = min(min(fR, fG), fB);
		float fDelta = fCMax - fCMin;

		if (fDelta > 0) {
			if (fCMax == fR) {
				fH = 60 * (fmod(((fG - fB) / fDelta), 6));
			}
			else if (fCMax == fG) {
				fH = 60 * (((fB - fR) / fDelta) + 2);
			}
			else if (fCMax == fB) {
				fH = 60 * (((fR - fG) / fDelta) + 4);
			}

			if (fCMax > 0) {
				fS = fDelta / fCMax;
			}
			else {
				fS = 0;
			}

			fV = fCMax;
		}
		else {
			fH = 0;
			fS = 0;
			fV = fCMax;
		}

		if (fH < 0) {
			fH = 360 + fH;
		}

		this->red = fH / 360;
		this->green = fS;
		this->blue = fV;


	}

	void fromCMYtoRGB() {
		this->red = 1.0 - this->red;
		this->green = 1.0 - this->green;
		this->blue = 1.0 - this->blue;
	}

	void fromRGBtoCMY() {
		this->red = 1.0 - this->red;
		this->green = 1.0 - this->green;
		this->blue = 1.0 - this->blue;
	}

	void fromRGBtoYCbCr601() {
		float fr = this->red;
		float fg = this->green;
		float fb = this->blue;

		float Y = (float)(0.2989 * fr + 0.5866 * fg + 0.1145 * fb);
		float Cb = (float)(-0.1687 * fr - 0.3313 * fg + 0.5000 * fb);
		float Cr = (float)(0.5000 * fr - 0.4184 * fg - 0.0816 * fb);

		this->red = Y;
		this->green = Cb;
		this->blue = Cr;
	}

	void fromYCbCr601toRGB(){
		float r = max(0.0f, min(1.0f, (float)(this->red  + 0.0000 * this->green  + 1.4022 * this->blue)));
		float g = max(0.0f, min(1.0f, (float)(this->red  - 0.3456 * this->green  - 0.7145 * this->blue)));
		float b = max(0.0f, min(1.0f, (float)(this->red  + 1.7710 * this->green  + 0.0000 * this->blue)));

		this->red = r ;
		this->green = g ;
		this->blue = b;
	}

	void fromRGBtoYCbCr709() {
		float fr = this->red;
		float fg = this->green;
		float fb = this->blue;

		float Y = (float)(0.2126 * fr + 0.7152 * fg + 0.0722 * fb);
		float Cb = (float)(-0.9991 * fr - 0.33609 * fg + 0.436 * fb);
		float Cr = (float)(0.615 * fr - 0.55861 * fg - 0.05639 * fb);

		this->red = Y ;
		this->green = Cb;
		this->blue = Cr;
	}

	void fromYCbCr709toRGB() {
		float r = max(0.0f, min(1.0f, (float)(this->red  + 0.0000 * this->green + 1.28033 * this->blue )));
		float g = max(0.0f, min(1.0f, (float)(this->red - 0.21482 * this->green -0.38059 * this->blue )));
		float b = max(0.0f, min(1.0f, (float)(this->red + 2.12798 * this->green  + 0.0000 * this->blue )));

		this->red = r ;
		this->green = g;
		this->blue = b;
	}

	void fromRGBtoYCoCg() {
		
		double r = this->red / 4 + this->green / 2 + this->blue / 4;
		double g = this->red / 2 + this->green * 0 - this->blue / 2;
		double b = - this->red / 4 + this->green / 2 - this->blue / 4;

		this->red = r;
		this->green = g;
		this->blue = b;
	}

	void fromYCoCgtoRGB() {

		double r = this->red + this->green - this->blue;
		double g = this->red + this->green * 0 + this->blue;
		double b = this->red - this->green - this->blue;

		this->red = r;
		this->green = g;
		this->blue = b;
	}


};


typedef vector<vector<double>> doubleMatrix;
typedef vector<vector<baseColor*>> pnmMatrix;
typedef vector<char> chars;

enum palette {
	RGB,
	HSL,
	HSV,
	YCbCr601,
	YCbCr709,
	YCoCg,
	CMY,
	NO_PALETTE
};

enum files {
	One,
	Three
};

enum channel {
	All,
	Red,
	Green,
	Blue
};




class baseImage {

public:

	int width = 0; // y
	int height = 0; // x
	palette colorSpace = RGB;


	pnmMatrix m;
	chars errorEncounter;


	baseImage(size_t w, size_t h, baseColor color = baseColor()) {

		m = pnmMatrix(h, vector<baseColor*>(w, nullptr));
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				m[i][j] = new baseColor(color);
			}
		}

		width = w;
		height = h;

	}

	~baseImage() {

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				delete m[i][j];
			}
		}

	}
	
	bool loadChannelsFromFile(string filename, channel ch = All, const double channelDepth = 255) {

		if (!errorEncounter.empty()) {
			return false;
		}

		FILE* file = fopen(filename.c_str(), "rb");
		if ((file != NULL)) {}
		else {
			errorEncounter.push_back(1);
			return false;
		}

		char p1, p2 = ' ';
		int w = 0, h = 0, d = 0;

		fscanf(file, "%c%c\n%i %i\n%i\n", &p1, &p2, &w, &h, &d);


		if (((w <= 0) || (h <= 0)) || (p1 != 'P') || (!((p2 == '5') || (p2 == '6'))) || !(d == 255)) {
			errorEncounter.push_back(1);
			return false;
		}

		if ((width == 0) && (height == 0)) {
			width = w;
			height = h;
		}

		if ((ch == All) || (ch == Red)) {
			m = pnmMatrix(h, vector<baseColor*>(w, nullptr));
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					m[i][j] = new baseColor();
				}
			}
		}

		unsigned char t;
		unsigned char r, g, b;

		for (int j = 0; j < min(h, height); j++) {
			for (int i = 0; i < min(w, width); i++) {

				if (ch == All) {

					
					size_t res = 0;
					res += fread(&r, UCHAR_SIZE, 1, file);
					res += fread(&g, UCHAR_SIZE, 1, file);
					res += fread(&b, UCHAR_SIZE, 1, file);

					if (res != 3 * UCHAR_SIZE) {
						errorEncounter.push_back(1);
						return false;
					}

					m[j][i]->red = double(r) / channelDepth;
					m[j][i]->green = double(g) / channelDepth;
					m[j][i]->blue = double(b) / channelDepth;


				}
				else {

				

					size_t r = fread(&t, UCHAR_SIZE, 1, file);
					if (r != UCHAR_SIZE) {
						errorEncounter.push_back(1);
						return false;
					}

					switch (ch) {
					case Red:
						m[j][i]->red = double(t) / channelDepth;
						break;
					case Green:
						m[j][i]->green = double(t) / channelDepth;
						break;
					case Blue:
						m[j][i]->blue = double(t) / channelDepth;
						break;
					}

				}

			}

		}

		fclose(file);
		return true;

	}



	baseImage(string filename, palette c, files count) : colorSpace(c) {

		if (!errorEncounter.empty()) {
			return;
		}


		if (count == Three) {

			string fn = filename.substr(0, filename.find_last_of("."));
			string ext = filename.substr(filename.find_last_of(".") + 1, filename.size() - (filename.find_last_of(".") + 1));

			if (!loadChannelsFromFile(fn + "_1." + ext, Red)) {
				return;
			}
			if (!loadChannelsFromFile(fn + "_2." + ext, Green)) {
				return;
			}
			if (!loadChannelsFromFile(fn + "_3." + ext, Blue)) {
				return;
			}

		} else {
			if (!loadChannelsFromFile(filename, All)) {
				return;
			}
		}

	}


	void processConversionTo(palette c) {
		
		if (colorSpace == c) {
			return;
		}

		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				// Now let's convert to RGB
				switch (colorSpace) {
				case RGB:
					break;
				case HSL:
					m[j][i]->fromHSLToRGB();
					break;
				case HSV:
					m[j][i]->fromHSVToRGB();
					break;
				case YCbCr601:
					m[j][i]->fromYCbCr601toRGB();
					break;
				case YCbCr709:
					m[j][i]->fromYCbCr709toRGB();
					break;
				case YCoCg:
					m[j][i]->fromYCoCgtoRGB();
					break;
				case CMY:
					m[j][i]->fromCMYtoRGB();
					break;
				}


				// Now let's go back from RGB
				switch (c) {
				case RGB:
					break;
				case HSL:
					m[j][i]->fromRGBToHSL();
					break;
				case HSV:
					m[j][i]->fromRGBtoHSV();
					break;
				case YCbCr601:
					m[j][i]->fromRGBtoYCbCr601();
					break;
				case YCbCr709:
					m[j][i]->fromRGBtoYCbCr709();
					break;
				case YCoCg:
					m[j][i]->fromRGBtoYCoCg();
					break;
				case CMY:
					m[j][i]->fromRGBtoCMY();
					break;
				}


			}
		}

	}


	void print(string filename, palette c, files count) {

		if (!errorEncounter.empty()) {
			return;
		}

		processConversionTo(c);


		if (count == Three) {

			string fn = filename.substr(0, filename.find_last_of("."));
			string ext = filename.substr(filename.find_last_of(".") + 1, filename.size() - (filename.find_last_of(".") + 1));

			if (!writeChannels(fn + "_1." + ext, Red)) {
				return;
			}
			if (!writeChannels(fn + "_2." + ext, Green)) {
				return;
			}
			if (!writeChannels(fn + "_3." + ext, Blue)) {
				return;
			}

		}
		else {
			if (!writeChannels(filename, All)) {
				return;
			}
		}

	}

	bool writeChannels(string filename, channel ch = All, unsigned char channelDepth = 255) {
		
		if (!errorEncounter.empty()) {
			return false;
		}

		FILE* file = fopen(filename.c_str(), "wb");
		if (!(file != NULL)) {
			errorEncounter.push_back(1);
			return false;
		}

		if (ch != All) {

			fprintf(file, "P5\n");
			fprintf(file, "%i %i\n%i\n", width, height, channelDepth);

			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {

					unsigned char t;
					switch (ch) {
					case Red:
						t = double(m[j][i]->red * channelDepth);
						break;
					case Green:
						t = double(m[j][i]->green * channelDepth);
						break;
					case Blue:
						t = double(m[j][i]->blue  * channelDepth);
						break;
					}

					fwrite(&t, sizeof(unsigned char), 1, file);

				}
			}

		}
		else {

			fprintf(file, "P6\n");
			fprintf(file, "%i %i\n%i\n", width, height, channelDepth);

			for (int j = 0; j < height; j++) {
				for (int i = 0; i < width; i++) {

					unsigned char r = m[j][i]->red * channelDepth;
					unsigned char g = m[j][i]->green * channelDepth;
					unsigned char b = m[j][i]->blue * channelDepth;

					fwrite(&r, sizeof(unsigned char), 1, file);
					fwrite(&g, sizeof(unsigned char), 1, file);
					fwrite(&b, sizeof(unsigned char), 1, file);

				}
			}

		}

		fclose(file);

	}



};









palette stringToPalette(string arg_next) {
	palette t = RGB;
	if (arg_next == "RGB") {
		t = RGB;
	}
	if (arg_next == "HSL") {
		t = HSL;
	}
	if (arg_next == "HSV") {
		t = HSV;
	}
	if (arg_next == "YCbCr.601") {
		t = YCbCr601;
	}
	if (arg_next == "YCbCr.709") {
		t = YCbCr709;
	}
	if (arg_next == "YCoCg") {
		t = YCoCg;
	}
	if (arg_next == "CMY") {
		t = CMY;
	}
	return t;
}


int main(int argc, char* argv[]) {


	palette fromPalette = RGB;
	palette toPalette = RGB;
	files fromFiles = One;
	files toFiles = One;
	string in = "";
	string out = "";

	for (int i = 0; i < argc; i++) {
		if (i == 0) {
			continue;
		}
		
		string arg(argv[i]);
		if (arg == "-f") {
			if (i + 1 < argc) {
				fromPalette = stringToPalette(argv[i + 1]);
			}
		}
		if (arg == "-t") {
			if (i + 1 < argc) {
				toPalette = stringToPalette(argv[i + 1]);
			}
		}
		if (arg == "-i") {
			if (i + 2 < argc) {
				if (string(argv[i + 1]) == "1") {
					fromFiles = One;
				}
				if (string(argv[i + 1]) == "3") {
					fromFiles = Three;
				}
				in = string(argv[i + 2]);
			}
		}
		if (arg == "-o") {
			if (i + 2 < argc) {
				if (string(argv[i + 1]) == "1") {
					toFiles = One;
				}
				if (string(argv[i + 1]) == "3") {
					toFiles = Three;
				}
				out = string(argv[i + 2]);
			}
		}
	}

	baseImage im(in, fromPalette, fromFiles);
	im.print(out, toPalette, toFiles);

	if (!im.errorEncounter.empty()) {
		cerr << "Some errors encountered.";
		return 1;
	}

	// TEST ALL MODES
	//string name = "west_1";
	//string toname = "test/im";

	//for (int i = RGB; i != NO_PALETTE; i++) {
	//	for (int j = RGB; j != NO_PALETTE; j++) {

	//		string lol = toname + "_" + to_string(i) + "_" + to_string(j);
	//		baseImage a(name + ".ppm", static_cast<palette>(i), files::One);
	//		a.print(lol + "_1to1.ppm", static_cast<palette>(j), files::One);
	//		
	//		baseImage b(name + ".ppm", static_cast<palette>(i), files::One);
	//		b.print(lol + "_1to3.ppm", static_cast<palette>(j), files::Three);

	//		baseImage c(name + ".ppm", static_cast<palette>(i), files::Three);
	//		c.print(lol + "_3to1.ppm", static_cast<palette>(j), files::One);

	//		baseImage d(name + ".ppm", static_cast<palette>(i), files::Three);
	//		d.print(lol + "_3to3.ppm", static_cast<palette>(j), files::Three);

	//	}
	//}


	return 0;

}
