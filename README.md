This is a very minimal package to just get a gl.Context
(golang.org/x/mobile/gl) which can be used for off-screen rendering. It uses
EGL to create a 1x1 pixel pbuffer surface (if EGL_KHR_surfaceless_context is
supported, then this actually wouldn't even be necessary) to get a valid context
for drawing.

To use this package, simply call hlegl.Initialise(), and then use the returned
context like a gl.Context. Drawing should only be done into FBOs. The rendered
result can be accessed via ReadPixels() or other methods (PBOs, EGLImage, etc).

There's a very minimal example in example/
