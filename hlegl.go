package hlegl

// #cgo LDFLAGS: -lEGL
// #include "egl.h"
import "C"

import (
	"runtime"
	"golang.org/x/mobile/gl"
)

type Context struct {
	gl.Context

	eglContext *C.struct_egl_context
	eglSurface *C.struct_egl_surface

	stopChan chan bool
	finished chan bool
}

func Initialise() *Context {
	ctx := Context{
		stopChan: make(chan bool),
		finished: make(chan bool),
	};
	retChan := make(chan gl.Context)

	go func() {
		runtime.LockOSThread()

		eglCtx := C.egl_new_context();
		if eglCtx == nil {
			retChan<- nil
			close(retChan)
			return
		}

		eglSurf := C.egl_new_pbuffer_surface(eglCtx, 1, 1)
		if eglCtx == nil {
			retChan<- nil
			close(retChan)
			return
		}

		ret := C.egl_make_current(eglCtx, eglSurf)
		if ret < 0 {
			retChan<- nil
			close(retChan)
			return
		}

		ctx.eglContext = eglCtx
		ctx.eglSurface = eglSurf

		glctx, worker := gl.NewContext()
		retChan<- glctx
		close(retChan)

		workAvailable := worker.WorkAvailable()

		for {
			select {
			case <-workAvailable:
				worker.DoWork()
			case <-ctx.stopChan:
				ctx.finished<- true
				close(ctx.finished)
				return
			}
		}
	}()

	ctx.Context = <-retChan
	if ctx.Context == nil {
		return nil
	}

	return &ctx
}

func (c *Context) Stop() {
	c.stopChan<- true
	<-c.finished
	C.egl_destroy_surface(c.eglContext, c.eglSurface)
	C.egl_destroy_context(c.eglContext)
}
