#include "EGL/egl.h"

struct egl_context {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
};

struct egl_surface {
	EGLSurface surface;
};

struct egl_context *egl_new_context();
void egl_destroy_context(struct egl_context *ctx);
struct egl_surface *egl_new_pbuffer_surface(struct egl_context *ctx, int width, int height);
void egl_destroy_surface(struct egl_context *ctx, struct egl_surface *surface);
int egl_make_current(struct egl_context *ctx, struct egl_surface *surface);
