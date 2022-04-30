
/*

There are special variables that give you access to the 
data set by an OpenGL application into on-card registers, several
special-purpose operations on vectors and matrices that are
designed specifically for graphics, special variable types to
reflect the different kinds of operations that will be done with
variables, and shared name spaces that provide communication between
applications, vertex shaders and fragment shaders.


Goals: Primary: speed, secondary: image quality
Shader types: Vertex, Tessellation Control, Tessellation Evaluation, Geometry, Fragment
Shader Variables: Attribute, Uniform, Constant, Out, In
Coordinate Systems: Model, World, Eye, Clip
Noise: Either as a texture or using the built-in function
Compile Shaders: Done by the driver


Vector Constructors:

vec3(float, float, float)
vec4(ivec4)
vec2(float)         Initializes a vec2 with the float value in each position.
ivec3(int, int, int)
bvec4(int, int, float, float)   Performs four boolean conversions.
vec2(vec3)
vec3(vec4)
vec3(vec2, float)
vec3(float, vec2)
vec4(vec3, float)
vec4(float, vec3)
vec4(vec2a, vec2b)

mat2(vec2, vec2)            Each matrix is filled using one column per argument.
mat3(vec3, vec3, vec3)
mat4(vec4, vec4, vec4)

mat2(float, float, float, float) Rows are first column and second column, respectively
mat3(float, float, float,        Rows are first column, second column, and
     float, float, float,        third column, respectively.
     float, float, float)
mat4(float, float, float, float,    Rows are first column, second column, third column, and
     float, float, float, float,    column, and fourth column, respectively.
     float, float, float, float,
     float, float, float, float)
mat2x3(vec2, float,             Rows are first column and second column, respectively.
       vec2, float)
 
mat3x3(mat4x4)          Uses the upper left 3x3 submatrix of the mat4x4 matrix.
mat2x3(mat4x2)          Takes the upper left 2x2 submatrix of the mat4x2, and sets
                        the last column to vec2(0)

FUNCTIONS EXTENDED TO MATRICES AND VECTORS
-------------------------------------------

genType radians(genType degrees)        Converts degrees to radians (PI / 180) * degrees.
genType degrees(genType radians)        Converts radians to degrees (180 / PI) * radians.
genType sin(genType angle)
genType cos(genType angle)
genType tan(genType angle)
genType asin(genType x)
genType acos(genType x)
genType atan(genType y, genType x)
genType atan(genType y_over_x)

genType pow(genType x, genType y)
genType exp(genType x)
genType log(genType x)
genType exp2(genType x)
genType log2(genType x)
genType sqrt(genType x)
genType inversesqrt(genType x)

NOTE: Don't use inversesqrt() to normalize a vector! Use normalize() instead.

genType abs(genType x)
genType sign(genType x)
genType floor(genType x)
genType ceil(genType x)
genType fract(genType x)
genType truncate(genType x)         Returns the integer closest to x whose absolute valueu isn't larger than abs(x)
genType round(genType x)            Returns the integer closest to x
genType mod(genType x, genType y)   Generalized modulus, Returns x - y * floor(x /  y).


*/