#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include "jpeglib.h"
#include "linmath.h"

#define TARGET_FPS 60

GLuint program;
GLuint gTextureMap;

struct {float x; float y; float z;} eyelocation;
double pitch ;
double yaw ;
double speed;
float hmult;
vec3 gaze = {0,0,0};
vec3 up = {0,0,0};

typedef GLubyte Pixel[3];

typedef struct
{
    float x, y, z;
    float s, t;
	float nx,ny,nz;
} Vertice;

Vertice* vertices;
/*
███████╗██╗  ██╗ █████╗ ██████╗ ███████╗██████╗ ███████╗
██╔════╝██║  ██║██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔════╝
███████╗███████║███████║██║  ██║█████╗  ██████╔╝███████╗
╚════██║██╔══██║██╔══██║██║  ██║██╔══╝  ██╔══██╗╚════██║
███████║██║  ██║██║  ██║██████╔╝███████╗██║  ██║███████║
╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝
                                                        
*/
void readShader(const char* fname, char *source)
{
	FILE *fp;
	fp = fopen(fname,"r");
	if (fp==NULL)
	{
		fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
		glfwTerminate();
		exit(1);
	}
	char tmp[1000];
	while (fgets(tmp,1000,fp)!=NULL)
	{
		strcat(source,tmp);
		strcat(source,"\n");
	}
}

unsigned int loadShader(const char *source, unsigned int mode)
{
	GLuint id;
	id = glCreateShader(mode);

	glShaderSource(id,1,&source,NULL);

	glCompileShader(id);

	char error[1000];

	glGetShaderInfoLog(id,1000,NULL,error);
	printf("Compile status: \n %s\n",error);

	return id;
}

void initShaders()
{
	char *vsSource, *fsSource;
	GLuint vs,fs;

	vsSource = (char *)malloc(sizeof(char)*20000);
	fsSource = (char *)malloc(sizeof(char)*20000);

	vsSource[0]='\0';
	fsSource[0]='\0';

	program = glCreateProgram();

	readShader("hw3_flat.vs",vsSource);
	readShader("hw3_flat.fs",fsSource);

	vs = loadShader(vsSource,GL_VERTEX_SHADER);
	fs = loadShader(fsSource,GL_FRAGMENT_SHADER);

	glAttachShader(program,vs);
	glAttachShader(program,fs);

	glLinkProgram(program);

	glUseProgram(program);
	
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
int width;
int height;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE &&  (action == GLFW_REPEAT || action == GLFW_PRESS ))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	else if (key == GLFW_KEY_Y && (action == GLFW_REPEAT || action == GLFW_PRESS ))
        speed++;
	else if (key == GLFW_KEY_H &&  (action == GLFW_REPEAT || action == GLFW_PRESS ))
        speed--;
	else if (key == GLFW_KEY_I &&  (action == GLFW_REPEAT || action == GLFW_PRESS )){
        eyelocation = {width/2 , width/10 , -width/4};
		pitch = 0;
		yaw = 0;
		speed = 0;
		gaze[0] = 0;
		gaze[1] = 0;
		gaze[2] = 1;
		up[0]=0;
		up[1]=1;
		up[2]=0;
	}
	else if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS ) ){
		vec3 right;
		vec3_mul_cross(right, gaze, up);
		vec3_norm(right, right);

		vec3_scale(gaze,gaze,20);
		vec3_sub(gaze, gaze, right);
		vec3_norm(gaze, gaze);

	}
	else if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS ) ){
		vec3 right;
		vec3_mul_cross(right, gaze, up);
		vec3_norm(right, right);

		vec3_scale(gaze,gaze,20);
		vec3_add(gaze, gaze, right);
		vec3_norm(gaze, gaze);
	}
	else if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS )){
		vec3 tempGaze  = {gaze[0], gaze[1], gaze[2]};
		vec3_scale(gaze, gaze, 20);
		vec3_sub(gaze, gaze, up);
		vec3_norm(gaze, gaze);

		vec3_scale(up,up, 20);
		vec3_add(up , up , tempGaze);
		vec3_norm(up, up);

	}
	else if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS )){
		vec3 tempGaze  = {gaze[0], gaze[1], gaze[2]};
		vec3_scale(gaze,gaze,20);
		vec3_add(gaze, gaze, up);
		vec3_norm(gaze, gaze);

		vec3_scale(up, up,20);
		vec3_sub(up , up , tempGaze);
		vec3_norm(up, up);
	}
	else if (key == GLFW_KEY_X && (action == GLFW_REPEAT || action == GLFW_PRESS )){
		speed = 0;
	}
	else if (key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS )){
		hmult += 0.5;
	}
	else if (key == GLFW_KEY_F && (action == GLFW_REPEAT || action == GLFW_PRESS )){
		hmult -= 0.5;
	}
}
/*
████████╗███████╗██╗  ██╗████████╗██╗   ██╗██████╗ ███████╗
╚══██╔══╝██╔════╝╚██╗██╔╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝
   ██║   █████╗   ╚███╔╝    ██║   ██║   ██║██████╔╝█████╗  
   ██║   ██╔══╝   ██╔██╗    ██║   ██║   ██║██╔══██╗██╔══╝  
   ██║   ███████╗██╔╝ ██╗   ██║   ╚██████╔╝██║  ██║███████╗
   ╚═╝   ╚══════╝╚═╝  ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝
                                                           
*/
Pixel*  image;
void read_texture_file( const char *filename )
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];
	
	FILE *infile = fopen( filename, "rb" );
	
	
	if ( !infile )
	{
		printf("Error opening jpeg file %s\n!", filename );
		return;
	}

	cinfo.err = jpeg_std_error( &jerr );

	jpeg_create_decompress( &cinfo );

	jpeg_stdio_src( &cinfo, infile );

	jpeg_read_header( &cinfo, TRUE );


	jpeg_start_decompress( &cinfo );
	printf("components = %d\n", cinfo.num_components);
	width = cinfo.output_width;
	height = cinfo.output_height;
	image  = (Pixel *)malloc( width*height* sizeof(Pixel));	

	row_pointer[0] = (unsigned char *)malloc( width*3);

	int row = 0;
	while( cinfo.output_scanline < height )
	{
		jpeg_read_scanlines( &cinfo, row_pointer, 1 );
		int i = 0;
		for( int col=0; col<width;col++){
			image[row*width + col][0] = (GLubyte)row_pointer[0][i++];
			image[row*width + col][1] = (GLubyte)row_pointer[0][i++];
			image[row*width + col][2] = (GLubyte)row_pointer[0][i++];
		}
		row++;
	}

	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	fclose( infile );
}
/*
██╗  ██╗███████╗██╗ ██████╗ ██╗  ██╗████████╗███╗   ███╗ █████╗ ██████╗ 
██║  ██║██╔════╝██║██╔════╝ ██║  ██║╚══██╔══╝████╗ ████║██╔══██╗██╔══██╗
███████║█████╗  ██║██║  ███╗███████║   ██║   ██╔████╔██║███████║██████╔╝
██╔══██║██╔══╝  ██║██║   ██║██╔══██║   ██║   ██║╚██╔╝██║██╔══██║██╔═══╝ 
██║  ██║███████╗██║╚██████╔╝██║  ██║   ██║   ██║ ╚═╝ ██║██║  ██║██║     
╚═╝  ╚═╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝     
                                                                        
*/
double*  heightmap;
void read_height_file( const char *filename )
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];
	
	FILE *infile = fopen( filename, "rb" );int width;
