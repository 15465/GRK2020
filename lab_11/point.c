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


double procenty = 1;


/* we will be using this uninitialized pointer later to store raw, uncompressd image */
JSAMPARRAY row_pointers = NULL;


/* dimensions of the image we want to write */
JDIMENSION width;
JDIMENSION height;
int num_components;
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

void jasnosc(){
  int x, y;
  if (color_space != JCS_RGB)  return;

  for (y=0; y<height; y++) {
    JSAMPROW row = row_pointers[y];
    for (x=0; x<width; x++) {
      JSAMPROW ptr = &(row[x*3]);
      
		int a = ptr[0];
		a = a + (a*procenty*0.05);
		if(a > 255){
			a = 255;
		}
		if(a < 0) {
			a = 0;
		}
		ptr[0] = a;
		
		a = ptr[1];
		a = a + (a*procenty*0.05);
		if(a > 255){
			a = 255;
		}
		if(a < 0) {
			a = 0;
		}
		ptr[1] = a;
		
		a = ptr[2];
		a = a + (k*procenty*0.05);
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

void kontrast(){
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
    if(strcmp(filter, "kontrast") == 0){
            kontrast();
    }
    if(strcmp(filter, "jasnosc") == 0){
            jasnosc();
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


/**
 * read_jpeg_file Reads from a jpeg file on disk specified by filename and saves into the 
 * raw_image buffer in an uncompressed format.
 * 
 * \returns positive integer if successful, -1 otherwise
 * \param *filename char string specifying the file name to read from
 *
 */

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
		abort_("Error opening input jpeg file %s!\n", filename);
	}
	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &cinfo );
	/* this makes the library read from infile */
	jpeg_stdio_src( &cinfo, infile );
	/* reading the image header which contains image information */
	jpeg_read_header( &cinfo, TRUE );


	/* Start decompression jpeg here */
	jpeg_start_decompress( &cinfo );
	width = cinfo.output_width;
	height = cinfo.output_height;
	num_components = cinfo.out_color_components;
	color_space = cinfo.out_color_space;
	
	size_t rowbytes = width * num_components;

	/* allocate memory to hold the uncompressed image */
	row_pointers = (JSAMPARRAY) malloc(sizeof(j_common_ptr) * height);
	for (y=0; y<height; y++){
		row_pointers[y] = (JSAMPROW) malloc(rowbytes);
	}
	/* read one scan line at a time */
	y=0;
	JSAMPARRAY tmp = row_pointers;
	while( cinfo.output_scanline < cinfo.image_height )
	{
		y = jpeg_read_scanlines( &cinfo, tmp, 1 );
		tmp +=y;
	}
	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
// 	free( row_pointer[0] );
	fclose( infile );
	/* yup, we succeeded! */
}




/**
 * write_jpeg_file Writes the raw image data stored in the raw_image buffer
 * to a jpeg image with default compression and smoothing options in the file
 * specified by *filename.
 *
 * \returns positive integer if successful, -1 otherwise
 * \param *filename char string specifying the file name to save to
 *
 */
void write_jpeg_file( char *filename )
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int y;
	JSAMPARRAY tmp;

	
	/* this is a pointer to one row of image data */
	FILE *outfile = fopen( filename, "wb" );
	
	if ( !outfile )	{
		abort_("Error opening output jpeg file %s!\n", filename );
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	/* Setting the parameters of the output file here */
	cinfo.image_width = width;	
	cinfo.image_height = height;
	cinfo.input_components = num_components;
	cinfo.in_color_space = color_space;
    /* default compression parameters, we shouldn't be worried about these */
	jpeg_set_defaults( &cinfo );
	jpeg_set_quality (&cinfo, quality, TRUE);
	/* Now do the compression .. */
	jpeg_start_compress( &cinfo, TRUE );
	/* like reading a file, this time write one row at a time */
	tmp = row_pointers;
	while( cinfo.next_scanline < cinfo.image_height )
	{
		y = jpeg_write_scanlines( &cinfo, tmp, 1 );
		tmp +=y;
	}
	/* similar to read file, clean up after we're done compressing */
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose( outfile );
	
        /* cleanup heap allocation */
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
  optrega(&procenty, OPT_DOUBLE, 'p', (char*) "procenty", (char*) "procenty jasnosci");
  opt(&argc,&argv);
  opt_free();

	read_jpeg_file(input_file);
	process_file();
	write_jpeg_file(output_file);
	return 0;
}

