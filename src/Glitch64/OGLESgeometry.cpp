/*
* Glide64 - Glide video plugin for Nintendo 64 emulators.
* Copyright (c) 2002  Dave2001
* Copyright (c) 2003-2009  Sergey 'Gonetz' Lipski
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include "glide.h"
#include "glitchmain.h"
#include "../Glide64/winlnxdefs.h"
#include "../Glide64/rdp.h"

#define Z_MAX (65536.0f)
#define VERTEX_SIZE sizeof(VERTEX) //Size of vertex struct

#if EMSCRIPTEN
#include "emscripten.h"
#include <iostream>
//#include <GL/glu.h>
#endif

static int xy_off;
static int xy_en;
static int z_en;
static int z_off;
static int q_off;
static int q_en;
static int pargb_off;
static int pargb_en;
static int st0_off;
static int st0_en;
static int st1_off;
static int st1_en;
static int fog_ext_off;
static int fog_ext_en;

int w_buffer_mode;
int inverted_culling;
int culling_mode;

#define VERTEX_BUFFER_SIZE 1500 //Max amount of vertices to buffer, this seems large enough.
static VERTEX vertex_buffer[VERTEX_BUFFER_SIZE];
static int vertex_buffer_count = 0;
static GLenum vertex_draw_mode;
static bool vertex_buffer_enabled = false;

void vbo_init()
{

}

void vbo_draw()
{
  
#if 0 //EMSCRIPTEN
  return;
#endif

  if(vertex_buffer_count)
  {

#if 0
    std::cerr<<"vbo_draw with vertex_buffer_count: "<<vertex_buffer_count<<std::endl;

    // Why are they not creating a vertex buffer in OGLES and uploading vertices to GPU?
#if  1 //EMSCRIPTEN

    GLuint    vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

#if 0 //VERIFY buffer data
    if(vertex_buffer_count == 6)
    {
      for(int i=0;i<6;++i)
      {
        VERTEX* v = &(vertex_buffer[i]);
        std::cerr<<"v"<<i<<": "<<v->x<<","<<v->y<<","<<v->z<<std::endl;
      }
    }
#endif
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*vertex_buffer_count, vertex_buffer, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*vertex_buffer_count, (void*)&vertex_buffer[0], GL_DYNAMIC_DRAW);

#if 1 //EMSCRITPEN

    // get our current program
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM,&currentProgram);
    // glBindAttribLocation(currentProgram, POSITION_ATTR, "aVertex");
    // glBindAttribLocation(currentProgram, COLOUR_ATTR, "aColor");
    // glBindAttribLocation(currentProgram, TEXCOORD_0_ATTR, "aMultiTexCoord0");
    // glBindAttribLocation(currentProgram, TEXCOORD_1_ATTR, "aMultiTexCoord1");
    // glBindAttribLocation(currentProgram, FOG_ATTR, "aFog");
   EM_ASM({
      var prog = GLctx.getIntegerv()
      var loc = GLctx.GetAttribLocation()
   });





    glEnableVertexAttribArray(POSITION_ATTR);
    glVertexAttribPointer(POSITION_ATTR, 4, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].x); //Position
    
    glEnableVertexAttribArray(COLOUR_ATTR);
    glVertexAttribPointer(COLOUR_ATTR, 4, GL_UNSIGNED_BYTE, true, VERTEX_SIZE, &vertex_buffer[0].b); //Colour
    

    glEnableVertexAttribArray(TEXCOORD_0_ATTR);
    glVertexAttribPointer(TEXCOORD_0_ATTR, 2, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].coord[2]); //Tex0
    

    glEnableVertexAttribArray(TEXCOORD_1_ATTR);
    glVertexAttribPointer(TEXCOORD_1_ATTR, 2, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].coord[0]); //Tex1
    

    glEnableVertexAttribArray(FOG_ATTR);
    glVertexAttribPointer(FOG_ATTR, 1, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].f); //Fog

#endif

#if 1 //EMSCRIPTEN
    {
      GLenum errCode;
      const char *errString;
      if ((errCode = glGetError()) != GL_NO_ERROR) 
      {
        std::cerr<<"glGetError ERROR _BEFORE_ calling glDrawArrays: "<<(int)errCode<<std::endl;
      }
    }
#endif

#endif

    //GL_INVALID_OPERATION is generated if a non-zero buffer object name is bound to an enabled array and the buffer object's data store is currently mapped.
    //GL_INVALID_OPERATION is generated if a geometry shader is active and mode is incompatible with the input primitive type of the geometry shader in the currently installed program object.

    // EM_ASM({
    //   GLctx.activeTexture(GLctx.TEXTURE0);
    //   GLctx.bindTexture(GLctx.TEXTURE_2D, null);
    //   GLctx.activeTexture(GLctx.TEXTURE1);
    //   GLctx.bindTexture(GLctx.TEXTURE_2D, null);
    //   GLctx.activeTexture(GLctx.TEXTURE2);
    //   GLctx.bindTexture(GLctx.TEXTURE_2D, null);
    // });

    //glUseProgram(0);


    glDrawArrays(vertex_draw_mode,0,vertex_buffer_count);
    //std::cerr<<"glDrawArrays with count: "<<vertex_buffer_count<<std::endl;

#if 1 //EMSCRIPTEN
    {
      GLenum errCode;
      const char *errString;
      if ((errCode = glGetError()) != GL_NO_ERROR) 
      {
        std::cerr<<"glGetError ERROR _AFTER_ calling glDrawArrays: "<<(int)errCode<<std::endl;
      }
    }
#endif

#else

    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM,&currentProgram);

    EM_ASM({

    var gl = GLctx;
    GLctx.clearColor(0.0, 1.0, 0.0, 1.0);
    // Enable depth testing
    GLctx.enable(GLctx.DEPTH_TEST);
    // Near things obscure far things
    GLctx.depthFunc(GLctx.LEQUAL);
    // Clear the color as well as the depth buffer.
    GLctx.clear(GLctx.COLOR_BUFFER_BIT | GLctx.DEPTH_BUFFER_BIT);

    if(!Module.shaderProgram)
    {

      console.error("******** COMPILING SHADER ************");
      // Creates fragment shader (returns white color for any position)
      var fshader = gl.createShader(gl.FRAGMENT_SHADER);
      gl.shaderSource(fshader, 'void main(void)\n{gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);}');
      gl.compileShader(fshader);
      if (!gl.getShaderParameter(fshader, gl.COMPILE_STATUS))
      {alert("Error during fragment shader compilation:\n" + gl.getShaderInfoLog(fshader)); return;}

      // Creates vertex shader (converts 2D point position to coordinates)
      var vshader = gl.createShader(gl.VERTEX_SHADER);
      gl.shaderSource(vshader, 'attribute vec2 ppos; \nvoid main(void)\n{ gl_Position = vec4(ppos.x, ppos.y, 0.0, 1.0);}');
      gl.compileShader(vshader);
      if (!gl.getShaderParameter(vshader, gl.COMPILE_STATUS))
      {alert('Error during vertex shader compilation:\n' + gl.getShaderInfoLog(vshader)); return;}

      // Creates program and links shaders to it
      var program = gl.createProgram();
      gl.attachShader(program, fshader);
      gl.attachShader(program, vshader);
      gl.linkProgram(program);
      if (!gl.getProgramParameter(program, gl.LINK_STATUS))
       {alert("Error during program linking:" + gl.getProgramInfoLog(program));return;}

      // Validates and uses program in the GL context
      gl.validateProgram(program);
      if (!gl.getProgramParameter(program, gl.VALIDATE_STATUS))
      {alert("Error during program validation:\n" + gl.getProgramInfoLog(program));return;}
      
      Module.shaderProgram = program;

    }

    var program = Module.shaderProgram;

    gl.useProgram(program);

    // Gets address of the input 'attribute' of the vertex shader
    var vattrib = gl.getAttribLocation(program, 'ppos');
    if(vattrib == -1)
    {alert('Error during attribute address retrieval');return;}
    gl.enableVertexAttribArray(vattrib);


    // Initializes the vertex buffer and sets it as current one
    var vbuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbuffer);

    // Puts vertices to buffer and links it to attribute variable 'ppos'
    var vertices = new Float32Array([0.0,0.5,-0.5,-0.5,0.5,-0.5]);
    gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(vattrib, 2, gl.FLOAT, false, 0, 0);

    // Draws the objext
    console.error("******** GL DRAWARRAYS ************");
    gl.drawArrays(gl.TRIANGLES, 0, 3);
    gl.flush()

  });


  //reset the previous program
  glUseProgram(currentProgram);

#endif // if 0
    vertex_buffer_count = 0;

#if 0 //EMSCRIPTEN
    glDeleteBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
  }
}

//Buffer vertices instead of glDrawArrays(...)
void vbo_buffer(GLenum mode,GLint first,GLsizei count,void* pointers)
{
  //std::cerr<<"vbo_buffer invoked."<<std::endl;
  if((count != 3 && mode != GL_TRIANGLES) || vertex_buffer_count + count > VERTEX_BUFFER_SIZE)
  {
    vbo_draw();
  }

  memcpy(&vertex_buffer[vertex_buffer_count],pointers,count * VERTEX_SIZE);
  vertex_buffer_count += count;

  if(count == 3 || mode == GL_TRIANGLES)
  {
    vertex_draw_mode = GL_TRIANGLES;
  }
  else
  {
    vertex_draw_mode = mode;
    vbo_draw(); //Triangle fans and strips can't be joined as easily, just draw them straight away.
  }

  //std::cerr<<"After vbo_buffer vertex_buffer_count: "<<vertex_buffer_count<<std::endl;
}

void vbo_enable()
{
#if 1 //(EMSCRIPTEN)
  return;
#endif

  if(vertex_buffer_enabled)
    return;

  std::cerr<<"vbo_enable"<<std::endl;

  vertex_buffer_enabled = true;
  glEnableVertexAttribArray(POSITION_ATTR);
  glVertexAttribPointer(POSITION_ATTR, 4, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].x); //Position

  glEnableVertexAttribArray(COLOUR_ATTR);
  glVertexAttribPointer(COLOUR_ATTR, 4, GL_UNSIGNED_BYTE, true, VERTEX_SIZE, &vertex_buffer[0].b); //Colour

  glEnableVertexAttribArray(TEXCOORD_0_ATTR);
  glVertexAttribPointer(TEXCOORD_0_ATTR, 2, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].coord[2]); //Tex0

  glEnableVertexAttribArray(TEXCOORD_1_ATTR);
  glVertexAttribPointer(TEXCOORD_1_ATTR, 2, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].coord[0]); //Tex1

  glEnableVertexAttribArray(FOG_ATTR);
  glVertexAttribPointer(FOG_ATTR, 1, GL_FLOAT, false, VERTEX_SIZE, &vertex_buffer[0].f); //Fog
}

void vbo_disable()
{
  vbo_draw();
  vertex_buffer_enabled = false;
}


inline float ZCALC(const float & z, const float & q) {
  float res = z_en ? ((z) / Z_MAX) / (q) : 1.0f;
  return res;
}

/*
#define zclamp (1.0f-1.0f/zscale)
static inline void zclamp_glVertex4f(float a, float b, float c, float d)
{
  if (c<zclamp) c = zclamp;
  glVertex4f(a,b,c,d);
}
#define glVertex4f(a,b,c,d) zclamp_glVertex4f(a,b,c,d)
*/


