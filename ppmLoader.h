#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <gl/glut.h>

GLubyte* 
glmReadPPM2(char* filename, int* width, int* height)
{
    FILE* fp;
    int i, j,w, h, d, r,g,b,c;
    unsigned char* image;
    char head[70];          /* max line <= 70 in PPM (per spec). */
    
    fp = fopen(filename, "r");
    if (!fp) {
        printf("error opening file %s\n",filename);
        return NULL;
    }
    
      /* read and discard first two lines */
      fgets(head, 128, fp);
      fgets(head, 128, fp);
      
      /* read image size and (?)max component value */
      fscanf(fp, "%d %d", &w, &h);
      fscanf(fp, "%d", &d);
      image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*4);

      c = 0;
      for (i=0; i<h; i++) {
	   for (j=0; j<w; j++) {
	    fscanf(fp, "%d %d %d", &r, &g, &b);
	    image[c++] = (GLubyte) r;
	    image[c++] = (GLubyte) g;
	    image[c++] = (GLubyte) b;
        //if pixel is mostly black, make it transparent
        if (r<50 && g<50 && b<50)
   	      image[c++] = (GLubyte) 0;
        else
 	      image[c++] = (GLubyte) 255;
	  }
	}
 
    *width = w;
    *height = h;
    return image;
}
