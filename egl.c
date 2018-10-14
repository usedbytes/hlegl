#include <stdio.h>
#include <stdlib.h>

#include "egl.h"

struct egl_context *egl_new_context()
{
	static const EGLint config_attributes[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_NONE,
	};
	static const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE,
	};
	EGLBoolean result;
	EGLint num_config;

	struct egl_context *ctx = calloc(1, sizeof(*ctx));
	if (!ctx) {
		return NULL;
	}

	ctx->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (ctx->display == EGL_NO_DISPLAY) {
		fprintf(stderr, "Couldn't get display %08x\n", eglGetError());
		goto fail;
	}

	result = eglInitialize(ctx->display, NULL, NULL);
	if (result == EGL_FALSE) {
		fprintf(stderr, "Couldn't initialise %08x\n", eglGetError());
		goto fail;
	}

	result = eglChooseConfig(ctx->display, config_attributes, &ctx->config, 1, &num_config);
	if (result == EGL_FALSE) {
		fprintf(stderr, "Couldn't get config %08x\n", eglGetError());
		goto fail;
	}

	ctx->context = eglCreateContext(ctx->display, ctx->config, EGL_NO_CONTEXT, context_attributes);
	if (ctx->context == EGL_NO_CONTEXT) {
		fprintf(stderr, "Couldn't get context %08x\n", eglGetError());
		goto fail;
	}

	return ctx;

fail:
	free(ctx);
	return NULL;
}

void egl_destroy_context(struct egl_context *ctx)
{
	eglMakeCurrent(ctx->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	EGLBoolean result = eglDestroyContext(ctx->display, ctx->context);
	if (result == EGL_FALSE) {
		fprintf(stderr, "Couldn't destroy context %08x\n", eglGetError());
	}

	eglTerminate(ctx->display);
	if (result == EGL_FALSE) {
		fprintf(stderr, "Couldn't destroy display %08x\n", eglGetError());
	}

	free(ctx);
}

struct egl_surface *egl_new_pbuffer_surface(struct egl_context *ctx, int width, int height)
{
	EGLint pbuffer_attributes[] = {
		EGL_WIDTH, width,
		EGL_HEIGHT, height,
		EGL_NONE,
	};

	struct egl_surface *surface = calloc(1, sizeof(*surface));
	if (!surface) {
		return NULL;
	}

	surface->surface = eglCreatePbufferSurface(ctx->display, ctx->config, pbuffer_attributes);
	if (surface->surface == EGL_NO_SURFACE) {
		fprintf(stderr, "Couldn't make surface %08x\n", eglGetError());
		goto fail;
	}

	return surface;

fail:
	free(surface);
	return NULL;
}

void egl_destroy_surface(struct egl_context *ctx, struct egl_surface *surface)
{
	EGLBoolean result = eglDestroySurface(ctx->display, surface->surface);
	if (result == EGL_FALSE) {
		fprintf(stderr, "Couldn't destroy surface %08x\n", eglGetError());
	}

	free(surface);
}

int egl_make_current(struct egl_context *ctx, struct egl_surface *surface)
{
	EGLBoolean result = eglMakeCurrent(ctx->display, surface->surface, surface->surface, ctx->context);

	return result == EGL_TRUE ? 0 : -1;
}
