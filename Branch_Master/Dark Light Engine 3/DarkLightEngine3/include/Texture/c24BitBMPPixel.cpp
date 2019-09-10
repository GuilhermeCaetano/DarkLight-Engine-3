// Written by Michael Feeney, Fanshawe College, 2006
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the MIT License:
// http://opensource.org/licenses/MIT
// (Also, it would be Supah Cool if I heard this was remotely useful!)
// Use this code at your own risk. It is indented only as a learning aid.

#include <Texture\c24BitBMPPixel.h>

// Default constructor - all zeros
c24BitBMPPixel::c24BitBMPPixel()
	: redPixel(0), greenPixel(0), bluePixel(0)
{
	return;
}

c24BitBMPPixel::c24BitBMPPixel(uchar red, uchar green, uchar blue)
	: redPixel(red), greenPixel(green), bluePixel(blue)
{
	return;
}

c24BitBMPPixel::~c24BitBMPPixel()
{
	return;
}

// Default constructor - all zeros
c32BitBMPPixel::c32BitBMPPixel()
	: redPixel(0), greenPixel(0), bluePixel(0), alphaPixel(1)
{
	return;
}

c32BitBMPPixel::c32BitBMPPixel(uchar red, uchar green, uchar blue)
	: redPixel(red), greenPixel(green), bluePixel(blue), alphaPixel(1)
{
	return;
}

c32BitBMPPixel::c32BitBMPPixel(uchar red, uchar green, uchar blue, uchar aplha)
	: redPixel(red), greenPixel(green), bluePixel(blue), alphaPixel(aplha)
{
	return;
}

c32BitBMPPixel::~c32BitBMPPixel()
{
	return;
}
