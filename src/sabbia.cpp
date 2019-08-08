#include "sabbia.h"


using namespace std;

// output contents of array to screen
void printArray(int arr[], int size) {
    for ( int i = 0; i < size; i++ ) {
        cout << arr[i] << ' ';
    }
    cout << endl;
}


Sabbia::Sabbia(int dim, bool draw_colors=false) : Mesh() {
    //float y = 1.0f;
    grid = dim;
    h_ondula = 0.0f;
    h_delta = 0.001;
    dirty=true;
    drawColors = draw_colors;	glShadeModel(GL_SMOOTH);
    show_normals = false;

    int indices_len = 6 * (grid - 1) * (grid - 1);
    //int indices_len = 3 * (grid*(grid - 1)-1); // con la triangle degeneration
    uint32_t indices[indices_len];
    //uint32_t indices[] = { 0, 1, 3, 1, 4, 3, 1, 2, 4, 2, 5, 4, 3, 4, 6, 4, 7, 6, 4, 5, 7, 5, 8, 7 }; // 24

    Point3 positions[grid * grid];
    Vector3 normals[grid * grid];

    int i = 0;
    float x, z;
    for (x = 0; x < grid; x++) {
        for (z = 0; z < grid; z++) {
            positions[i] = Point3(x, 0.0, z);
            normals[i] = Vector3(0.0f, 1.0f, 0.0f);
            i++;
        }
    }

    /* // con la triangle degeneration
    int j = 0;
    for (z = 0; z < grid - 1; z++) {
        for (x = 0; x < grid-1; x=x+2) {
            i = x + z * grid;  // i = 0
            indices[j++] = i;
            indices[j++] = i + grid;
            indices[j++] = i + 1;
            indices[j++] = i + grid + 1;
            indices[j++] = i + 2;
            indices[j++] = i + grid + 2;
        }

        if (z < (grid-1)) { // degenerate triangle
        	indices[j++] = i + grid + 2;
        	indices[j++] = i + grid + 2;
        	indices[j++] = i + grid;
        }
    }*/


    int j = 0;
    for (z = 0; z < grid - 1; z++) {
        for (x = 0; x < grid - 1; x++) {
            i = x + z * grid;
            indices[j] = i;
            indices[j + 1] = i + 1;
            indices[j + 2] = i + grid;
            indices[j + 3] = i + grid;
            indices[j + 4] = i + 1;
            indices[j + 5] = i + grid + 1;
            j += 6;
        }
    }

    //printArray(indices, indices_len);

    m = new Mesh();
    m->m_indices.insert(m->m_indices.begin(), indices,
            indices + indices_len);
    m->m_positions.insert(m->m_positions.begin(), positions,
            positions + grid * grid);
    m->m_normals.insert(m->m_normals.begin(), normals,
            normals + grid * grid);
    m->m_colours.resize(grid * grid, Colour(1.0f, 1.0f, 1.0f, 1.0f));

    m->Normalize(10.0f);
    //m->Transform(TranslationMatrix(Point3(0.0f, 0.0f, 0.0f)));

    //m->m_positions[4].y += 1.0f; //
    m->CalculateNormals();

    //initTexture();
    int w,h;
    texture_sabbia = png_texture_load("data/rock.png", &w, &h);
    texture_h = png_texture_load("data/texture1d.png", &w, &h, true);
}


Vec3 Sabbia::getColour(float h) {
    //if (h > 0.20 && h < 0.205)
    //  return Vec3(0.0, 0.0, 0.0);

    if (h < 0.25) {
        return Vec3(0.0, 1.0, 1.0 - h * 4);
    } else if (h < 0.5) {
        return Vec3((h - 0.25) * 2, 1.0, 0.0);
    } else if (h < 0.75) {
        return Vec3(2 * h - 0.5, 1.0, 0.0);
    } else
        return Vec3(1.0, 1.0 - (h - 0.75) * 4, 0.0);
}

void Sabbia::setH(float y, int x, int z) {
    int i = z + x * grid;
    m->m_positions[i].y = y;

    if (drawColors) {
        Vec3 colour = getColour(y);

        m->m_colours[i].r = colour.x;
        m->m_colours[i].g = colour.y;
        m->m_colours[i].b = colour.z;
    }
}

float Sabbia::getH(int x, int z) {
    return m->m_positions[z + x * grid].y;
}

void Sabbia::UpdateCollisions(FlexSolver *g_flex, float radius) {
    if (dirty) {
        flexSetTriangles(g_flex, (int*) &m->m_indices[0],
                (float*) &m->m_positions[0], m->GetNumFaces(),
                m->GetNumVertices(), radius * 2,
                eFlexMemoryHost); //eFlexMemoryDevice);
        dirty = false;
    }
}

