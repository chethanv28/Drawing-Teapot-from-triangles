#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

// Enum to identify the color index
enum RGBIdentifier
{
	Red   = 0,
	Green = 1,
	Blue  = 2,
};
/******** Helper method - for general assitance ********/
// Method to exchange the vertices in the array
int SwapVertices(GzCoord* gzVer, int nVer1, int nVer2)
{
	// Temp variables to do the vertices exchange.
	float fExchangeX = 0.0f, fExchangeY = 0.0f, fExchangeZ = 0.0f;

	fExchangeX = gzVer[nVer1][X];
	fExchangeY = gzVer[nVer1][Y];
	fExchangeZ = gzVer[nVer1][Z];

	gzVer[nVer1][X] = gzVer[nVer2][X];
	gzVer[nVer1][Y] = gzVer[nVer2][Y];
	gzVer[nVer1][Z] = gzVer[nVer2][Z];

	gzVer[nVer2][X] = fExchangeX;
	gzVer[nVer2][Y] = fExchangeY;
	gzVer[nVer2][Z] = fExchangeZ;

	return GZ_SUCCESS;
}
/******** Helper method - for general assitance ********/
// Method to find the leftmost and rightmost pixels of the bounding box
int GetBoundingBoxWidth(int &nLeft, int &nRight, GzCoord* gzPoints)
{
	// Cover all 6 combinations of the sorted order
	if (gzPoints[0][X] < gzPoints[1][X])
	{
		if (gzPoints[0][X] < gzPoints[2][X])
		{
			// Pixels are not floats
			nLeft = (int)floor(gzPoints[0][X]);
			if (gzPoints[1][X] < gzPoints[2][X])
			{
				//Leftmost - 0 Rightmost 2
				nRight = (int)ceil(gzPoints[2][X]);
			}
			else
			{
				//Leftmost - 0 Rightmost 1
				nRight = (int)ceil(gzPoints[1][X]);
			}
		}
		else
		{
			//Leftmost - 2 Rightmost 1
			nLeft = (int)floor(gzPoints[2][X]);
			nRight = (int)ceil(gzPoints[1][X]);
		}
	}
	else
	{
		if (gzPoints[1][X] < gzPoints[2][X])
		{
			nLeft = (int)floor(gzPoints[1][X]);
			if (gzPoints[0][X] < gzPoints[2][X])
			{
				//Leftmost - 1 Rightmost 2
				nRight = (int)ceil(gzPoints[2][X]);
			}
			else
			{
				//Leftmost - 1 Rightmost 0
				nRight = (int)ceil(gzPoints[0][X]);
			}
		}
		else
		{
			//Leftmost - 2 Rightmost 0
			nLeft = (int)floor(gzPoints[2][X]);
			nRight = (int)ceil(gzPoints[0][X]);
		}
	}
	return GZ_SUCCESS;
}

// Method to convert float color to GzIntensity short
short	ctoi(float color);