static inline float ytex(int tmu, float y) {
  if (invtex[tmu])
    return invtex[tmu] - y;
  else
    return y;
}

void init_geometry()
{
  xy_en = q_en = pargb_en = st0_en = st1_en = z_en = 0;
  w_buffer_mode = 0;
  inverted_culling = 0;

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  vbo_init();
}

FX_ENTRY void FX_CALL
grCoordinateSpace( GrCoordinateSpaceMode_t mode )
{
  LOG("grCoordinateSpace(%d)\r\n", mode);
  switch(mode)
  {
  case GR_WINDOW_COORDS:
    break;
  default:
    display_warning("unknwown coordinate space : %x", mode);
  }
}

FX_ENTRY void FX_CALL
grVertexLayout(FxU32 param, FxI32 offset, FxU32 mode)
{
  LOG("grVertexLayout(%d,%d,%d)\r\n", param, offset, mode);
  switch(param)
  {
  case GR_PARAM_XY:
    xy_en = mode;
    xy_off = offset;
    break;
  case GR_PARAM_Z:
    z_en = mode;
    z_off = offset;
    break;
  case GR_PARAM_Q:
    q_en = mode;
    q_off = offset;
    break;
  case GR_PARAM_FOG_EXT:
    fog_ext_en = mode;
    fog_ext_off = offset;
    break;
  case GR_PARAM_PARGB:
    pargb_en = mode;
    pargb_off = offset;
    break;
  case GR_PARAM_ST0:
    st0_en = mode;
    st0_off = offset;
    break;
  case GR_PARAM_ST1:
    st1_en = mode;
    st1_off = offset;
    break;
  default:
    display_warning("unknown grVertexLayout parameter : %x", param);
  }
}

