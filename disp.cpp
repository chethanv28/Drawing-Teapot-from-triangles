/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"

// Define constants here
#define BYTESIZEOFRGBCOLORSYSTEM	   3
#define MAXBYTESIZE					0xFF
#define BACKGRNDCOLOR_VALUE			2000
#define HIGHINTENSITY				4095
#define LOWINTENSITY				   0

// 
char GetCharFromShort(short sVar)
{
	sVar = sVar >> 4;

	// Clamp the values to maximum and minimum if they go out of range 
	sVar = sVar < 0 ? 0 : sVar;
	sVar = sVar > MAXBYTESIZE ? MAXBYTESIZE : sVar;

	return sVar;
}

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	// Allocate memory for the framebuffer
	*framebuffer = (char*) new char[ width * height * BYTESIZEOFRGBCOLORSYSTEM ];

	// Check for unsuccessful allocation
	if (*framebuffer == NULL)
	{
		return GZ_FAILURE;
	}

	// Return success
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/

	// Create a display
	*display = (GzDisplay*) new GzDisplay();


	(*display)->xres = xRes >= MAXXRES ? MAXXRES : xRes;
	(*display)->yres = yRes >= MAXYRES ? MAXYRES : yRes;

	
	// Compute the screen resolution
	int nHorizontalResolution = (*display)->xres;
	int nVerticalResolution = (*display)->yres;
	int ScreenResolution = nHorizontalResolution * nVerticalResolution;
	
	//Create memory for the framebuffer equivalent to the required screen resolution
	(*display)->fbuf = new GzPixel[ScreenResolution];

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	// Delete framebuffer
	display->fbuf = NULL;
	delete display->fbuf;

	// Delete display
	display = NULL;
	delete display;

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	if ( NULL == display )
	{
		return GZ_FAILURE;
	}

	*xRes = display->xres;
	*yRes = display->yres;
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
	// Compute the pixel count
	int nPixelCount = display->xres * display->yres;

	// Initialize the 
	for (int niterator = 0; niterator < nPixelCount; niterator++)
	{
		display->fbuf[niterator].blue = BACKGRNDCOLOR_VALUE;
		display->fbuf[niterator].green = BACKGRNDCOLOR_VALUE;
		display->fbuf[niterator].red = BACKGRNDCOLOR_VALUE;
		display->fbuf[niterator].z = MAXINT;
		display->fbuf[niterator].alpha = 0;
	}
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	if (i >= 0 && j >= 0 && i < display->xres && j < display->yres)
	{
		// Write pixel values to the framebuffer of the display

		display->fbuf[ARRAY(i, j)].alpha = a;
		display->fbuf[ARRAY(i, j)].z = z;

		//Check for boudary values and clamp the value of red, green and blue if the intensity is not in the range of 0 - 4095
		display->fbuf[ARRAY(i, j)].red = r > HIGHINTENSITY ? HIGHINTENSITY : r;
		display->fbuf[ARRAY(i, j)].red = r < LOWINTENSITY ? LOWINTENSITY : r;

		display->fbuf[ARRAY(i, j)].green = g > HIGHINTENSITY ? HIGHINTENSITY : g;
		display->fbuf[ARRAY(i, j)].green = g < LOWINTENSITY ? LOWINTENSITY : g;

		display->fbuf[ARRAY(i, j)].blue = b > HIGHINTENSITY ? HIGHINTENSITY : b;
		display->fbuf[ARRAY(i, j)].blue = b < LOWINTENSITY ? LOWINTENSITY : b;

		return GZ_SUCCESS;
	}
	else
	{
		return GZ_FAILURE;
	}
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	// Check if the values are valid
	if (i > 0 && j > 0 && display->xres >= i && display->yres >= j)
	{
		// Pass back a pixel value into the display
		GzPixel &PixelVal = display->fbuf[ARRAY(i, j)];
		*r = PixelVal.red;
		*g = PixelVal.green;
		*b = PixelVal.blue;
		*a = PixelVal.alpha;
		*z = PixelVal.z;
		return GZ_SUCCESS;
	}
	else
	{
		return GZ_FAILURE;
	}
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	char cPixelVal;

	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);
	
	// Writing pixel values to the ppm file
	for (int iter1 = 0; iter1 < display->yres; iter1++)
	{ 
		for (int iter2 = 0; iter2 < display->xres; iter2++)
		{
			cPixelVal = GetCharFromShort(display->fbuf[ARRAY(iter2, iter1)].red);
			fprintf(outfile, "%c", cPixelVal);

			cPixelVal = GetCharFromShort(display->fbuf[ARRAY(iter2, iter1)].green);
			fprintf(outfile, "%c", cPixelVal);

			cPixelVal = GetCharFromShort(display->fbuf[ARRAY(iter2, iter1)].blue);
			fprintf(outfile, "%c", cPixelVal);
		}
	}
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	// Compute the column height in bytes - 3 x y resolution  
	int nColumnHeightinBytes  = display->yres * BYTESIZEOFRGBCOLORSYSTEM;
	for (int nColumnNum = 0; nColumnNum < display->yres; nColumnNum++)
	{
		for (int nRowNum = 0; nRowNum < display->xres; nRowNum++)
		{
			// Store the pixels in the order of blue, green and red for the windows framebuffer
			framebuffer[nColumnNum * nColumnHeightinBytes + nRowNum * BYTESIZEOFRGBCOLORSYSTEM       ] = GetCharFromShort(display->fbuf[ARRAY(nRowNum, nColumnNum)].blue);
			// Increment the array index by one for every iteration
			framebuffer[(nColumnNum * nColumnHeightinBytes + nRowNum * BYTESIZEOFRGBCOLORSYSTEM) + 1 ] = GetCharFromShort(display->fbuf[ARRAY(nRowNum, nColumnNum)].green);
			// Increment the array index by two for every iteration
			framebuffer[(nColumnNum * nColumnHeightinBytes + nRowNum * BYTESIZEOFRGBCOLORSYSTEM) + 2 ] = GetCharFromShort(display->fbuf[ARRAY(nRowNum, nColumnNum)].red);
		}
	}
	return GZ_SUCCESS;
}