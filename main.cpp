#include "particle_system.hpp"
//#include "utils.hpp"
/*



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

std::string glStringEnum(GLenum glenum)
{
    std::string enumString = "NONE";

    switch (glenum)
    {
    case GL_INVALID_ENUM:                   enumString = "INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                  enumString = "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:              enumString = "INVALID_OPERATION"; break;
    case GL_OUT_OF_MEMORY:                  enumString = "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  enumString = "INVALID_FRAMEBUFFER_OPERATION"; break;

    case GL_DEBUG_SEVERITY_MEDIUM:          enumString = "GL_DEBUG_SEVERITY_MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH:            enumString = "GL_DEBUG_SEVERITY_HIGH"; break;
    case GL_DEBUG_SEVERITY_LOW:             enumString = "GL_DEBUG_SEVERITY_LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:    enumString = "GL_DEBUG_SEVERITY_NOTIFICATION"; break;

    case GL_DEBUG_SOURCE_API:               enumString = "GL_DEBUG_SOURCE_API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     enumString = "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   enumString = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:       enumString = "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:       enumString = "GL_DEBUG_SOURCE_APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:             enumString = "GL_DEBUG_SOURCE_OTHER"; break;

    case GL_DEBUG_TYPE_ERROR:               enumString = "GL_DEBUG_TYPE_ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: enumString = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  enumString = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         enumString = "GL_DEBUG_TYPE_PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         enumString = "GL_DEBUG_TYPE_PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER:              enumString = "GL_DEBUG_TYPE_MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          enumString = "GL_DEBUG_TYPE_PUSH_GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           enumString = "GL_DEBUG_TYPE_POP_GROUP"; break;
    case GL_DEBUG_TYPE_OTHER:               enumString = "GL_DEBUG_TYPE_OTHER"; break;
    }
    return enumString;
}



static void debug(void) {
    std::cout << glGetString(GL_VERSION) << std::endl;
    glfwSetErrorCallback([](int error, const char* description)
        {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        });
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        (void)id;
    (void)userParam;
    (void)length;
    if (type != GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR && (severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR))
        std::cerr << "Opengl : " << " severity : " << glStringEnum(severity) << " type : " << glStringEnum(type) << " source : " << glStringEnum(source) << std::endl << "message : " << message << std::endl << std::endl;
        }, 0);
}


int main() {
	ParticleSystem particleSystem;

    debug();
	particleSystem.Start();
	particleSystem.Run();
	particleSystem.Stop();

	return 0;
}