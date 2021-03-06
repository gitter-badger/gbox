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
 * @file        color.c
 * @ingroup     core
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
gb_gl_program_ref_t gb_gl_program_init_color()
{
    // the vertex shader
    static tb_char_t const* vshader = 
#if defined(TB_CONFIG_OS_IOS) || defined(TB_CONFIG_OS_ANDROID)
        "precision mediump float;                                               \n"
#endif
        "                                                                       \n"
        "attribute vec4 aColor;                                                 \n"
        "attribute vec4 aVertices;                                              \n"
        "                                                                       \n"
        "varying vec4 vColors;                                                  \n"
        "uniform mat4 uMatrixModel;                                             \n"
        "uniform mat4 uMatrixProject;                                           \n"
        "                                                                       \n"
        "void main()                                                            \n"
        "{                                                                      \n"
        "   vColors = aColor;                                                   \n"  
        "   gl_Position = uMatrixProject * uMatrixModel * aVertices;            \n"
        "}                                                                      \n";
    
    // the fragment shader
    static tb_char_t const* fshader = 
#if defined(TB_CONFIG_OS_IOS) || defined(TB_CONFIG_OS_ANDROID)
        "precision mediump float;                                               \n"
#endif
        "                                                                       \n"
        "varying vec4 vColors;                                                  \n"
        "                                                                       \n"
        "void main()                                                            \n"
        "{                                                                      \n"
        "   gl_FragColor = vColors;                                             \n"
        "}                                                                      \n";


    // init program
    gb_gl_program_ref_t program = gb_gl_program_init(GB_GL_PROGRAM_TYPE_COLOR, vshader, fshader);
    tb_assert_and_check_return_val(program, tb_null);

    // init location
    gb_gl_program_location_set(program, GB_GL_PROGRAM_LOCATION_COLORS,          gb_gl_program_attr(program, "aColor"));  //!< FIXME some name can not work for mac, .e.g aColors aColorx ...
    gb_gl_program_location_set(program, GB_GL_PROGRAM_LOCATION_VERTICES,        gb_gl_program_attr(program, "aVertices"));
    gb_gl_program_location_set(program, GB_GL_PROGRAM_LOCATION_MATRIX_MODEL,    gb_gl_program_unif(program, "uMatrixModel"));
    gb_gl_program_location_set(program, GB_GL_PROGRAM_LOCATION_MATRIX_PROJECT,  gb_gl_program_unif(program, "uMatrixProject"));

    // ok
    return program;
}