FX_ENTRY void FX_CALL
grCullMode( GrCullMode_t mode )
{
  LOG("grCullMode(%d)\r\n", mode);
  static int oldmode = -1, oldinv = -1;
  culling_mode = mode;
  if (inverted_culling == oldinv && oldmode == mode)
    return;
  oldmode = mode;
  oldinv = inverted_culling;
  switch(mode)
  {
  case GR_CULL_DISABLE:
    glDisable(GL_CULL_FACE);
    break;
  case GR_CULL_NEGATIVE:
    if (!inverted_culling)
      glCullFace(GL_FRONT);
    else
      glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    break;
  case GR_CULL_POSITIVE:
    if (!inverted_culling)
      glCullFace(GL_BACK);
    else
      glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    break;
  default:
    display_warning("unknown cull mode : %x", mode);
  }
}

// Depth buffer

FX_ENTRY void FX_CALL
grDepthBufferMode( GrDepthBufferMode_t mode )
{
  LOG("grDepthBufferMode(%d)\r\n", mode);
  switch(mode)
  {
  case GR_DEPTHBUFFER_DISABLE:
    glDisable(GL_DEPTH_TEST);
    w_buffer_mode = 0;
    return;
  case GR_DEPTHBUFFER_WBUFFER:
  case GR_DEPTHBUFFER_WBUFFER_COMPARE_TO_BIAS:
    glEnable(GL_DEPTH_TEST);
    w_buffer_mode = 1;
    break;
  case GR_DEPTHBUFFER_ZBUFFER:
  case GR_DEPTHBUFFER_ZBUFFER_COMPARE_TO_BIAS:
    glEnable(GL_DEPTH_TEST);
    w_buffer_mode = 0;
    break;
  default:
    display_warning("unknown depth buffer mode : %x", mode);
  }
}

