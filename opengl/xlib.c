#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/X.h>

#include <glad/gl.h>
#include <glad/glx.h>




int main(void) {

    Display *dpy = XOpenDisplay(NULL);
    assert(dpy != NULL);

    int scr         = XDefaultScreen(dpy);
    Window win_root = XRootWindow(dpy, scr);

    int att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

    XVisualInfo *vis = glXChooseVisual(dpy, scr, att);
    assert(vis != NULL);

    Colormap cmap = XCreateColormap(dpy, win_root, vis->visual, AllocNone);

    XSetWindowAttributes swa = { 0 };
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;
    Window win = XCreateWindow(dpy, win_root, 0, 0, 600, 600, 0, vis->depth, InputOutput, vis->visual, CWColormap | CWEventMask, &swa);


    XMapRaised(dpy, win);

    GLXContext glc = glXCreateContext(dpy, vis, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
    glEnable(GL_DEPTH_TEST); 

    XEvent xev = { 0 };
    while(1) {
        XNextEvent(dpy, &xev);
    }




    XCloseDisplay(dpy);

    return EXIT_SUCCESS;
}