void Sabbia::initTexture() {
    for (int j = 1; j < stripeImageWidth; j++) {
        if (j % 30 == 0) {
            stripeImage[4 * j] = (GLubyte) 0;
            stripeImage[4 * j + 1] = (GLubyte) 0;
            stripeImage[4 * j + 2] = (GLubyte) 0;
            stripeImage[4 * j + 3] = (GLubyte) 255;

        }
        else {
            stripeImage[4 * j] = (GLubyte) 255;
            stripeImage[4 * j + 1] = (GLubyte) 255-j;
            stripeImage[4 * j + 2] = (GLubyte) 255;
            stripeImage[4 * j + 3] = (GLubyte) 255;
        }
    }

    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_1D, texName);
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, stripeImage);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Sabbia::draw() {
    glUseProgram(0); // fondamentale, altrimenti la texture non viene visualizzata

    // prima texture
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);


    glBindTexture(GL_TEXTURE_2D, texture_sabbia);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


    GLfloat plane1[4] = { 0.5, 0.0, 0.0, 0.0 };
    GLfloat plane2[4] = { 0.0, 0.0, 0.5, 0.0 };
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane1);

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, plane2);

    // seconda texture, quella delle isoipse
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture_h);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    GLfloat zplane[4] = { 0.0, 0.5, 0.0, 0.0 };
    glTexGenfv(GL_S, GL_OBJECT_PLANE, zplane);

    // disegnamo l'oggetto
    draw_mesh();
    //glutSolidCube(2.0);

    // disabilitiamo la seconda texture
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_1D);

    if (show_normals)
    	draw_normals();

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);

}

void Sabbia::draw_mesh() {

	glVerify(glBindBuffer(GL_ARRAY_BUFFER, 0));
	glVerify(glEnableClientState(GL_NORMAL_ARRAY));
	glVerify(glEnableClientState(GL_VERTEX_ARRAY));

	glVerify(glNormalPointer(GL_FLOAT, sizeof(float)*3, &m->m_normals[0]));
	glVerify(glVertexPointer(3, GL_FLOAT, sizeof(float)*3, &m->m_positions[0]));

	glVerify(glDrawElements(GL_TRIANGLES, m->GetNumFaces()*3, GL_UNSIGNED_INT, &m->m_indices[0]));

	glVerify(glDisableClientState(GL_VERTEX_ARRAY));
	glVerify(glDisableClientState(GL_NORMAL_ARRAY));


}

void Sabbia::draw_normals()
{
	int numVertices = int(m->GetNumVertices());
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	for (int i=0; i < numVertices; ++i) {
		glVertex3f(m->m_positions[i].x, m->m_positions[i].y, m->m_positions[i].z);
		glVertex3f(m->m_positions[i].x+m->m_normals[i].x/3.0f,
				m->m_positions[i].y+m->m_normals[i].y/3.0f,
				m->m_positions[i].z+m->m_normals[i].z/3.0f);
	}
	glEnd();
}


void Sabbia::ondula() {
    ondula(h_ondula + h_delta);
}

void Sabbia::ondula(float h) {
    h_ondula = h;
    if (h_ondula > 1) {
        h_ondula = 1.0f;
        h_delta = -h_delta;
    }
    if (h_ondula < 0) {
        h_ondula = 0.0f;
        h_delta = -h_delta;
    }
    printf("h_delta=%f ondula=%f\n", h_delta, h_ondula);
    int x, z;
    for (x = 0; x < grid; x++)
        for (z = 0; z < grid; z++)
            setH(2.0 * h_ondula *
            		(2+Sin(10.0*(z-h_ondula*grid)/grid)+Sin(20.0*(z-h_ondula*grid)/grid))*
            		(2+Sin(10.0*(x-h_ondula*grid)/grid)+Sin(20.0*(x-h_ondula*grid)/grid))/16.0
            		, x, z); //kPi

    float b=0.4;
    int c=grid-1;
    for (int i = 0; i < grid; i++) {
       	setH(max(b, getH(i,0)), i, 0);
       	setH(max(b, getH(i,c)), i, c);	glShadeModel(GL_SMOOTH);
       	setH(max(b, getH(0,i)), 0, i);
       	setH(max(b, getH(c,i)), c, i);
    }

    m->CalculateNormals();
    dirty = true;

}

void Sabbia::up(float h) {
    m->Transform(TranslationMatrix(Point3(0.0f, h, 0.0f)));
    dirty = true;
}
        
void Sabbia::draw_plane(float h) {
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_NOOP);
	Vec4 plane = Vec4(0.0f, 1.0f, 0.0f, -h); // piano orizzontale posto a +h
	DrawPlane(plane, false);
	glDisable(GL_COLOR_LOGIC_OP);
}

void Sabbia::toggle_normals() {
	show_normals = !show_normals;
}

