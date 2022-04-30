/*
	RENDER TO TEXTURE

	Textures need not come only from files or from computation, 
	as our previous examples have done. You can also render an image
	into a texture and then use that image as a texture.

	The render-to-texture operation uses the framebuffer extension, that is, 
	GL_framebuffer_object, so you need to be sure this is supported on your system.

	1. You will be changing the display destination. Generate a handle for a framebuffer
		object, and generate handles for a (depth) renderbuffer object and for a texture
		object. (These will later be attached to the framebuffer object.)
	2. Bind the framebuffer object to the context.
	3. Bind the depth renderbuffer object to the context.
		Assign storage attributes to it.
		Attach it to the framebuffer object.
	4. Bind the texture object to the context.
		Assign storage attributes to it.
		Assign texture parameters to it.
		Attach it to the framebuffer object.
	5. Render as normal.
	6. Un-bind the framebuffer object from the context.
 */

// generate FrameBuffer handle, RenderBuffer handle, Texture handle
 GLuint Framebuffer;
 GLuint DepthBuffer;
 GLuint Texture;

 glGenFramebuffers(1, &Framebuffer);
 glGenRenderBuffers(1, &DepthBuffer);
 glGenTextures(1, &Texture);

 // set up the size for the rendered texture
 int sizeX = 2048;
 int sizeY = 2048;

 // Bind the offscreen framebuffer to be the current output display
 glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

 // Bind the Depth Buffer to the context, allocate its storage, and attach it to the Framebuffer
 glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);
 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sizeX, sizeY);
 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);

 // Bind the Texture to the context
 glBindTexture(GL_TEXTURE_2D, Texture);

 // Set up a NULL texture of the size you want to render into
 // and set its properties
 glTexImage2D(GL_TEXTURE_2D, 0, 4, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

 // Tell OpenGL that you are going to render into the color planes of the Texture
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture, 0);

 // see if OpenGL thinks the framebuffer is complete enough to use:
 GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
 if(status != GL_FRAMEBUFFER_COMPLETE)
 	fprintf(stderr, "Framebuffer is not complete!\n");

// render as normal; be sure to set the viewport to match the
// size of the color and depth buffers
glClearColor(0, 0.2, 0, 1);
glClear(GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT);
glEnable(GL_DEPTH_TEST);
glShadeModel(GL_FLAT);
glViewport(0, 0, sizeX, sizeY);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(90, 1, 0.1, 1000);

glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

glTranslatef(TransXYZ[0], TransXYZ[1], TransXYZ[2]);
glMultMatrixf(RotMatrix);
glScalef(scale, scale, scale);
glColor3f(1, 1, 1);

gluWireTeapot(1);

// Tell OpenGL to go back to rendering to the hardware framebuffer:
glBindFramebuffer(GL_FRAMEBUFFER, 0);

// if you want, have OpenGL create the multiple mipmap layers for you
glGenerateMipmap(GL_TEXTURE_2D);

// now render the rest of the scene as normal, using the texture
// as you normally would:

glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, Texture);
glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(-1, -1);
    glTexCoord2f(1, 0);
    glVertex2f(1, -1);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(-1, 1);
glEnd();

glDisable(GL_TEXTURE_2D);
