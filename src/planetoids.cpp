#include "planetoids.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "soil/src/SOIL.h"
#include <time.h>
#include <cstdlib>
#include <iostream>

Planetoids::Planetoids()
{
    nb = 0;
    program = 0;
}


Planetoids::Planetoids(int num, const char* model) : Body()
{
    m = LoadModelPlus((char*)model);
    nb = num;
    for(int i = 0; i < num; i++){
        reflectivity[i] = 1;
        scale[i] = 1;
        matrix[i] = IdentityMatrix();
        scale_mat[i] = S(scale[i],scale[i],scale[i]);
        rot_mat[i] = IdentityMatrix();
        trans_mat[i] = IdentityMatrix();

        position[i] = vec3(0,0,0);
        velocity[i] = 0;
    }
}

Planetoids::Planetoids(int num, const char *model, const char *tex) : Body()
{
    m = LoadModelPlus((char*)model);

    texture = SOIL_load_OGL_texture(
            tex,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y
            );

    // Typical Texture Generation Using Data From The Bitmap
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    //LoadTGATextureSimple(tex, &texture);
    for(int i = 0; i < num; i++){
        reflectivity[i] = 1;
        scale[i]  = 1;
        matrix[i]  = IdentityMatrix();
        scale_mat[i]  = S(scale[i], scale[i], scale[i]);
        rot_mat[i]  = IdentityMatrix();
        trans_mat[i]  = IdentityMatrix();

        position[i] = vec3(0,0,0);
        velocity[i] = 0;

    }
}

Planetoids::Planetoids(int num, float dist, const char *model, const char *tex, const char* vertex_shader, const char* fragment_shader) : Body()
{
    nb = num;
    reflectivity = new float[num];
    scale = new float[num];

    matrix = new mat4[num];
    scale_mat = new mat4[num];
    rot_mat = new mat4[num];
    trans_mat = new mat4[num];

    position = new vec3[num];
    velocity = new float[num];
    rot_axis = new vec3[num];
    
    // Frö för slumpvärden
    srand(time(NULL));

    m = LoadModelPlus((char*)model);
	program = loadShaders(vertex_shader, fragment_shader);

    texture = SOIL_load_OGL_texture(
            tex,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y
            );

    // Typical Texture Generation Using Data From The Bitmap
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    //LoadTGATextureSimple(tex, &texture);

    vec3 tmp;
    tmp = vec3(((float) rand()/RAND_MAX * 2 -1)/2,((float) rand()/RAND_MAX * 2 -1)/2,((float) rand()/RAND_MAX * 2 -1)/2);
    for(int i = 0; i < num; i++){
        reflectivity[i] = 1;
        scale[i]  = (float) rand()/RAND_MAX * 0.8;
        matrix[i]  = IdentityMatrix();
        scale_mat[i]  = S(scale[i], scale[i], scale[i]);
        rot_mat[i]  = IdentityMatrix();
        trans_mat[i]  = IdentityMatrix();

        rot_mat[i] = ArbRotate(tmp, (float) rand()/RAND_MAX * 2*M_PI);
        position[i] = rot_mat[i]*(vec3(dist,dist,dist)+dist*vec3(((float) rand()/RAND_MAX * 2 -1)/5,((float) rand()/RAND_MAX * 2 -1)/2,((float) rand()/RAND_MAX * 2 -1)/2));
        velocity[i] = (float) rand()/RAND_MAX * 5;

        rot_axis[i] = tmp;

        place(position[i], i);

    }
}

void Planetoids::matrix_update(int index)
{
    matrix[index] = trans_mat[index]*rot_mat[index]*scale_mat[index];
}

void Planetoids::rotate(vec3 direction, float angle, int index)
{
    rot_mat[index] = ArbRotate(direction, angle) * rot_mat[index];
    matrix_update(index);
}

void Planetoids::place(vec3 pos, int index)
{
    trans_mat[index] = T(pos.x, pos.y, pos.z);
    matrix_update(index);
}

void Planetoids::translate(vec3 d, int index)
{
    trans_mat[index] = T(d.x, d.y, d.z)* trans_mat[index];
    matrix_update(index);
}

void Planetoids::update(float dt)
{
    vec3 v,ds;
    for(int i = 0; i < nb; i++){
        v = velocity[i]*Normalize(CrossProduct(rot_axis[i], position[i]));
        position[i] = position[i] + dt*v;
        place(position[i], i);
    }
}

void Planetoids::draw()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    for(int i = 0; i < nb; i++){
        glUniformMatrix4fv(glGetUniformLocation(program, "mdl_matrix"), 1, GL_TRUE, matrix[i].m);
        DrawModel(m, program, "in_position", "in_normal", "in_tex_coord");
    }        
}
