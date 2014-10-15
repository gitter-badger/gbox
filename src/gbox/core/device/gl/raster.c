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
 * @file        raster.c
 * @ingroup     core
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "gl_raster"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "raster.h"
#include "../../impl/raster.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the gl raster impl type
typedef struct __gb_gl_raster_impl_t
{
    // the raster
    gb_raster_ref_t         raster;

    // the user raster func
    gb_gl_raster_func_t     func;

    // the user private data
    tb_cpointer_t           priv;

}gb_gl_raster_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t gb_gl_raster_reduce_to_convex(tb_long_t xb, tb_long_t xe, tb_long_t yb, tb_long_t ye, tb_cpointer_t priv)
{
    // check
    gb_gl_raster_impl_t* impl = (gb_gl_raster_impl_t*)priv;
    tb_assert_abort(impl);

    // trace
    tb_trace_d("xb: %ld, xe: %ld, yb: %ld, ye: %ld", xb, yb, xe, ye);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
gb_gl_raster_ref_t gb_gl_raster_init()
{
    // done
    tb_bool_t               ok = tb_false;
    gb_gl_raster_impl_t*    impl = tb_null;
    do
    {
        // make gl raster
        impl = tb_malloc0_type(gb_gl_raster_impl_t);
        tb_assert_and_check_break(impl);

        // init raster
        impl->raster = gb_raster_init();
        tb_assert_and_check_break(impl->raster);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) gb_gl_raster_exit((gb_gl_raster_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (gb_gl_raster_ref_t)impl;
}
tb_void_t gb_gl_raster_exit(gb_gl_raster_ref_t raster)
{
    // check
    gb_gl_raster_impl_t* impl = (gb_gl_raster_impl_t*)raster;
    tb_assert_and_check_return(impl);

    // exit raster
    if (impl->raster) gb_raster_exit(impl->raster);
    impl->raster = tb_null;

    // exit it
    tb_free(impl);
}
tb_void_t gb_gl_raster_done(gb_gl_raster_ref_t raster, gb_polygon_ref_t polygon, gb_rect_ref_t bounds, tb_size_t rule, gb_gl_raster_func_t func, tb_cpointer_t priv)
{
    // check
    gb_gl_raster_impl_t* impl = (gb_gl_raster_impl_t*)raster;
    tb_assert_and_check_return(impl && impl->raster && polygon && func);

    // is convex polygon for each contour?
    if (polygon->convex)
    {
        // done
        tb_size_t       index   = 0;
        gb_point_ref_t  points  = polygon->points;
        tb_uint16_t*    counts  = polygon->counts;
        tb_uint16_t     count   = 0;
        while ((count = *counts++))
        {
            // done it
            func(points + index, count, priv);

            // update the contour index
            index += count;
        }
    }
    else
    {
        // save the user func and private data
        impl->func = func;
        impl->priv = priv;

        // trace
        tb_trace_d("bounds: %{rect}", bounds);

        // done raster and reduce the complex polygon to the some convex polygons
        gb_raster_done(impl->raster, polygon, bounds, rule, gb_gl_raster_reduce_to_convex, (tb_cpointer_t)impl);
    }
}