FX_ENTRY void FX_CALL
grDepthBufferFunction( GrCmpFnc_t function )
{
  LOG("grDepthBufferFunction(%d)\r\n", function);
  switch(function)
  {
  case GR_CMP_GEQUAL:
    if (w_buffer_mode)
      glDepthFunc(GL_LEQUAL);
    else
      glDepthFunc(GL_GEQUAL);
    break;
  case GR_CMP_LEQUAL:
    if (w_buffer_mode)
      glDepthFunc(GL_GEQUAL);
    else
      glDepthFunc(GL_LEQUAL);
    break;
  case GR_CMP_LESS:
    if (w_buffer_mode)
      glDepthFunc(GL_GREATER);
    else
      glDepthFunc(GL_LESS);
    break;
  case GR_CMP_ALWAYS:
    glDepthFunc(GL_ALWAYS);
    break;
  case GR_CMP_EQUAL:
    glDepthFunc(GL_EQUAL);
    break;
  case GR_CMP_GREATER:
    if (w_buffer_mode)
      glDepthFunc(GL_LESS);
    else
      glDepthFunc(GL_GREATER);
    break;
  case GR_CMP_NEVER:
    glDepthFunc(GL_NEVER);
    break;
  case GR_CMP_NOTEQUAL:
    glDepthFunc(GL_NOTEQUAL);
    break;

  default:
    display_warning("unknown depth buffer function : %x", function);
  }
}

