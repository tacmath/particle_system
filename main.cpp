#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>
#include <vector>
#include <iostream>
#include "kernel.hpp"
/*


 IsCLExtensionSupported( "cl_khr_gl_sharing" )

bool IsCLExtensionSupported(const char* extension)
{
	// see if the extension is bogus:
	if (extension == NULL || extension[0] == '\0')
		return false;
	char* where = (char*)strchr(extension, ' ');
	if (where != NULL)
		return false;
	// get the full list of extensions:
	size_t extensionSize;
	clGetDeviceInfo(Device, CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
	char* extensions = new char[extensionSize];
	clGetDeviceInfo(Device, CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);
	for (char* start = extensions; ; )
	{
		where = (char*)strstr((const char*)start, extension);
		if (where == 0)
		{
			delete[] extensions;
			return false;
		}
		char* terminator = where + strlen(extension); // points to what should be the separator
		if (*terminator == ' ' || *terminator == '\0' || *terminator == '\r' || *terminator == '\n')
		{
			delete[] extensions;
			return true;
		}
		start = terminator;
	}
}

For Windows :
cl_context_properties props[] =
{
CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
CL_CONTEXT_PLATFORM, (cl_context_properties)Platform,
0
};
cl_context Context = clCreateContext(props, 1, &Device, NULL, NULL, &status);
For Linux :
cl_context_properties props[] =
{
CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
CL_CONTEXT_PLATFORM, (cl_context_properties)Platform,
0
};
cl_context Context = clCreateContext(props, 1, &Device, NULL, NULL, &status);
For Apple :
cl_context_properties props[] =
{
CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
(cl_context_properties)kCGLShareGroup,
0
};
cl_context Context = clCreateContext(props, 0, 0, NULL, NULL, &status);




void
InitCL( )
{
. . .
// create the velocity array and the opengl vertex array buffer and color array buffer:
delete [ ] hVel;
hVel = new struct xyzw [ NUM_PARTICLES ];
glGenBuffers( 1, &hPobj );
glBindBuffer( GL_ARRAY_BUFFER, hPobj );
glBufferData( GL_ARRAY_BUFFER, 4 * NUM_PARTICLES * sizeof(float), NULL, GL_STATIC_DRAW );glGenBuffers( 1, &hCobj );
glBindBuffer( GL_ARRAY_BUFFER, hCobj );
glBufferData( GL_ARRAY_BUFFER, 4 * NUM_PARTICLES * sizeof(float), NULL, GL_STATIC_DRAW );glBindBuffer( GL_ARRAY_BUFFER, 0 ); // unbind the buffer
// fill those arrays and buffers:
ResetParticles( );


void
ResetParticles( )
{
glBindBuffer( GL_ARRAY_BUFFER, hPobj );
struct xyzw *points = (struct xyzw *) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );for( int i = 0; i < NUM_PARTICLES; i++ )
{
points[ i ].x = Ranf( &Seed, XMIN, XMAX );
points[ i ].y = Ranf( &Seed, YMIN, YMAX );
points[ i ].z = Ranf( &Seed, ZMIN, ZMAX );
points[ i ].w = 1.;
}
glUnmapBuffer( GL_ARRAY_BUFFER );
glBindBuffer( GL_ARRAY_BUFFER, hCobj );
struct rgba *colors = (struct rgba *) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );for( int i = 0; i < NUM_PARTICLES; i++ )
{
colors[ i ].r = Ranf( &Seed, 0., 1. );
colors[ i ].g = Ranf( &Seed, 0., 1. );
colors[ i ].b = Ranf( &Seed, 0., 1. );
colors[ i ].a = 1.;
}
glUnmapBuffer( GL_ARRAY_BUFFER );

for( int i = 0; i < NUM_PARTICLES; i++ )
{
hVel[ i ].x = Ranf( &Seed, VMIN, VMAX );
hVel[ i ].y = Ranf( &Seed, 0. , VMAX );
hVel[ i ].z = Ranf( &Seed, VMIN, VMAX );
hVel[ i ].w = 0.;
}
}


InitCL( )
{
. . .
// 5. create the opencl version of the velocity array:
dVel = clCreateBuffer( Context, CL_MEM_READ_WRITE, 4*sizeof(float)*NUM_PARTICLES, NULL, &status );PrintCLError( status, "clCreateBuffer: " );
// 6. write the data from the host buffers to the device buffers:
status = clEnqueueWriteBuffer( CmdQueue, dVel, CL_FALSE, 0, 4*sizeof(float)*NUM_PARTICLES, hVel, 0, NULL, NULL );PrintCLError( status, "clEneueueWriteBuffer: " );
// 5. create the opencl version of the opengl buffers:
dPobj = clCreateFromGLBuffer( Context, CL_MEM_READ_WRITE, hPobj, &status );
PrintCLError( status, "clCreateFromGLBuffer (1)" );
dCobj = clCreateFromGLBuffer( Context, CL_MEM_READ_WRITE , hCobj, &status );
PrintCLError( status, "clCreateFromGLBuffer (2)" )


dPobj = clCreateFromGLBuffer( Context, CL_MEM_READ_WRITE, hPobj, &status );
PrintCLError( status, "clCreateFromGLBuffer (1)" );

// 10. setup the arguments to the Kernel object:
status = clSetKernelArg( Kernel, 0, sizeof(cl_mem), &dPobj );
PrintCLError( status,"clSetKernelArg (1): " );
status = clSetKernelArg( Kernel, 1, sizeof(cl_mem), &dVel );
PrintCLError( status , "clSetKernelArg (2): " );
status = clSetKernelArg( Kernel, 2, sizeof(cl_mem), &dCobj );
PrintCLError( status, "clSetKernelArg (3): " );




glutSetWindow( MainWindow );
glFinish( );
cl_int status;
status = clEnqueueAcquireGLObjects( CmdQueue, 1, &dPobj, 0, NULL, NULL );
PrintCLError( status, “clEnqueueAcquireGLObjects (1) : “ );
status = clEnqueueAcquireGLObjects( CmdQueue, 1, &dCobj, 0, NULL, NULL );
PrintCLError( status, “clEnqueueAcquireGLObjects (2) : “ );
Wait( ); // note: only need to wait here because doing timing
double time0 = omp_get_wtime( );
// 11. enqueue the Kernel object for execution:
cl_event wait;
status = clEnqueueNDRangeKernel( CmdQueue, Kernel, 1, NULL, GlobalWorkSize, LocalWorkSize, 0, NULL, &wait );PrintCLError( status, "clEnqueueNDRangeKernel: " );
Wait( ); // note: only need to wait here because doing timing
double time1 = omp_get_wtime( );
ElapsedTime = time1 - time0;
clEnqueueReleaseGLObjects( CmdQueue, 1, &dCobj, 0, NULL, NULL );
PrintCLError( status, “clEnqueueReleaseGLObjects (1): “ );
clEnqueueReleaseGLObjects( CmdQueue, 1, &dPobj, 0, NULL, NULL );
PrintCLError( status, “clEnqueueReleaseGLObject (2): “ );
Wait( );
glutSetWindow( MainWindow );
glutPostRedisplay( );

glBindBuffer( GL_ARRAY_BUFFER, hPobj );
glVertexPointer( 4, GL_FLOAT, 0, (void *)0 );
glEnableClientState( GL_VERTEX_ARRAY );
glBindBuffer( GL_ARRAY_BUFFER, hCobj );
glColorPointer( 4, GL_FLOAT, 0, (void *)0 );
glEnableClientState( GL_COLOR_ARRAY );
glPointSize( 2. );
glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
glPointSize( 1. );
glDisableClientState( GL_VERTEX_ARRAY );
glDisableClientState( GL_COLOR_ARRAY );
glBindBuffer( GL_ARRAY_BUFFER, 0 );
glutSwapBuffers( );
glFlush( );
*/

