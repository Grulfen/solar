#ifndef OBJECT_H
#define OBJECT_H

#include <GL/gl.h>
#include "VectorUtils3.h"
#include "loadobj.h"

class Object {
    protected:
        mat4 rot_mat;
        mat4 trans_mat;
        mat4 scale_mat;

        // För att skala objekt
        float scale;
    public:
        Model *m;

        mat4 matrix;
        vec3 position;

        // För att sätta skalan
        void set_scale(float);
        // För att returnera skala
        float get_scale(); 

        float reflectivity;
        GLuint texture;

        void rotate(char direction, float angle);
        void translate(float dx, float dy, float dz);
        void place(vec3 pos);
        void update();
        void draw(int);

        Object();
        Object(const char*);
        Object(Model*, const char*);
        Object(const char*, const char*);
};

#endif
