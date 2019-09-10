#ifndef _c24BitBMPPixel_HG_
#define _c24BitBMPPixel_HG_

// Written by Michael Feeney, Fanshawe College, 2006
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the MIT License:
// http://opensource.org/licenses/MIT
// (Also, it would be Supah Cool if I heard this was remotely useful!)
// Use this code at your own risk. It is indented only as a learning aid.

typedef unsigned char uchar;

struct c24BitBMPPixel
{
public:
	c24BitBMPPixel();	// Default constructor - all zeros
	c24BitBMPPixel(uchar red, uchar green, uchar blue);
	~c24BitBMPPixel();
	uchar redPixel;
	uchar greenPixel;
	uchar bluePixel;
};

struct c32BitBMPPixel
{
public:
	c32BitBMPPixel();	// Default constructor - all zeros
	c32BitBMPPixel(uchar red, uchar green, uchar blue);
	c32BitBMPPixel(uchar red, uchar green, uchar blue, uchar aplha);
	~c32BitBMPPixel();
	uchar redPixel;
	uchar greenPixel;
	uchar bluePixel;
	uchar alphaPixel;
};



#endif // !_c24BitBMPPixel_HG_

