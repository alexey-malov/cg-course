#include "StdAfx.h"
#include "BlurFilter.h"

CBlurFilter::CBlurFilter(void)
{
	/*
	»нициализируем €дро свертки следующей матрицей 3*3
	       |1 3 1|
	1/20 * |3 4 3|
	       |1 3 1|
	*/
	static const float blurMatrix[3][3] = 
	{
		{1, 3, 1},
		{3, 4, 3},
		{1, 3, 1}
	};
	InitializeMatrix(3, 3, &blurMatrix[0][0], 1.0f / 20);
}


