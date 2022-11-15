/*
 * Copyright © 2010-2011 Linaro Limited
 * Copyright © 2013 Canonical Ltd
 *
 * This file is part of the glmark2 OpenGL (ES) 2.0 benchmark.
 *
 * glmark2 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * glmark2 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * glmark2.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *  Alexandros Frantzis
 */
#include "native-state-x11.h"
#include "log.h"

#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <cstring>
/******************
 * Public methods *
 ******************/

NativeStateX11::~NativeStateX11()
{
    if (xdpy_)
    {
        if (xwin_)
            XDestroyWindow(xdpy_, xwin_);

        XCloseDisplay(xdpy_);
    }
}

bool
NativeStateX11::init_display()
{
    if (!xdpy_)
        xdpy_ = XOpenDisplay(NULL);

    return (xdpy_ != 0);
}

void*
NativeStateX11::display()
{
    return (void*)xdpy_;
}

bool
NativeStateX11::create_window(WindowProperties const& properties)
{

    if(strcmp(getenv("stdout_headless"),"1")==0){

        int pbufferWidth = 800;
        int pbufferHeight = 600;

        char* stdout_size;
        stdout_size = getenv ("stdout_size");

        if (stdout_size!=NULL){
            // https://www.codingunit.com/c-reference-stdlib-h-function-getenv
            Log::debug("stdout_size: %s\n",stdout_size);
            char string[50];
            // https://stackoverflow.com/questions/48673332/convert-char-to-array-of-chars
            strcpy(string, stdout_size);
            Log::debug("stdout_size: %s\n",string);
            // Extract the first token
            // https://www.educative.io/answers/splitting-a-string-using-strtok-in-c
            char * token = strtok(string, "x");
            Log::debug( "width:%s\n", token ); //printing the token
            // https://stackoverflow.com/questions/2523467/how-to-split-a-string-to-2-strings-in-c
            char * token2 = strtok(NULL, "x");
            Log::debug( "height: %s\n", token2 );
            // https://www.educative.io/answers/how-to-convert-a-string-to-an-integer-in-c
            int width = atoi(token);
            int height = atoi(token2);
            pbufferWidth = width;
            pbufferHeight = height;
        }
        printf ("stdout_size: %dx%d\n",pbufferWidth, pbufferHeight);
        properties_.width = pbufferWidth;
        properties_.height = pbufferHeight;
        return true;
    } else {

    }

    static const char *win_name("glmark2 " GLMARK_VERSION);

    if (!xdpy_) {
        Log::error("Error: X11 Display has not been initialized!\n");
        return false;
    }

    /* Recreate an existing window only if it has actually been resized */
    if (xwin_) {
        if (properties_.fullscreen != properties.fullscreen ||
            (properties.fullscreen == false &&
             (properties_.width != properties.width ||
              properties_.height != properties.height)))
        {
            XDestroyWindow(xdpy_, xwin_);
            xwin_ = 0;
        }
        else
        {
            return true;
        }
    }

    /* Set desired attributes */
    properties_.fullscreen = properties.fullscreen;
    properties_.visual_id = properties.visual_id;

    if (properties_.fullscreen) {
        /* Get the screen (root window) size */
        XWindowAttributes window_attr;
        XGetWindowAttributes(xdpy_, RootWindow(xdpy_, DefaultScreen(xdpy_)), 
                             &window_attr);
        properties_.width = window_attr.width;
        properties_.height = window_attr.height;
    }
    else {
        properties_.width = properties.width;
        properties_.height = properties.height;
    }

    XVisualInfo vis_tmpl;
    XVisualInfo *vis_info = 0;
    int num_visuals;

    /* The X window visual must match the supplied visual id */
    vis_tmpl.visualid = properties_.visual_id;
    vis_info = XGetVisualInfo(xdpy_, VisualIDMask, &vis_tmpl,
                             &num_visuals);
    if (!vis_info) {
        Log::error("Error: Could not get a valid XVisualInfo!\n");
        return false;
    }

    Log::debug("Creating XWindow W: %d H: %d VisualID: 0x%x\n",
               properties_.width, properties_.height, vis_info->visualid);

    /* window attributes */
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root = RootWindow(xdpy_, DefaultScreen(xdpy_));

    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(xdpy_, root, vis_info->visual, AllocNone);
    attr.event_mask = KeyPressMask;
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    xwin_ = XCreateWindow(xdpy_, root, 0, 0, properties_.width, properties_.height,
                          0, vis_info->depth, InputOutput,
                          vis_info->visual, mask, &attr);

    XFree(vis_info);

    if (!xwin_) {
        Log::error("Error: XCreateWindow() failed!\n");
        return false;
    }

    /* set hints and properties */
    Atom fs_atom = None;
    if (properties_.fullscreen) {
        fs_atom = XInternAtom(xdpy_, "_NET_WM_STATE_FULLSCREEN", True);
        if (fs_atom == None)
            Log::debug("Warning: Could not set EWMH Fullscreen hint.\n");
    }
    if (fs_atom != None) {
        XChangeProperty(xdpy_, xwin_,
                        XInternAtom(xdpy_, "_NET_WM_STATE", True),
                        XA_ATOM, 32, PropModeReplace,
                        reinterpret_cast<unsigned char*>(&fs_atom),  1);
    }
    else {
        XSizeHints sizehints;
        sizehints.min_width  = properties_.width;
        sizehints.min_height = properties_.height;
        sizehints.max_width  = properties_.width;
        sizehints.max_height = properties_.height;
        sizehints.flags = PMaxSize | PMinSize;

        XSetWMProperties(xdpy_, xwin_, NULL, NULL,
                         NULL, 0, &sizehints, NULL, NULL);
    }

    /* Set the window name */
    XStoreName(xdpy_ , xwin_,  win_name);

    /* Gracefully handle Window Delete event from window manager */
    Atom wmDelete = XInternAtom(xdpy_, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(xdpy_, xwin_, &wmDelete, 1);

    return true;
}

void*
NativeStateX11::window(WindowProperties& properties)
{
    properties = properties_;
    return (void*)xwin_;
}

void
NativeStateX11::visible(bool visible)
{
    if (visible)
        XMapWindow(xdpy_, xwin_);
}

bool
NativeStateX11::should_quit()
{
    XEvent event;

    if (!XPending(xdpy_))
        return false;

    XNextEvent(xdpy_, &event);

    if (event.type == KeyPress) {
        if (XLookupKeysym(&event.xkey, 0) == XK_Escape)
            return true;
    }
    else if (event.type == ClientMessage) {
        /* Window Delete event from window manager */
        return true;
    }

    return false;
}