int height;
	
	
	if ( !infile )
	{
		printf("Error opening jpeg file %s\n!", filename );
		return;
	}

	cinfo.err = jpeg_std_error( &jerr );

	jpeg_create_decompress( &cinfo );

	jpeg_stdio_src( &cinfo, infile );

	jpeg_read_header( &cinfo, TRUE );


	jpeg_start_decompress( &cinfo );
	printf("components = %d\n", cinfo.num_components);
	width = cinfo.output_width;
	height = cinfo.output_height;
	heightmap  = (double *)malloc( width*height* sizeof(double));	

	row_pointer[0] = (unsigned char *)malloc( width*3);

	int row = 0;
	while( cinfo.output_scanline < height )
	{
		jpeg_read_scanlines( &cinfo, row_pointer, 1 );
		int i = 0;
		for( int col=0; col<width;col++){
			heightmap[(height-1 - row )*width +  width-1-col] = (GLubyte)row_pointer[0][i] / 255.0f;
			i+=3;
		}
		row++;
	}

	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	fclose( infile );
}

GLuint runTexture(){
	GLuint texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	std::cout << "Height: " << height<< std::endl;
	std::cout << "Width: " << width << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width , height , 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D,0);
	free(image);

	return texture;
}

float getHeight( int colPos, int rowPos){
	
	double averageWeight= 0;
	int denum =0 ;
	if (colPos  >  0 &&  rowPos > 0){
		averageWeight += heightmap[(rowPos-1)*width + colPos - 1];
		denum++;
	}
	if(colPos > 0 && rowPos < height){
		averageWeight += heightmap[(rowPos)*width + colPos - 1];
		denum++;
	}
	if(colPos < width && rowPos > 0){
		averageWeight += heightmap[(rowPos-1)*width + colPos];
		denum++;
	}
	if(colPos < width && rowPos < height){
		averageWeight += heightmap[(rowPos)*width + colPos];
		denum++;
	}

	return averageWeight/ ((double)denum);
}

/*
███╗   ███╗ █████╗ ██╗███╗   ██╗
████╗ ████║██╔══██╗██║████╗  ██║
██╔████╔██║███████║██║██╔██╗ ██║
██║╚██╔╝██║██╔══██║██║██║╚██╗██║
██║ ╚═╝ ██║██║  ██║██║██║ ╚████║
╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝
*/