int GzNewRender(GzRender **render, GzDisplay *display)
{
/* 
- malloc a renderer struct
- span interpolator needs pointer to display for pixel writes
*/

	// Create a new render

	*render = new GzRender();

	if (NULL == *render || NULL == display)
	{
		// Return failure if the render or display is NULL
		return GZ_FAILURE;
	}


	// Assign the Gzdisplay to renderes's display

	(*render)->display = display;

	(*render)->flatcolor[Red]   = 0;
	(*render)->flatcolor[Green] = 0;
	(*render)->flatcolor[Blue]  = 0;

	// Return Success here
	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/

	// If render is already, return failure
	if (NULL != render)
	{
		// Delete the render and assign display to NULL
		render->display = NULL;

		delete render;		
	}
	else
	{
		return GZ_FAILURE;
	}

	// Return success after deleting the render
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/

	// Check if the render is NULL and return failure 
	if (render == NULL)
	{
		return GZ_FAILURE;
	}

	// Initialize the display to default values
	GzInitDisplay(render->display);

	//Return failure if the initialized values are not proper.
	if (NULL != render->display)
	{
		render->flatcolor[Red]   = 0;
		render->flatcolor[Green] = 0;
		render->flatcolor[Blue]  = 0;		
	}
	else
	{
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/

	// Check if the tokens/render/valuelist are NULL
	if (NULL == nameList || NULL == render || NULL == valueList)
	{
		return GZ_FAILURE;
	}
	
	GzColor* gzColor = NULL;
	for (int i = 0; i < numAttributes; i++)
	{
		if (nameList[i] = GZ_RGB_COLOR) 
		{
			gzColor = (GzColor*)valueList[i];

			float fRed   = gzColor[i][Red];
			float fGreen = gzColor[i][Green];
			float fBlue  = gzColor[i][Blue];

			render->flatcolor[Red]   = fRed;
			render->flatcolor[Green] = fGreen;
			render->flatcolor[Blue]  = fBlue;
		}
	}	

	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions 
- Invoke the scan converter and return an error code
*/
	if (NULL != nameList && NULL != render && NULL != valueList)
	{

		int nLowestY = 0;

		for (int i = 0; i < numParts; i++)
		{
			if (nameList[i] == GZ_POSITION)
			{
				// get points
				GzCoord* gzPoints = (GzCoord*)valueList[i];

				// sort the vertices by Y using bubble sort technique				
				for (int i = 0; i <= 1; i++)
				{
					nLowestY = i;
					for (int j = i + 1; j <= 2; j++)
					{
						if (gzPoints[nLowestY][Y] > gzPoints[j][Y])
						{
							nLowestY = j;
						}
					}

					if (nLowestY != i)
					{
						SwapVertices(gzPoints, nLowestY, i);
					}
				}

				if (gzPoints[0][Y] == gzPoints[1][Y])
				{
					if (gzPoints[0][X] > gzPoints[1][X])
					{
						SwapVertices(gzPoints, 0, 1);
					}
				}

				else if (gzPoints[1][Y] == gzPoints[2][Y])
				{
					if (gzPoints[1][X] < gzPoints[2][X])
					{
						SwapVertices(gzPoints, 2, 1);
					}
				}

				// Find two Edges of the triangle to compute the normal vector
				GzCoord Triedge12;
				Triedge12[X] = gzPoints[1][X] - gzPoints[0][X];
				Triedge12[Y] = gzPoints[1][Y] - gzPoints[0][Y];
				Triedge12[Z] = gzPoints[1][Z] - gzPoints[0][Z];

				GzCoord Triedge23;
				Triedge23[X] = gzPoints[2][X] - gzPoints[1][X];
				Triedge23[Y] = gzPoints[2][Y] - gzPoints[1][Y];
				Triedge23[Z] = gzPoints[2][Z] - gzPoints[1][Z];

				// compute the normal vector by doing the cross product of two edges
				float fOutvectorX = Triedge12[Y] * Triedge23[Z] - Triedge12[Z] * Triedge23[Y];
				float fOutvectorY = Triedge12[Z] * Triedge23[X] - Triedge12[X] * Triedge23[Z];
				float fOutvectorZ = Triedge12[X] * Triedge23[Y] - Triedge12[Y] * Triedge23[X];

				//Ax + By + Cz + D = 0   ------- [1]
				float D = -(fOutvectorX * gzPoints[0][X]) - (fOutvectorY * gzPoints[0][Y]) - (fOutvectorZ *	gzPoints[0][Z]);

				int nLeft = 0, nRight = 0;
				GetBoundingBoxWidth(nLeft, nRight, gzPoints);

				// get the T/B from the sorted Y vertices
				int nTop = (int)floor(gzPoints[0][Y]);
				int nBottom = (int)ceil(gzPoints[2][Y]);
				
				float interpolatedZ = 0.0f;
				// for each pixel(x,y) in the bounding box evaluate equation [1]
				for (int x = nLeft; x < nRight; x++)
				{
					for (int y = nTop; y < nBottom; y++)
					{
						float Edge12 = 0.0f, Edge23 = 0.0f, Edge31 = 0.0f;

						Edge12 = (gzPoints[1][Y] - gzPoints[0][Y])*((float)x - gzPoints[0][X]) - (gzPoints[1][X] - gzPoints[0][X])*((float)y - gzPoints[0][Y]);

						Edge23 = (gzPoints[2][Y] - gzPoints[1][Y])*((float)x - gzPoints[1][X]) - (gzPoints[2][X] - gzPoints[1][X])*((float)y - gzPoints[1][Y]);

						Edge31 = (gzPoints[0][Y] - gzPoints[2][Y])*((float)x - gzPoints[2][X]) - (gzPoints[0][X] - gzPoints[2][X])*((float)y - gzPoints[2][Y]);

						if (((Edge12 > 0) && (Edge23 > 0) && (Edge31 > 0)) || ((Edge12 < 0) && (Edge23 < 0) && (Edge31 < 0)) 
							|| (Edge12 == 0 || Edge23 == 0 || Edge31 == 0))
						{
							// Z - buffer to remove hidden surfaces
							interpolatedZ = (- (fOutvectorX * x) - (fOutvectorY * y) - D) / fOutvectorZ;
							GzDepth zCurrentValFb = 0;
							GzIntensity red   = 0;
							GzIntensity green = 0;
							GzIntensity blue  = 0;
							GzIntensity alpha = 0;

							// get the current values from the frambuffer
							GzGetDisplay(render->display, x, y, &red, &green, &blue, &alpha, &zCurrentValFb);
							
							// Compare between computed Zpix versus framebuffer Z value. If lesser write new pixel color, else dont do anything
							if (interpolatedZ < zCurrentValFb)
							{
								zCurrentValFb = interpolatedZ;

								red   = (GzIntensity)ctoi((float)render->flatcolor[Red]);
								green = (GzIntensity)ctoi((float)render->flatcolor[Green]);
								blue  = (GzIntensity)ctoi((float)render->flatcolor[Blue]);
								
								//Writing new pixel value to the framebuffer
								GzPutDisplay(render->display, x, y, red, green, blue, alpha, zCurrentValFb);
							}
						}
					}
				}
			}

			if (nameList[i] == GZ_NULL_TOKEN)
			{
				//Do nothing
				continue;
			}
		}
		return GZ_SUCCESS;
	}

	else
	{
		return GZ_FAILURE;
	}
	
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

