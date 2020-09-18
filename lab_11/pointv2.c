#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <jpeglib.h>
#include <opt.h>
#include <string.h>

char * input_file = (char*) "test.jpeg";
char * output_file = (char*) "test.jpeg";
char * filter = (char*)"none";
double times = 1;
double percentage = 1;
JSAMPARRAY row_pointers = NULL;
JDIMENSION width;
JDIMENSION height;
int comp_num;
int quality = 75;
J_COLOR_SPACE color_space;

void negate(){
  int x, y;
  if (color_space != JCS_RGB)  return;

  for (y=0; y<height; y++) {
    JSAMPROW row = row_pointers[y];
    for (x=0; x<width; x++) {
      JSAMPROW ptr = &(row[x*3]);
		ptr[2] = 255 - ptr[2];
		ptr[1] = 255 - ptr[1];
		ptr[0] = 255 - ptr[0];
    }
  }
}    

void brightness(){
  int x, y;
  if (color_space != JCS_RGB)  return;

  for (y=0; y<height; y++) {
    JSAMPROW row = row_pointers[y];
    for (x=0; x<width; x++) {
      JSAMPROW ptr = &(row[x*3]);
      
		int a = ptr[0];
		a = a + (a*percentage*0.05);
		if(a > 255){
			a = 255;
		}
		if(a < 0) {
			a = 0;
		}
		ptr[0] = a;
		
		a = ptr[1];
		a = a + (a*percentage*0.05);
		if(a > 255){
			a = 255;
		}
		if(a < 0) {
			a = 0;
		}
		ptr[1] = a;
		
		a = ptr[2];
		a = a + (percentage*0.05);
		if(a > 255){
			a = 255;
		}
		if(a < 0) {
			a = 0;
		}
		ptr[2] = a;
  }
}    
}

void contrast(){
  int x, y;
  if (color_space != JCS_RGB)  return;

  for (y=0; y<height; y++) {
    JSAMPROW row = row_pointers[y];
    for (x=0; x<width; x++) {
      JSAMPROW ptr = &(row[x*3]);

	int a = ptr[0];
	a = times * (a-127) + 127;
	if(a > 255){
			a = 255;
	}
	if(a < 0) {
			a = 0;
	}
	ptr[0] = a;
	a = ptr[1];
	a = times * (a-127) + 127;
	if(a > 255){
			a = 255;
	}
	if(a < 0) {
			a = 0;
	}
	ptr[1] = a;
	a = ptr[2];
	a = times * (a-127) + 127;
	if(a > 255){
			a = 255;
	}
	if(a < 0) {
			a = 0;
	}
	ptr[2] = a;      
    }
  }
}    

void process_file(){
    if(strcmp(filter, "negate") == 0){
            negate();
    }
    if(strcmp(filter, "contrast") == 0){
            contrast();
    }
    if(strcmp(filter, "brightness") == 0){
            brightness();
    }
}

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

void read_jpeg_file( char *filename )
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	int x, y;
	
	FILE *infile = fopen( filename, "rb" );
	unsigned long location = 0;
	int i = 0;
	
	if ( !infile )
	{
		abort_("Error", filename);
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &cinfo );
	jpeg_stdio_src( &cinfo, infile );
	jpeg_read_header( &cinfo, TRUE );


	jpeg_start_decompress( &cinfo );
	width = cinfo.output_width;
	height = cinfo.output_height;
	comp_num = cinfo.out_color_components;
	color_space = cinfo.out_color_space;
	
	size_t rowbytes = width * comp_num;

	row_pointers = (JSAMPARRAY) malloc(sizeof(j_common_ptr) * height);
	for (y=0; y<height; y++){
		row_pointers[y] = (JSAMPROW) malloc(rowbytes);
	}
	y=0;
	JSAMPARRAY tmp = row_pointers;
	while( cinfo.output_scanline < cinfo.image_height )
	{
		y = jpeg_read_scanlines( &cinfo, tmp, 1 );
		tmp +=y;
	}
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	fclose( infile );
}

void write_jpeg_file( char *filename )
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int y;
	JSAMPARRAY tmp;

	
	/* this is a pointer to one row of image data */
	FILE *outfile = fopen( filename, "wb" );
	
	if ( !outfile )	{
		abort_("Error", filename );
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = width;	
	cinfo.image_height = height;
	cinfo.input_components = comp_num;
	cinfo.in_color_space = color_space;
	jpeg_set_defaults( &cinfo );
	jpeg_set_quality (&cinfo, quality, TRUE);
	jpeg_start_compress( &cinfo, TRUE );
	tmp = row_pointers;
	while( cinfo.next_scanline < cinfo.image_height )
	{
		y = jpeg_write_scanlines( &cinfo, tmp, 1 );
		tmp +=y;
	}
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose( outfile );
	
	for (y=0; y<height; y++){
		free(row_pointers[y]);
	}
	free(row_pointers);
}

int main(int argc, char **argv)
{
	
  optrega(&input_file, OPT_STRING, 'i', (char*) "input-file", (char*) "Input JPEG File");
  optrega(&output_file, OPT_STRING, 'o', (char*) "output-file", (char*) "Output JPEG File");
  optrega(&filter, OPT_STRING, 'f', (char*) "filter", (char*) "Filter");
  optrega(&times, OPT_DOUBLE, 't', (char*) "times", (char*) "Multiplyer");
  optrega(&percentage, OPT_DOUBLE, 'p', (char*) "percentage", (char*) "percentage brightnessi");
  opt(&argc,&argv);
  opt_free();

	read_jpeg_file(input_file);
	process_file();
	write_jpeg_file(output_file);
	return 0;
}