int main(int argc , char** argv)
{
	read_texture_file (argv[2]);
	read_height_file(argv[1]);
	vertices = (Vertice *)malloc( width*height* sizeof(Vertice) * 6);

	for (int row=0; row< height ; row++){
		for(int col=0; col<width; col++){
			float texX = 1 - (float)col/width;
			float texY = 1-  (float)row/height;
			vec3 v1 = { 1, (getHeight(col+1,row)-getHeight(col,row))*20 , 0};
			vec3 v2 = { 0, (getHeight(col,row+1)-getHeight(col,row))*20 , 1};
			vec3 prod;
			vec3_mul_cross(prod,v1,v2);
			vec3_norm(prod,prod);
			vertices[row*width*6 + col*6] = { col, getHeight(col,row) , row, texX, texY , prod[0],prod[1], prod[2] };
			vertices[row*width*6 + col*6+1] = { col+1,getHeight(col+1,row) , row, texX, texY , prod[0], prod[1], prod[2]};
			vertices[row*width*6 + col*6+2] = { col, getHeight(col,row+1) , row+1, texX, texY, prod[0], prod[1], prod[2]};
			vertices[row*width*6 + col*6+3] = { col, getHeight(col,row+1) , row+1, texX, texY, prod[0], prod[1], prod[2] };
			vertices[row*width*6 + col*6+4] = { col+1,getHeight(col+1,row) , row, texX, texY, prod[0], prod[1], prod[2] };
			vertices[row*width*6 + col*6+5] = { col+1, getHeight(col+1,row+1) ,row+1, texX, texY, prod[0], prod[1], prod[2] };
		}
	}


    GLFWwindow* window;

    GLuint vertex_buffer;

    GLint mvp_location, vpos_location, vtex_location , vnor_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(1000, 1000, "Flat Earth", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
    if (glewInit() != GLEW_OK){
	    printf("glew cant start");
	    exit(EXIT_FAILURE);
    }
    // NOTE: OpenGL error checks have been omitted for brevity
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, width*height*6*sizeof(Vertice), vertices, GL_STATIC_DRAW);

    initShaders();

    glEnable(GL_TEXTURE_2D);
    gTextureMap = runTexture();

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vtex_location = glGetAttribLocation(program, "vTexCoord");
	vnor_location = glGetAttribLocation(program, "vnor");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 8, (void*) 0);
    glEnableVertexAttribArray(vtex_location);
    glVertexAttribPointer(vtex_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 8, (void*) (sizeof(float) * 3));
	glEnableVertexAttribArray(vnor_location);
	glVertexAttribPointer(vnor_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(float) * 8, (void*) (sizeof(float) * 5));
    double lastTime = glfwGetTime();

	
//	███╗   ███╗ █████╗ ██╗███╗   ██╗    ██╗      ██████╗  ██████╗ ██████╗ 
//	████╗ ████║██╔══██╗██║████╗  ██║    ██║     ██╔═══██╗██╔═══██╗██╔══██╗
//	██╔████╔██║███████║██║██╔██╗ ██║    ██║     ██║   ██║██║   ██║██████╔╝
//	██║╚██╔╝██║██╔══██║██║██║╚██╗██║    ██║     ██║   ██║██║   ██║██╔═══╝ 
//	██║ ╚═╝ ██║██║  ██║██║██║ ╚████║    ███████╗╚██████╔╝╚██████╔╝██║     
//	╚═╝     ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝    ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝     
	                                                                      
	eyelocation = {width/2 , width/10 , -width/4};
	pitch = 0;
	yaw = 0;
	speed = 0;
	hmult = 10;
	gaze[0] = 0;
	gaze[1] = 0;
	gaze[2] = 1;
	up[0]=0;
	up[1]=1;
	up[2]=0;
    while (!glfwWindowShouldClose(window)){
		float ratio;
        int wwidth, wheight;
        mat4x4 cam, p, mvp;
        glfwGetFramebufferSize(window, &wwidth, &wheight);
        ratio = wwidth / (float) wheight;
        glViewport(0, 0, wwidth, wheight);
        glClear(GL_COLOR_BUFFER_BIT);
        mat4x4_identity(cam);

		vec4 eye = {eyelocation.x ,eyelocation.y ,eyelocation.z , 1};
        vec4 center;
		vec4 gaze4 = {gaze[0], gaze[1], gaze[2] , 0};
		vec4 up4 = {up[0], up[1], up[2] , 0};
        vec4_add(center,eye , gaze4) ;
        mat4x4_look_at(cam, eye, center, up);

        eyelocation = {eyelocation.x + gaze[0] * speed ,eyelocation.y + gaze[1] * speed ,eyelocation.z + gaze[2] * speed};
        //mat4x4_rotate( pitchmat, mat4x4 M, float x, float y, float z, float angle);
        mat4x4_perspective(p, M_PI/4, 1, 0.1f , 10000.f);
        //mat4x4_ortho(p, -1000, 5000, -2500, 2500, 0.f, 20000.f);
        mat4x4_mul(mvp, p, cam);

    	
    	

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    	glUniform1i(glGetUniformLocation(program, "texturemap"), 0);
		glUniform1f(glGetUniformLocation(program, "hmult"), hmult);
    	glBindTexture(GL_TEXTURE_2D,gTextureMap);
    	glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, width*height*6);
	while (glfwGetTime() < lastTime + 1.0/TARGET_FPS)
	{
	}
	
	lastTime += 1.0/TARGET_FPS;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
