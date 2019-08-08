#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <core/types.h>
#include <core/maths.h>
#include <core/platform.h>
#include <core/maths.h>
#include <core/shader.h>
#include <core/mesh.h>

#include "include/flex.h"
#include "shaders.h"
#include "png_texture.h"

#define	stripeImageWidth 255

class Sabbia: public Mesh {
    private:
        bool drawColors;
        bool show_normals;
        GLubyte stripeImage[4*stripeImageWidth];
        GLuint texName;
        GLuint texture_sabbia;
        GLuint texture_h;
        void draw_mesh();
        void draw_normals();

    public:
        Mesh *m;
        int grid;
        float h_ondula;
        float h_delta;
        bool dirty;

        Sabbia(int dim, bool draw_colors);

        Vec3 getColour(float h);
        void setH(float y, int x, int z);
        float getH(int x, int z);
        void UpdateCollisions(FlexSolver *g_flex, float radius);
        void initTexture();
        void draw();
        void ondula();
        void ondula(float h);
        void up(float h);
        void draw_plane(float h);
        void toggle_normals();
};

