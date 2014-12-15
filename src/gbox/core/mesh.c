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
 * @file        mesh.c
 * @ingroup     core
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mesh.h"
#include "impl/mesh_edge.h"
#include "impl/mesh_face.h"
#include "impl/mesh_vertex.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mesh impl type 
typedef struct __gb_mesh_impl_t
{
    // the edges
    gb_mesh_edge_list_ref_t         edges;

    // the faces
    gb_mesh_face_list_ref_t         faces;

    // the vertices
    gb_mesh_vertex_list_ref_t       vertices;

}gb_mesh_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
gb_mesh_ref_t gb_mesh_init(tb_item_func_t edge_func, tb_item_func_t face_func, tb_item_func_t vertex_func)
{
    // done
    tb_bool_t           ok = tb_false;
    gb_mesh_impl_t*     impl = tb_null;
    do
    {
        // make mesh
        impl = tb_malloc0_type(gb_mesh_impl_t);
        tb_assert_and_check_break(impl);
    
        // init edges
        impl->edges = gb_mesh_edge_list_init(edge_func);
        tb_assert_and_check_break(impl->edges);
  
        // init faces
        impl->faces = gb_mesh_face_list_init(face_func);
        tb_assert_and_check_break(impl->faces);

        // init vertices
        impl->vertices = gb_mesh_vertex_list_init(vertex_func);
        tb_assert_and_check_break(impl->vertices);

        // clear the mesh first and create a single vertex-edge mesh
        if (!gb_mesh_clear((gb_mesh_ref_t)impl)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) gb_mesh_exit((gb_mesh_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (gb_mesh_ref_t)impl;
}
tb_void_t gb_mesh_exit(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl);
   
    // clear it first
    gb_mesh_clear(mesh);

    // exit edges
    if (impl->edges) gb_mesh_edge_list_exit(impl->edges);
    impl->edges = tb_null;

    // exit faces
    if (impl->faces) gb_mesh_face_list_exit(impl->faces);
    impl->faces = tb_null;

    // exit vertices
    if (impl->vertices) gb_mesh_vertex_list_exit(impl->vertices);
    impl->vertices = tb_null;

    // exit it
    tb_free(impl);
}
tb_bool_t gb_mesh_clear(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl, tb_false);

    // done
    tb_bool_t               ok = tb_false;
    gb_mesh_edge_ref_t      edge = tb_null;
    gb_mesh_face_ref_t      lface = tb_null;
    gb_mesh_face_ref_t      rface = tb_null;
    gb_mesh_vertex_ref_t    vertex = tb_null;
    do
    {
        // clear edges
        if (impl->edges) gb_mesh_edge_list_clear(impl->edges);

        // clear faces
        if (impl->faces) gb_mesh_face_list_clear(impl->faces);

        // clear vertices
        if (impl->vertices) gb_mesh_vertex_list_clear(impl->vertices);

        // make the vertex
        vertex = gb_mesh_vertex_list_make(impl->vertices);
        tb_assert_and_check_break(vertex);

        // make the left face
        lface = gb_mesh_face_list_make(impl->faces);
        tb_assert_and_check_break(lface);

        // make the right face
        rface = gb_mesh_face_list_make(impl->faces);
        tb_assert_and_check_break(rface);

        // make the edge
        edge = gb_mesh_edge_list_make(impl->edges);
        tb_assert_and_check_break(edge);

        /* create a looping edge that connects to itself at a single vertex
         *
         *          -------
         *         |       |
         *         | rface |
         *         |       |
         *         O/D <---
         *
         *           lface
         */
        gb_mesh_edge_org_set(edge, vertex);
        gb_mesh_edge_dst_set(edge, vertex);
        gb_mesh_edge_lface_set(edge, lface);
        gb_mesh_edge_rface_set(edge, rface);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // kill the vertex
        if (vertex) gb_mesh_vertex_list_kill(impl->vertices, vertex);
        vertex = tb_null;

        // kill the left face
        if (lface) gb_mesh_face_list_kill(impl->faces, lface);
        lface = tb_null;

        // kill the right face
        if (rface) gb_mesh_face_list_kill(impl->faces, rface);
        rface = tb_null;

        // kill the edge
        if (edge) gb_mesh_edge_list_kill(impl->edges, edge);
        edge = tb_null;
    }

    // ok?
    return ok;
}

