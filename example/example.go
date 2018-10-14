package main

import (
	"fmt"
	"image"
	"image/png"
	"os"

	"github.com/usedbytes/hlegl"
	"golang.org/x/mobile/gl"
)

func main() {
	fmt.Println("Hello hlegl")

	// Set up the headless EGL rendering context
	glctx := hlegl.Initialise()

	// Create and bind an FBO to render into
	fbo := glctx.CreateFramebuffer()
	fboTex := glctx.CreateTexture()
	glctx.BindFramebuffer(gl.FRAMEBUFFER, fbo)
	glctx.BindTexture(gl.TEXTURE_2D, fboTex)
	glctx.TexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 64, 64, gl.RGBA, gl.UNSIGNED_BYTE, nil)
	glctx.FramebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, fboTex, 0)
	result := glctx.CheckFramebufferStatus(gl.FRAMEBUFFER)
	if result != gl.FRAMEBUFFER_COMPLETE {
		fmt.Println("Framebuffer not complete.")
		os.Exit(1)
	}

	// Draw something
	glctx.ClearColor(1.0, 0.0, 1.0, 1.0)
	glctx.Clear(gl.COLOR_BUFFER_BIT)
	glctx.Finish()

	// Read it out into a PNG image
	dst := make([]uint8, int(64 * 64 * 4))
	glctx.ReadPixels(dst, 0, 0, 64, 64, gl.RGBA, gl.UNSIGNED_BYTE)
	img := image.NRGBA{
		Pix: dst,
		Stride: 64 * 4,
		Rect: image.Rect(0, 0, 64, 64),
	}

	f, err := os.Create("image.png")
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	if err := png.Encode(f, &img); err != nil {
		f.Close()
		fmt.Println(err)
		os.Exit(1)
	}

	if err := f.Close(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	glctx.Stop()
}