int main() {
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl::Platform::get(&platforms);

	_ASSERT(platforms.size() > 0);
	cl::Platform platform = platforms.front();

	std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	
	_ASSERT(devices.size() > 0);

	for (auto& device : devices) {
		std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
		std::cout << "Vendor : " << device.getInfo<CL_DEVICE_VENDOR>() << "\n";
		std::cout << "Version : " << device.getInfo<CL_DEVICE_VERSION>() << "\n";
	}

	cl::Context context({ devices.front() });

	int A_h[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int B_h[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

	cl::Buffer A_d(context, CL_MEM_READ_WRITE, sizeof(A_h));
	cl::Buffer B_d(context, CL_MEM_READ_WRITE, sizeof(B_h));

	cl::CommandQueue queue(context, devices.front());

	queue.enqueueWriteBuffer(A_d, CL_TRUE, 0, sizeof(A_h), A_h);
	queue.enqueueWriteBuffer(B_d, CL_TRUE, 0, sizeof(B_h), B_h);

	std::string kernel_code =
		"   void kernel simple_add(global const int* A, global const int* B, global int* C){ "
		"       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];                 "
		"   } ";

	cl::Program::Sources sources;
	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl::Program program(context, sources);
	if (program.build({ devices.front() }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << "\n";
		exit(1);
	}

	Kernel kernel, kernel2, k3;

	kernel.Build(context, devices.front(), {"kernel.cl"});

	kernel2.Build(context, devices.front(), { "particle.cl" });

	k3.Build(context, devices.front(), { "example_partile.cl" });

	return 0;
}