FX_ENTRY void FX_CALL
grDepthMask( FxBool mask )
{
  LOG("grDepthMask(%d)\r\n", mask);
  glDepthMask(mask);
}
float biasFactor = 0;
#if 0
void FindBestDepthBias()
{
  float f, bestz = 0.25f;
  int x;
  if (biasFactor) return;
  biasFactor = 64.0f; // default value
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glColor4ub(255,255,255,255);
  glDepthMask(GL_TRUE);
  for (x=0, f=1.0f; f<=65536.0f; x+=4, f*=2.0f) {
    float z;
    glPolygonOffset(0, f);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(float(x+4 - widtho)/(width/2), float(0 - heighto)/(height/2), 0.5);
    glVertex3f(float(x - widtho)/(width/2), float(0 - heighto)/(height/2), 0.5);
    glVertex3f(float(x+4 - widtho)/(width/2), float(4 - heighto)/(height/2), 0.5);
    glVertex3f(float(x - widtho)/(width/2), float(4 - heighto)/(height/2), 0.5);
    glEnd();
    glReadPixels(x+2, 2+viewport_offset, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    z -= 0.75f + 8e-6f;
    if (z<0.0f) z = -z;
    if (z > 0.01f) continue;
    if (z < bestz) {
      bestz = z;
      biasFactor = f;
    }
    //printf("f %g z %g\n", f, z);
  }
  //printf(" --> bias factor %g\n", biasFactor);
  glPopAttrib();
}
#endif

FX_ENTRY void FX_CALL
grDepthBiasLevel( FxI32 level )
{
  LOG("grDepthBiasLevel(%d)\r\n", level);
  if (level)
  {
    if(settings.force_polygon_offset)
    {
      glPolygonOffset(settings.polygon_offset_factor, settings.polygon_offset_units);
    }
    else
    {
      if(w_buffer_mode)
        glPolygonOffset(1.0f, -(float)level*zscale/255.0f);
      else
        glPolygonOffset(0, (float)level*biasFactor);
    }
    glEnable(GL_POLYGON_OFFSET_FILL);
  }
  else
  {
    glPolygonOffset(0,0);
    glDisable(GL_POLYGON_OFFSET_FILL);
  }
}

// draw

FX_ENTRY void FX_CALL
grDrawTriangle( const void *a, const void *b, const void *c )
{
#if 0 //Verify the geometry
  VERTEX* av = (VERTEX*)a;
  VERTEX* bv = (VERTEX*)b;
  VERTEX* cv = (VERTEX*)c;
  std::cerr<<"triangle a: "<<av->x<<","<<av->y<<","<<av->z<<std::endl;
  std::cerr<<"triangle b: "<<bv->x<<","<<bv->y<<","<<bv->z<<std::endl;
  std::cerr<<"triangle c: "<<cv->x<<","<<cv->y<<","<<cv->z<<std::endl;
  //std::cerr<<"grDrawTriangle "<<std::endl;
#endif
  LOG("grDrawTriangle()\r\n\t");

  if(nvidia_viewport_hack && !render_to_texture)
  {
    glViewport(0, viewport_offset, viewport_width, viewport_height);
    nvidia_viewport_hack = 0;
  }

  reloadTexture();

  if(need_to_compile) compile_shader();

  if(vertex_buffer_count + 3 > VERTEX_BUFFER_SIZE)
  {
    vbo_draw();
  }

#if 1 //EMSCRIPTEN
  vbo_enable();
#endif

  vertex_draw_mode = GL_TRIANGLES;
  memcpy(&vertex_buffer[vertex_buffer_count],a,VERTEX_SIZE);
  memcpy(&vertex_buffer[vertex_buffer_count+1],b,VERTEX_SIZE);
  memcpy(&vertex_buffer[vertex_buffer_count+2],c,VERTEX_SIZE);
  vertex_buffer_count += 3;

#if 0 // EMSCRIPTEN
  vbo_draw();
#endif
}

FX_ENTRY void FX_CALL
grDrawPoint( const void *pt )
{
/*
  float *x = (float*)pt + xy_off/sizeof(float);
  float *y = (float*)pt + xy_off/sizeof(float) + 1;
  float *z = (float*)pt + z_off/sizeof(float);
  float *q = (float*)pt + q_off/sizeof(float);
  unsigned char *pargb = (unsigned char*)pt + pargb_off;
  float *s0 = (float*)pt + st0_off/sizeof(float);
  float *t0 = (float*)pt + st0_off/sizeof(float) + 1;
  float *s1 = (float*)pt + st1_off/sizeof(float);
  float *t1 = (float*)pt + st1_off/sizeof(float) + 1;
  float *fog = (float*)pt + fog_ext_off/sizeof(float);
  LOG("grDrawPoint()\r\n");

  if(nvidia_viewport_hack && !render_to_texture)
  {
    glViewport(0, viewport_offset, viewport_width, viewport_height);
    nvidia_viewport_hack = 0;
  }

  reloadTexture();

  if(need_to_compile) compile_shader();

  glBegin(GL_POINTS);

  if (nbTextureUnits > 2)
  {
    if (st0_en)
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *s0 / *q / (float)tex1_width,
      ytex(0, *t0 / *q / (float)tex1_height));
    if (st1_en)
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB, *s1 / *q / (float)tex0_width,
      ytex(1, *t1 / *q / (float)tex0_height));
  }
  else
  {
    if (st0_en)
      glTexCoord2f(*s0 / *q / (float)tex0_width,
      ytex(0, *t0 / *q / (float)tex0_height));
  }
  if (pargb_en)
    glColor4f(pargb[2]/255.0f, pargb[1]/255.0f, pargb[0]/255.0f, pargb[3]/255.0f);
  if (fog_enabled && fog_coord_support)
  {
    if(!fog_ext_en || fog_enabled != 2)
      glSecondaryColor3f((1.0f / *q) / 255.0f, 0.0f, 0.0f);
    else
      glSecondaryColor3f((1.0f / *fog) / 255.0f, 0.0f, 0.0f);
  }
  glVertex4f((*x - (float)widtho) / (float)(width/2) / *q,
    -(*y - (float)heighto) / (float)(height/2) / *q, ZCALC(*z ,*q), 1.0f / *q);

  glEnd();
*/
}

FX_ENTRY void FX_CALL
grDrawLine( const void *a, const void *b )
{
/*
  float *a_x = (float*)a + xy_off/sizeof(float);
  float *a_y = (float*)a + xy_off/sizeof(float) + 1;
  float *a_z = (float*)a + z_off/sizeof(float);
  float *a_q = (float*)a + q_off/sizeof(float);
  unsigned char *a_pargb = (unsigned char*)a + pargb_off;
  float *a_s0 = (float*)a + st0_off/sizeof(float);
  float *a_t0 = (float*)a + st0_off/sizeof(float) + 1;
  float *a_s1 = (float*)a + st1_off/sizeof(float);
  float *a_t1 = (float*)a + st1_off/sizeof(float) + 1;
  float *a_fog = (float*)a + fog_ext_off/sizeof(float);

  float *b_x = (float*)b + xy_off/sizeof(float);
  float *b_y = (float*)b + xy_off/sizeof(float) + 1;
  float *b_z = (float*)b + z_off/sizeof(float);
  float *b_q = (float*)b + q_off/sizeof(float);
  unsigned char *b_pargb = (unsigned char*)b + pargb_off;
  float *b_s0 = (float*)b + st0_off/sizeof(float);
  float *b_t0 = (float*)b + st0_off/sizeof(float) + 1;
  float *b_s1 = (float*)b + st1_off/sizeof(float);
  float *b_t1 = (float*)b + st1_off/sizeof(float) + 1;
  float *b_fog = (float*)b + fog_ext_off/sizeof(float);
  LOG("grDrawLine()\r\n");

  if(nvidia_viewport_hack && !render_to_texture)
  {
    glViewport(0, viewport_offset, viewport_width, viewport_height);
    nvidia_viewport_hack = 0;
  }

  reloadTexture();

  if(need_to_compile) compile_shader();

  glBegin(GL_LINES);

  if (nbTextureUnits > 2)
  {
    if (st0_en)
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *a_s0 / *a_q / (float)tex1_width, ytex(0, *a_t0 / *a_q / (float)tex1_height));
    if (st1_en)
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB, *a_s1 / *a_q / (float)tex0_width, ytex(1, *a_t1 / *a_q / (float)tex0_height));
  }
  else
  {
    if (st0_en)
      glTexCoord2f(*a_s0 / *a_q / (float)tex0_width, ytex(0, *a_t0 / *a_q / (float)tex0_height));
  }
  if (pargb_en)
    glColor4f(a_pargb[2]/255.0f, a_pargb[1]/255.0f, a_pargb[0]/255.0f, a_pargb[3]/255.0f);
  if (fog_enabled && fog_coord_support)
  {
    if(!fog_ext_en || fog_enabled != 2)
      glSecondaryColor3f((1.0f / *a_q) / 255.0f, 0.0f, 0.0f);
    else
      glSecondaryColor3f((1.0f / *a_fog) / 255.0f, 0.0f, 0.0f);
  }
  glVertex4f((*a_x - (float)widtho) / (float)(width/2) / *a_q,
    -(*a_y - (float)heighto) / (float)(height/2) / *a_q, ZCALC(*a_z, *a_q), 1.0f / *a_q);

  if (nbTextureUnits > 2)
  {
    if (st0_en)
      glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *b_s0 / *b_q / (float)tex1_width,
      ytex(0, *b_t0 / *b_q / (float)tex1_height));
    if (st1_en)
      glMultiTexCoord2fARB(GL_TEXTURE0_ARB, *b_s1 / *b_q / (float)tex0_width,
      ytex(1, *b_t1 / *b_q / (float)tex0_height));
  }
  else
  {
    if (st0_en)
      glTexCoord2f(*b_s0 / *b_q / (float)tex0_width,
      ytex(0, *b_t0 / *b_q / (float)tex0_height));
  }
  if (pargb_en)
    glColor4f(b_pargb[2]/255.0f, b_pargb[1]/255.0f, b_pargb[0]/255.0f, b_pargb[3]/255.0f);
  if (fog_enabled && fog_coord_support)
  {
    if(!fog_ext_en || fog_enabled != 2)
      glSecondaryColor3f((1.0f / *b_q) / 255.0f, 0.0f, 0.0f);
    else
      glSecondaryColor3f((1.0f / *b_fog) / 255.0f, 0.0f, 0.0f);
  }
  glVertex4f((*b_x - (float)widtho) / (float)(width/2) / *b_q,
    -(*b_y - (float)heighto) / (float)(height/2) / *b_q, ZCALC(*b_z, *b_q), 1.0f / *b_q);

  glEnd();
*/
}

FX_ENTRY void FX_CALL
grDrawVertexArray(FxU32 mode, FxU32 Count, void *pointers2)
{
  //std::cerr<<"grDrawVertexArray: mode: "<<mode<<" Count: "<<Count<<std::endl;
  void **pointers = (void**)pointers2;
  LOG("grDrawVertexArray(%d,%d)\r\n", mode, Count);

#if 1 //(!EMSCRIPTEN)
  if(nvidia_viewport_hack && !render_to_texture)
#endif
  {
    glViewport(0, viewport_offset, viewport_width, viewport_height);
    nvidia_viewport_hack = 0;
  }

  reloadTexture();

  if(need_to_compile) compile_shader();

  if(mode != GR_TRIANGLE_FAN)
  {
    display_warning("grDrawVertexArray : unknown mode : %x", mode);
  }

#if 0 //EMSCRIPTEN

    GLuint    vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*vertex_buffer_count, (void*)&vertex_buffer[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*Count, pointers[0], GL_STATIC_DRAW);

#endif

  vbo_enable();

#if 1 //(!EMSCRIPTEN)
  vbo_buffer(GL_TRIANGLE_FAN,0,Count,pointers[0]);
#endif

#if 0 //EMSCRIPTEN

  {
      GLenum errCode;
      const char *errString;
      if ((errCode = glGetError()) != GL_NO_ERROR) 
      {
        std::cerr<<"glGetError ERROR _BEFORE_ calling glDrawArrays: "<<(int)errCode<<std::endl;
      }
    }

  glDrawArrays(mode,0,Count);

    {
      GLenum errCode;
      const char *errString;
      if ((errCode = glGetError()) != GL_NO_ERROR) 
      {
        std::cerr<<"glGetError ERROR _AFTER_ calling glDrawArrays: "<<(int)errCode<<std::endl;
      }
    }

  glDeleteBuffers(1, &vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

FX_ENTRY void FX_CALL
grDrawVertexArrayContiguous(FxU32 mode, FxU32 Count, void *pointers, FxU32 stride)
{

  //std::cerr<<"grDrawVertexArrayContiguous mode: "<<mode<<" Count: "<<Count<<" stride: "<<stride<<std::endl;
  LOG("grDrawVertexArrayContiguous(%d,%d,%d)\r\n", mode, Count, stride);

  if(nvidia_viewport_hack && !render_to_texture)
  {
    glViewport(0, viewport_offset, viewport_width, viewport_height);
    nvidia_viewport_hack = 0;
  }

  if(stride != 156)
  {
	  LOGINFO("Incompatible stride\n");
  }

  reloadTexture();

  if(need_to_compile) compile_shader();

  vbo_enable();

  switch(mode)
  {
  case GR_TRIANGLE_STRIP:
    vbo_buffer(GL_TRIANGLE_STRIP,0,Count,pointers);
    break;
  case GR_TRIANGLE_FAN:
    vbo_buffer(GL_TRIANGLE_FAN,0,Count,pointers);
    break;
  default:
    display_warning("grDrawVertexArrayContiguous : unknown mode : %x", mode);
  }
}
