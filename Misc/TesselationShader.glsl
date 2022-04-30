
/*
	Tessellation shaders follow the vertex shader in the shader
	pipeline. They take vertex data and can interpolate the original
	vertices to create additional vertices in your geometry.
	(Note that this interpolation is quite different from the interpolations
	in fragment shaders.) 
	

	There are two kinds of Tessellation shaders: 
	Tessellation Control shadres let you setup the parameters
	for the interpolations to be carried out.
	Tessellation evaluation shaders let you define the computation
	that will be used in creating the actual output geometry.

	Example of tessellation: A Bezier surface interpolated from a 4x4
	patch by tessellation shaders.
	
 */
