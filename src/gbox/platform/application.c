/*!The Graphic Box Library
 * 
 * GBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * GBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2014 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        application.c
 * @ingroup     platform
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../gbox.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the application impl type
typedef struct __gb_application_impl_t
{
    // the arguments count
    tb_size_t                   argc;

    // the arguments list
    tb_char_t**                 argv;

    // the private data
    tb_cpointer_t               priv;

    // the window info
    gb_window_info_t            info;

    // the func
    gb_application_func_t       func;

    // the window
    gb_window_ref_t             window;

    // loaded?
    tb_bool_t                   loaded;

}gb_application_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the application
static gb_application_ref_t     g_application = tb_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t gb_application_init(gb_window_ref_t window, gb_canvas_ref_t canvas, tb_cpointer_t priv)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)g_application;
    tb_assert_and_check_return_val(impl, tb_false);

    // notify: init window
    if (impl->info.init && !impl->info.init(window, canvas, priv)) return tb_false;

    // notify: application have been loaded
    if (impl->func.loaded && !impl->func.loaded((gb_application_ref_t)impl)) return tb_false;

    // ok
    return tb_true;
}
static tb_void_t gb_application_exit(gb_window_ref_t window, gb_canvas_ref_t canvas, tb_cpointer_t priv)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)g_application;
    tb_assert_and_check_return(impl);

    // notify: exit application
    if (impl->func.exit) impl->func.exit((gb_application_ref_t)impl);

    // notify: exit window
    if (impl->info.exit) impl->info.exit(window, canvas, priv);
}
static tb_void_t gb_application_event(gb_window_ref_t window, gb_event_ref_t event, tb_cpointer_t priv)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)g_application;
    tb_assert_and_check_return(impl);

    // notify: event
    if (impl->info.event) impl->info.event(window, event, priv);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
gb_application_ref_t gb_application()
{
    return g_application;
}
tb_int_t gb_application_main(tb_int_t argc, tb_char_t** argv, gb_application_func_ref_t func)
{
    // done
    tb_int_t                ok = -1;
    gb_application_impl_t*  impl = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(func && func->init);

        // init tbox
        if (!tb_init(tb_null, tb_null, 0)) break;

        // init gbox
        if (!gb_init()) break;

        // make application
        impl = tb_malloc0_type(gb_application_impl_t);
        tb_assert_and_check_break(impl);

        // save application
        g_application = (gb_application_ref_t)impl;

        // init arguments
        impl->argc = argc;
        impl->argv = argv;

        // save func
        impl->func = *func;

        // init window info
        impl->info.framerate    = 60;
        impl->info.flag         = GB_WINDOW_FLAG_NONE;

        // notify: init application
        if (!func->init((gb_application_ref_t)impl, &impl->info)) break;

        // init window info
        gb_window_info_t info   = impl->info;
        info.init               = gb_application_init;
        info.exit               = gb_application_exit;
        info.event              = gb_application_event;

        // init window
#if defined(GB_CONFIG_APP_WINDOW_SDL)
        impl->window = gb_window_init_sdl(&info);
#elif defined(GB_CONFIG_APP_WINDOW_FRAMEBUFFER)
        impl->window = gb_window_init_framebuffer(&info);
#else
        impl->window = gb_window_init(&info);
#endif
        tb_assert_and_check_break(impl->window);

        // loop window
        gb_window_loop(impl->window);

        // ok
        ok = 0;

    } while (0);

    // clear application 
    g_application = tb_null;

    // exit application
    if (impl) 
    {
        // exit window
        if (impl->window) gb_window_exit(impl->window);
        impl->window = tb_null;

        // exit it
        tb_free(impl);
    }
    impl = tb_null;

    // exit gbox
    gb_exit();

    // exit tbox
    tb_exit();

    // ok?
    return ok;
}
tb_size_t gb_application_argc(gb_application_ref_t application)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)application;
    tb_assert_and_check_return_val(impl, 0);

    // the argc
    return impl->argc;
}
tb_char_t** gb_application_argv(gb_application_ref_t application)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)application;
    tb_assert_and_check_return_val(impl, tb_null);

    // the argv
    return impl->argv;
}
tb_cpointer_t gb_application_getp(gb_application_ref_t application)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)application;
    tb_assert_and_check_return_val(impl, tb_null);

    // the private data
    return impl->priv;
}
tb_void_t gb_application_setp(gb_application_ref_t application, tb_cpointer_t priv)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)application;
    tb_assert_and_check_return(impl);

    // done
    impl->priv = priv;
}
gb_window_ref_t gb_application_window(gb_application_ref_t application)
{
    // check
    gb_application_impl_t* impl = (gb_application_impl_t*)application;
    tb_assert_and_check_return_val(impl, tb_null);

    // the window
    return impl->window;
}