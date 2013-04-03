// ----------------------------------------------------------------------------

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "PngConv.h"

int main(int argc, char **argv)
{
	int ret;
	CPngConv pngConv;

	if (argc!=3)
    {
        printf("'Apple PNG' to PNG image converter.\n\nUsage: %s <input> <output>\n\n", argv[0]);
        exit(1);
    }
	ret = pngConv.Convert(argv[1], argv[2]);
	if (ret)
		printf("## ERROR: %s could not be converted (%s)\n", argv[1], pngConv.GetErrorMessage(ret));
	else {
		if (pngConv.IsConverted())
			printf("%s converted\n", argv[1]);
		else
			printf("%s copied\n", argv[1]);
	}
	return ret;
}
