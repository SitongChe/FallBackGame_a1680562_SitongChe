#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "AppManager.h"
#include "Constants.h"
#include "Wall.h"

#include "Door.h"
#include "Mirror.h"
#include "PivotDoor.h"
#include "SlideDoor.h"
#include "Table.h"
#include "Chair.h"
#include "BMPLoader.h"
#include "TexturedSurface.h"
#include "Cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL/SOIL.h"
#include "tiny_obj_loader.h"
#include "particlesystem.h"
#include "shader_utils.h"
#include "particlesystem.h"
#include <iostream>
using namespace std;
ParticalSystem ps;
int screen_width=800, screen_height=600;
GLuint program;
GLuint normalmap_id;
GLuint heightmap_id;
GLuint sphere_vbo = -1;
GLint attribute_v_coord = -1, attribute_v_normal = -1, attribute_v_texcoords = -1, attribute_v_tangent = 1;
GLint uniform_m = -1, uniform_v = -1, uniform_p = -1,
uniform_m_3x3_inv_transp = -1, uniform_v_inv = -1,
uniform_normalmap = -1, uniform_heightmap=-1,
uniform_use_bump=-1,uniform_use_parallax=-1;
GLboolean use_bump=GL_TRUE;
GLboolean use_parallax=GL_FALSE;
bool flagg= false;
Mirror * tmpMirror;
struct demo {
    const char* texture_filename;
    const char* vshader_filename;
    const char* fshader_filename;
};
struct demo demos[] = {
    // Lighting of Bumpy Surfaces
    { "rockNormal.tga", "cube.v.glsl", "cube.f.glsl" },
};
int cur_demo = 0;

class Mesh {
private:
    GLuint vbo_vertices, vbo_normals, vbo_texcoords, vbo_tangents, ibo_elements;
public:
    vector<glm::vec4> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    vector<glm::vec3> tangents;
    vector<GLushort> elements;
    glm::mat4 object2world;
    
    Mesh() : vbo_vertices(0), vbo_normals(0), vbo_texcoords(0), vbo_tangents(0),
    ibo_elements(0), object2world(glm::mat4(1)) {}
    ~Mesh() {
        if (vbo_vertices != 0)
            glDeleteBuffers(1, &vbo_vertices);
        if (vbo_normals != 0)
            glDeleteBuffers(1, &vbo_normals);
        if (vbo_texcoords != 0)
            glDeleteBuffers(1, &vbo_texcoords);
        if (vbo_tangents != 0)
            glDeleteBuffers(1, &vbo_tangents);
        if (ibo_elements != 0)
            glDeleteBuffers(1, &ibo_elements);
    }
    
    /**
     * Express surface tangent in object space
     * http://www.terathon.com/code/tangent.html
     * http://www.3dkingdoms.com/weekly/weekly.php?a=37
     */
    void compute_tangents() {
        tangents.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
        
        for (int i = 0; i < elements.size(); i+=3) {
            int i1 = elements.at(i);
            int i2 = elements.at(i+1);
            int i3 = elements.at(i+2);
            glm::vec3 p1(vertices.at(i1));
            glm::vec3 p2(vertices.at(i2));
            glm::vec3 p3(vertices.at(i3));
            glm::vec2 uv1 = texcoords.at(i1);
            glm::vec2 uv2 = texcoords.at(i2);
            glm::vec2 uv3 = texcoords.at(i3);
            
            glm::vec3 p1p2 = p2 - p1;
            glm::vec3 p1p3 = p3 - p1;
            glm::vec2 uv1uv2 = uv2 - uv1;
            glm::vec2 uv1uv3 = uv3 - uv1;
            
            float c = uv1uv2.s * uv1uv3.t - uv1uv3.s * uv1uv2.t;
            if (c != 0) {
                float mul = 1.0 / c;
                glm::vec3 tangent = (p1p2 * uv1uv3.t - p1p3 * uv1uv2.t) * mul;
                tangents.at(i1) = tangents.at(i2) = tangents.at(i3) = glm::normalize(tangent);
            }
        }
    }
    
    /**
     * Store object vertices, normals and/or elements in graphic card
     * buffers
     */
    void upload() {
        if (this->vertices.size() > 0) {
            glGenBuffers(1, &this->vbo_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]),
                         this->vertices.data(), GL_STATIC_DRAW);
        }
        
        if (this->normals.size() > 0) {
            glGenBuffers(1, &this->vbo_normals);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
            glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]),
                         this->normals.data(), GL_STATIC_DRAW);
        }
        
        if (this->texcoords.size() > 0) {
            glGenBuffers(1, &this->vbo_texcoords);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texcoords);
            glBufferData(GL_ARRAY_BUFFER, this->texcoords.size() * sizeof(this->texcoords[0]),
                         this->texcoords.data(), GL_STATIC_DRAW);
        }
        
        if (this->tangents.size() > 0) {
            glGenBuffers(1, &this->vbo_tangents);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_tangents);
            glBufferData(GL_ARRAY_BUFFER, this->tangents.size() * sizeof(this->tangents[0]),
                         this->tangents.data(), GL_STATIC_DRAW);
        }
        
        if (this->elements.size() > 0) {
            glGenBuffers(1, &this->ibo_elements);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(this->elements[0]),
                         this->elements.data(), GL_STATIC_DRAW);
        }
    }
    
    /**
     * Draw the object
     */
    void draw() {
        if (this->vbo_vertices != 0) {
            glEnableVertexAttribArray(attribute_v_coord);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
            glVertexAttribPointer(
                                  attribute_v_coord,  // attribute
                                  4,                  // number of elements per vertex, here (x,y,z,w)
                                  GL_FLOAT,           // the type of each element
                                  GL_FALSE,           // take our values as-is
                                  0,                  // no extra data between each position
                                  0                   // offset of first element
                                  );
        }
        
        if (this->vbo_normals != 0) {
            glEnableVertexAttribArray(attribute_v_normal);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
            glVertexAttribPointer(
                                  attribute_v_normal,  // attribute
                                  3,                   // number of elements per vertex, here (x,y,z)
                                  GL_FLOAT,            // the type of each element
                                  GL_FALSE,            // take our values as-is
                                  0,                   // no extra data between each position
                                  0                    // offset of first element
                                  );
        }
        
        if (this->vbo_texcoords != 0) {
            glEnableVertexAttribArray(attribute_v_texcoords);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texcoords);
            glVertexAttribPointer(
                                  attribute_v_texcoords, // attribute
                                  2,                     // number of elements per vertex, here (x,y)
                                  GL_FLOAT,              // the type of each element
                                  GL_FALSE,              // take our values as-is
                                  0,                     // no extra data between each position
                                  0                      // offset of first element
                                  );
        }
        
        if (this->vbo_tangents != 0) {
            glEnableVertexAttribArray(attribute_v_tangent);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_tangents);
            glVertexAttribPointer(
                                  attribute_v_tangent, // attribute
                                  3,                   // number of elements per vertex, here (x,y,z)
                                  GL_FLOAT,            // the type of each element
                                  GL_FALSE,            // take our values as-is
                                  0,                   // no extra data between each position
                                  0                    // offset of first element
                                  );
        }
        
        /* Apply object's transformation matrix */
        glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(this->object2world));
        /* Transform normal vectors with transpose of inverse of upper left
         3x3 model matrix (ex-gl_NormalMatrix): */
        glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(this->object2world)));
        glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
        
        /* Push each element in buffer_vertices to the vertex shader */
        if (this->ibo_elements != 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
            int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        }
        
        if (this->vbo_tangents != 0)
            glDisableVertexAttribArray(attribute_v_tangent);
        if (this->vbo_texcoords != 0)
            glDisableVertexAttribArray(attribute_v_texcoords);
        if (this->vbo_normals != 0)
            glDisableVertexAttribArray(attribute_v_normal);
        if (this->vbo_vertices != 0)
            glDisableVertexAttribArray(attribute_v_coord);
    }
};
Mesh cube;

int init_resources()
{
    printf("init_resources: %s %s %s\n",
           demos[cur_demo].texture_filename, demos[cur_demo].vshader_filename, demos[cur_demo].fshader_filename);
    // TODO: try to optimize on Android by using ETC1 texture format
    normalmap_id = SOIL_load_OGL_texture
    (
     demos[cur_demo].texture_filename,
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
     );
    if (normalmap_id == 0)
        cerr << "SOIL loading error: '" << SOIL_last_result() << "' (" << demos[cur_demo].texture_filename << ")" << endl;
    
    heightmap_id = SOIL_load_OGL_texture
    (
     "rockHeight.tga",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_LOAD_L| SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
     );
    if (heightmap_id == 0)
        cerr << "SOIL loading error: '" << SOIL_last_result() << "' (" << "rockHeight.tga" << ")" << endl;
    
    GLint link_ok = GL_FALSE;
    
    GLuint vs, fs;
    if ((vs = create_shader(demos[cur_demo].vshader_filename, GL_VERTEX_SHADER))   == 0) return 0;
    if ((fs = create_shader(demos[cur_demo].fshader_filename, GL_FRAGMENT_SHADER)) == 0) return 0;
    
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        fprintf(stderr, "glLinkProgram:");
        print_log(program);
        return 0;
    }
    
    const char* attribute_name;
    attribute_name = "v_coord";
    attribute_v_coord = glGetAttribLocation(program, attribute_name);
    if (attribute_v_coord == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return 0;
    }
    attribute_name = "v_normal";
    attribute_v_normal = glGetAttribLocation(program, attribute_name);
    if (attribute_v_normal == -1) {
        fprintf(stderr, "Warning: Could not bind attribute %s\n", attribute_name);
    }
    attribute_name = "v_texcoords";
    attribute_v_texcoords = glGetAttribLocation(program, attribute_name);
    if (attribute_v_texcoords == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return 0;
    }
    attribute_name = "v_tangent";
    attribute_v_tangent = glGetAttribLocation(program, attribute_name);
    if (attribute_v_tangent == -1) {
        fprintf(stderr, "Warning: Could not bind attribute %s\n", attribute_name);
    }
    const char* uniform_name;
    uniform_name = "m";
    uniform_m = glGetUniformLocation(program, uniform_name);
    if (uniform_m == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }
    uniform_name = "v";
    uniform_v = glGetUniformLocation(program, uniform_name);
    if (uniform_v == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }
    uniform_name = "p";
    uniform_p = glGetUniformLocation(program, uniform_name);
    if (uniform_p == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }
    uniform_name = "m_3x3_inv_transp";
    uniform_m_3x3_inv_transp = glGetUniformLocation(program, uniform_name);
    if (uniform_m_3x3_inv_transp == -1) {
        fprintf(stderr, "Warning: Could not bind uniform %s\n", uniform_name);
    }
    uniform_name = "v_inv";
    uniform_v_inv = glGetUniformLocation(program, uniform_name);
    if (uniform_v_inv == -1) {
        fprintf(stderr, "Warning: Could not bind uniform %s\n", uniform_name);
    }
    uniform_name = "normalmap";
    uniform_normalmap = glGetUniformLocation(program, uniform_name);
    if (uniform_normalmap == -1) {
        fprintf(stderr, "Warning: Could not bind uniform %s\n", uniform_name);
    }
    
    uniform_name = "heightmap";
    uniform_heightmap = glGetUniformLocation(program, uniform_name);
    if (uniform_heightmap == -1) {
        fprintf(stderr, "Warning: Could not bind uniform %s\n", uniform_name);
    }
    uniform_name = "use_bump";
    uniform_use_bump = glGetUniformLocation(program, uniform_name);
    if (uniform_use_bump == -1) {
        fprintf(stderr, "Warning: Could not bind uniform %s\n", uniform_name);
    }
    
    uniform_name = "use_parallax";
    uniform_use_parallax = glGetUniformLocation(program, uniform_name);
    if (uniform_use_parallax == -1) {
        fprintf(stderr, "Warning: Could not bind uniform %s\n", uniform_name);
    }
    
    
    // Cube
    
    // front
    cube.vertices.push_back(glm::vec4(-1.0, -1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0, -1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0,  1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0,  1.0,  1.0,  1.0));
    // top
    cube.vertices.push_back(glm::vec4(-1.0,  1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0,  1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0,  1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0,  1.0, -1.0,  1.0));
    // back
    cube.vertices.push_back(glm::vec4( 1.0, -1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0, -1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0,  1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0,  1.0, -1.0,  1.0));
    // bottom
    cube.vertices.push_back(glm::vec4(-1.0, -1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0, -1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0, -1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0, -1.0,  1.0,  1.0));
    // left
    cube.vertices.push_back(glm::vec4(-1.0, -1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0, -1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0,  1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4(-1.0,  1.0, -1.0,  1.0));
    // right
    cube.vertices.push_back(glm::vec4( 1.0, -1.0,  1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0, -1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0,  1.0, -1.0,  1.0));
    cube.vertices.push_back(glm::vec4( 1.0,  1.0,  1.0,  1.0));
    
    for (int i = 0; i < 4; i++) { cube.normals.push_back(glm::vec3( 0.0,  0.0,  1.0)); }  // front
    for (int i = 0; i < 4; i++) { cube.normals.push_back(glm::vec3( 0.0,  1.0,  0.0)); }  // top
    for (int i = 0; i < 4; i++) { cube.normals.push_back(glm::vec3( 0.0,  0.0, -1.0)); }  // back
    for (int i = 0; i < 4; i++) { cube.normals.push_back(glm::vec3( 0.0, -1.0,  0.0)); }  // bottom
    for (int i = 0; i < 4; i++) { cube.normals.push_back(glm::vec3(-1.0,  0.0,  0.0)); }  // left
    for (int i = 0; i < 4; i++) { cube.normals.push_back(glm::vec3( 1.0,  0.0,  0.0)); }  // right
    
    for (int i = 0; i < 6; i++) {
        // front
        cube.texcoords.push_back(glm::vec2(0.0, 0.0));
        cube.texcoords.push_back(glm::vec2(1.0, 0.0));
        cube.texcoords.push_back(glm::vec2(1.0, 1.0));
        cube.texcoords.push_back(glm::vec2(0.0, 1.0));
    }
    
    // Triangulate
    GLushort cube_elements[] = {
        // front
        0,  1,  2,
        2,  3,  0,
        // top
        4,  5,  6,
        6,  7,  4,
        // back
        8,  9, 10,
        10, 11,  8,
        // bottom
        12, 13, 14,
        14, 15, 12,
        // left
        16, 17, 18,
        18, 19, 16,
        // right
        20, 21, 22,
        22, 23, 20,
    };
    for (int i = 0; i < sizeof(cube_elements)/sizeof(cube_elements[0]); i++)
        cube.elements.push_back(cube_elements[i]);
    
    cube.compute_tangents();
    
    cube.upload();
    
    return 1;
}


AppManager::AppManager()
{
	cam = new Camera(8.75, 0.1, 0.0);
    cam1 = new Camera(8.75, 0.1, 0.0);
	flashlightOn = true;
	state = 1;
	hasKey = false;
	freakoutAspectAngle = 0.0;
	freakoutPerspAngle = 0.0;
	roomNo = -1;
	flash = NULL;
	ghost = NULL;
	snowman = NULL;
	westWindowDoor = NULL;
	lockedDoor = NULL;
	key = NULL;
	skybox = NULL;
	carousel = NULL;
	clock = NULL;
    //cube= NULL;

	fillPolygons = true;

	freakoutAspectAngle = 0.0;
	freakoutAspectAmplitude = 0.0;
	freakoutAspectPeriod = 0.0;
	freakoutPerspAngle = 0.0;
	freakoutPerspAmplitude = 0.0;
	freakoutPerspPeriod = 0.0;
}

AppManager::~AppManager()
{
	for (unsigned int i = 0; i < objects.size(); i++)
		delete objects[i];

	for (unsigned int i = 0; i < materials.size(); i++)
		delete materials[i];

	for (unsigned int i = 0; i < windows.size(); i++)
		delete windows[i];
    


	glDeleteTextures(1, &textures[WALL]);
	glDeleteTextures(1, &textures[OUTER_WALL]);
	glDeleteTextures(1, &textures[FLOOR]);
	glDeleteTextures(1, &textures[WOOD_DOOR]);
	glDeleteTextures(1, &textures[METAL_DOOR]);
	glDeleteTextures(1, &textures[WINDOW]);
	glDeleteTextures(1, &textures[CAKE]);
	glDeleteTextures(1, &textures[DEMON]);
	glDeleteTextures(1, &textures[BLACK]);
	glDeleteTextures(1, &textures[SKY_UP]);
	glDeleteTextures(1, &textures[SKY_DOWN]);
	glDeleteTextures(1, &textures[SKY_NORTH]);
	glDeleteTextures(1, &textures[SKY_SOUTH]);
	glDeleteTextures(1, &textures[SKY_EAST]);
	glDeleteTextures(1, &textures[SKY_WEST]);

	delete [] textures;

	delete cam;
    delete cam1;
	delete survCam;
	delete ghost;
	delete snowman;
	delete skybox;
	delete key;
}

void AppManager::init()
{
	initGL();
	initLights();
	initMaterials();
	initTextures();
	initWorld();
    ps=ParticalSystem(100,-15.0);
    ps.init();
}

void AppManager::initGL()
{
	glClearColor(0.2, 0.2, 0.2, 1);
	glColor3f(0, 0, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutSetCursor(GLUT_CURSOR_NONE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, ASPECT_RATIO, 0.1, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void AppManager::initLights()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	// flashlight; spotlight always pointing in front of the player
	glEnable(GL_LIGHT1);	// light in room 1; red spotlight in the roof
    glEnable(GL_LIGHT4);	// light in room 1; red spotlight in the roof
	glEnable(GL_LIGHT2);	// light in room 2	blue spotlight in the roof
    glEnable(GL_LIGHT5);	// light in room 2	blue spotlight in the roof
	glEnable(GL_LIGHT3);	// light in room 3; green spotlight in the roof
    glEnable(GL_LIGHT6);	// light in room 3	blue spotlight in the roof
    glEnable(GL_LIGHT7);

	// since we're moving around the scene, we enable local lighting stuff
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	//float globalAmbient[] = { 0.05, 0.05, 0.05, 1.0 };
	float globalAmbient[] = { 0.05, 0.05, 0.05, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	float position0[] = { 0.0, 0.0, 0.0, 1.0 };	
	float specular0[] = { 1.0, 1.0, 1.0, 1.0 };
	float diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
	float ambient0[] = { 0.0, 0.0, 0.0, 1.0 };
	float direction0[] = { 0.0, 0.0, -1.0, 0.0 };
	float exponent0 = 1.0;
	float cutoff0 = 10.0;

	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent0);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff0);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.3);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.2);




	float specular1[] = { 0.4, 0.4, 0.4, 1.0 };
	float diffuse1[] = { 0.8, 0.0, 0.0, 1.0 };
	float ambient1[] = { 0.0, 0.0, 0.0, 1.0 };
	float exponent1 = 0.5;
	float cutoff1 = 80.0;

	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, exponent1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, cutoff1);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.3);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);
    
    glLightfv(GL_LIGHT4, GL_SPECULAR, specular1);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT4, GL_AMBIENT, ambient1);
    glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, exponent1);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, cutoff1);
    glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, 0.3);
    glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.1);
    
    glLightfv(GL_LIGHT7, GL_SPECULAR, specular1);
    glLightfv(GL_LIGHT7, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT7, GL_AMBIENT, ambient1);
    glLightf(GL_LIGHT7, GL_SPOT_EXPONENT, exponent1);
    glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, cutoff1);
    glLightf(GL_LIGHT7, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT7, GL_LINEAR_ATTENUATION, 0.3);
    glLightf(GL_LIGHT7, GL_QUADRATIC_ATTENUATION, 0.1);


	float specular2[] = { 0.2, 0.2, 0.2, 1.0 };
	float diffuse2[] = { 0.0, 0.0, 0.9, 1.0 };
	float ambient2[] = { 0.0, 0.0, 0.0, 1.0 };
	float exponent2 = 0.5;
	float cutoff2 = 80.0;

	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, exponent2);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, cutoff2);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.2);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.1);
    
    glLightfv(GL_LIGHT5, GL_SPECULAR, specular2);
    glLightfv(GL_LIGHT5, GL_DIFFUSE, diffuse2);
    glLightfv(GL_LIGHT5, GL_AMBIENT, ambient2);
    glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, exponent2);
    glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, cutoff2);
    glLightf(GL_LIGHT5, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION, 0.2);
    glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, 0.1);


	float specular3[] = { 0.6, 0.6, 0.6, 1.0 };
	float diffuse3[] = { 0.0, 0.9, 0.0, 1.0 };
	float ambient3[] = { 0.0, 0.0, 0.0, 1.0 };
	float exponent3 = 0.5;
	float cutoff3 = 80.0;

	glLightfv(GL_LIGHT3, GL_SPECULAR, specular3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse3);
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient3);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, exponent3);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, cutoff3);
	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.5);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.2);
    
    glLightfv(GL_LIGHT6, GL_SPECULAR, specular3);
    glLightfv(GL_LIGHT6, GL_DIFFUSE, diffuse3);
    glLightfv(GL_LIGHT6, GL_AMBIENT, ambient3);
    glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, exponent3);
    glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, cutoff3);
    glLightf(GL_LIGHT6, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(GL_LIGHT6, GL_LINEAR_ATTENUATION, 0.5);
    glLightf(GL_LIGHT6, GL_QUADRATIC_ATTENUATION, 0.2);
}

void AppManager::setLights()
{
	// First we set the position and direction of the lights, since we don't want them to change as
	// the camera moves around	
	float position1[] = { 8.75, 2.69, 0.0, 1.0 };
	float direction1[] = { 0.0, -1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    
    float position7[] = { 1.0,1.4,-7.3, 1.0 };
    float direction7[] = { 0.0, 1.0, 0.0, 0.0 };
    glLightfv(GL_LIGHT7, GL_POSITION, position7);
    glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, direction7);
    
    float position11[] = { 9.15, 2.69, 0.0, 1.0 };
    float direction11[] = { 0.0, -1.0, 0.0, 0.0 };
    glLightfv(GL_LIGHT4, GL_POSITION, position11);
    glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, direction11);
	
	float position2[] = { 0.0, 2.69, -8.75, 1.0 };
	float direction2[] = { 0.0, -1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT2, GL_POSITION, position2);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction2);
    
    float position22[] = { 0.4, 2.69, -8.75, 1.0 };
    float direction22[] = { 0.0, -1.0, 0.0, 0.0 };
    glLightfv(GL_LIGHT5, GL_POSITION, position22);
    glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, direction22);

	float position3[] = { -8.75, 2.69, 0.0, 1.0 };
	float direction3[] = { 0.0, -1.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT3, GL_POSITION, position3);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction3);
    
    float position33[] = { -9.15, 2.69, 0.0, 1.0 };
    float direction33[] = { 0.0, -1.0, 0.0, 0.0 };
    glLightfv(GL_LIGHT6, GL_POSITION, position33);
    glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, direction33);

	// Next, we draw the things representing the lights. We see both the back and the front side
	// of these lights, so we disable culling temporarily
	glDisable(GL_CULL_FACE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	Material::setWhiteMaterial();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glPushMatrix();
	glTranslatef(8.75, 2.5, 0.0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(0.2, 0.2, 10, 10);
	glPopMatrix();
    

    glPushMatrix();
    glTranslatef(9.15, 2.5, 0.0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.2, 0.2, 10, 10);
    glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 2.5, -8.75);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(0.2, 0.2, 10, 10);
	glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4, 2.5, -8.75);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.2, 0.2, 10, 10);
    glPopMatrix();
    
	glPushMatrix();
	glTranslatef(-8.75, 2.5, 0.0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(0.2, 0.2, 10, 10);
	glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-9.15, 2.5, 0.0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.2, 0.2, 10, 10);
    glPopMatrix();
	
	glEnable(GL_CULL_FACE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

void AppManager::setGlobalFlashlight()
{
	Vector3f camPos = cam->getPos();
	float position[] = { camPos.getX(), camPos.getY() + cam->getCharHeight(), camPos.getZ(), 1.0 };
	float theta = -(90 - cam->getTheta()) * 3.1416 / 180.0;
	float phi = (cam->getPhi() + 90) * 3.1416 / 180.0;

	Vector3f dirVector;
	dirVector.setX( sin(phi) * cos(theta) );	// spherical coordinates dude!
	dirVector.setZ( sin(phi) * sin(theta) );
	dirVector.setY( cos(phi) );

	float direction[] = { dirVector.getX(), dirVector.getY(), dirVector.getZ(), 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
}


void AppManager::setLocalFlashlight()
{
	float position[] = { 0.0, 0.0, 0.0, 1.0 };
	float direction[] = { 0.0, 0.0, -1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
}

void AppManager::initMaterials()
{
	materials.push_back(new Material(GL_FRONT));
	materials[WALL]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[WALL]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[WALL]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[WALL]->setShininess(30);

	materials.push_back(new Material(GL_FRONT));
	materials[OUTER_WALL]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[OUTER_WALL]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[OUTER_WALL]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[OUTER_WALL]->setShininess(30);

	materials.push_back(new Material(GL_FRONT));
	materials[FLOOR]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[FLOOR]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[FLOOR]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[FLOOR]->setShininess(30);

	materials.push_back(new Material(GL_FRONT));
	materials[WOOD_DOOR]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[WOOD_DOOR]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[WOOD_DOOR]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[WOOD_DOOR]->setShininess(30);

	materials.push_back(new Material(GL_FRONT));
	materials[METAL_DOOR]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[METAL_DOOR]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[METAL_DOOR]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[METAL_DOOR]->setShininess(100);

	materials.push_back(new Material(GL_FRONT_AND_BACK));
	materials[WINDOW]->setSpecular(0.0, 0.0, 0.0, 0.55);
	materials[WINDOW]->setDiffuse(1.0, 1.0, 1.0, 0.55);		
	materials[WINDOW]->setAmbient(1.0, 1.0, 1.0, 0.55);
	materials[WINDOW]->setShininess(30);

	materials.push_back(new Material(GL_FRONT));
	materials[FINAL_CORRIDOR]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[FINAL_CORRIDOR]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[FINAL_CORRIDOR]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[FINAL_CORRIDOR]->setEmission(1.0, 1.0, 1.0, 1.0);
	materials[FINAL_CORRIDOR]->setShininess(0);

	materials.push_back(new Material(GL_FRONT));
	materials[CAKE]->setSpecular(1.0, 1.0, 1.0, 1.0);
	materials[CAKE]->setDiffuse(1.0, 1.0, 1.0, 1.0);		
	materials[CAKE]->setAmbient(1.0, 1.0, 1.0, 1.0);
	materials[CAKE]->setEmission(0.5, 0.5, 0.5, 1.0);
	materials[CAKE]->setShininess(10);
}


void AppManager::initTextures()
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	textures = new GLuint[16];
	Texture tmpTexture;

	textures[FINAL_CORRIDOR] = 0;

	load_bmp("resources/rust.bmp", 24, tmpTexture);	
	glGenTextures(1, &textures[WALL]);
	glBindTexture(GL_TEXTURE_2D, textures[WALL]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/concrete.bmp", 24, tmpTexture);	
	glGenTextures(1, &textures[OUTER_WALL]);
	glBindTexture(GL_TEXTURE_2D, textures[OUTER_WALL]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/wood.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[FLOOR]);
	glBindTexture(GL_TEXTURE_2D, textures[FLOOR]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/woodDoor.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[WOOD_DOOR]);
	glBindTexture(GL_TEXTURE_2D, textures[WOOD_DOOR]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/metalDoor2.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[METAL_DOOR]);
	glBindTexture(GL_TEXTURE_2D, textures[METAL_DOOR]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/window.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[WINDOW]);
	glBindTexture(GL_TEXTURE_2D, textures[WINDOW]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/cake.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[CAKE]);
	glBindTexture(GL_TEXTURE_2D, textures[CAKE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tmpTexture.width, tmpTexture.height, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data );

	load_bmp("resources/demon.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[DEMON]);
	glBindTexture(GL_TEXTURE_2D, textures[DEMON]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/black.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[BLACK]);
	glBindTexture(GL_TEXTURE_2D, textures[BLACK]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/skybox_UP.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[SKY_UP]);
	glBindTexture(GL_TEXTURE_2D, textures[SKY_UP]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/skybox_DN.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[SKY_DOWN]);
	glBindTexture(GL_TEXTURE_2D, textures[SKY_DOWN]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/skybox_FR.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[SKY_NORTH]);
	glBindTexture(GL_TEXTURE_2D, textures[SKY_NORTH]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/skybox_BK.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[SKY_SOUTH]);
	glBindTexture(GL_TEXTURE_2D, textures[SKY_SOUTH]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/skybox_RT.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[SKY_EAST]);
	glBindTexture(GL_TEXTURE_2D, textures[SKY_EAST]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);

	load_bmp("resources/skybox_LF.bmp", 24, tmpTexture);
	glGenTextures(1, &textures[SKY_WEST]);
	glBindTexture(GL_TEXTURE_2D, textures[SKY_WEST]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpTexture.width, tmpTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpTexture.data);
}

void AppManager::render()
{
   
	// First, render the scene for the surveillance camera
    //handleEvents( );
    ps.simulate(0.01);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	survCam->prepareCam();
	setLights();
	setGlobalFlashlight();
	for (unsigned int i = 0; i < objects.size(); i++)
		objects[i]->draw();
    glPushMatrix();
    glTranslatef(1.0f,1.165f,-7.3f);
    GLUquadricObj *p = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(p, 0.05, 0.05, 0.24, 20, 20);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.0f,1.4f,-7.3f);
    ps.render();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.0f,1.4f,-7.3f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.05, 0.05, 10, 10);
    glPopMatrix();
    

	cam->drawCharacter();	// only draw the player "model" here
	ghost->draw();	// only draw the ghost the the camera is filming....
	survCam->drawScreen();
	survCam->createTexture();




	// Done with the texture for the surveillance monitor - time for the actual scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
    //cam->drawCharacter();
	setLocalFlashlight();
	cam->setCamera();
    if(flagg)
    {
    cam1->setCamera();
   // gluLookAt(0, 0.1, 0, 0, -0.02, 0.01, 0, 1, 0);
    gluLookAt(cam->getPos().getX(), cam->getPos().getY(), cam->getPos().getZ()+3.0, cam->getPos().getX(), cam->getPos().getY(), cam->getPos().getZ()-9.7, 0, 1, 0);
    }
	setLights();
    glPushMatrix();
    glTranslatef(1.0f,1.165f,-7.3f);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(p, 0.05, 0.05, 0.24, 20, 20);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1.0f,1.4f,-7.3f);
    ps.render();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1.0f,1.4f,-7.3f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.05, 0.05, 10, 10);
    glPopMatrix();
	if (skybox != NULL)
		skybox->draw();

	for (unsigned int i = 0; i < objects.size(); i++)
		objects[i]->draw();

	if (key != NULL)
		key->draw();

	survCam->drawScreen();
	survCam->drawCam();

	if (snowman != NULL)
		snowman->draw();

	for (unsigned int i = 0; i < windows.size(); i++)
		windows[i]->draw();


	// remember to draw the flash LAST, so that the alpha blending is correct
	if (flash != NULL)
		flash->drawFlash();

    if(state==9)
    {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
        
        float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 10;  // 10° per second
        glm::mat4 anim = \
        glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(1, 0, 0)) *  // X axis
        glm::rotate(glm::mat4(1.0f), angle*2.0f, glm::vec3(0, 1, 0)) *  // Y axis
        glm::rotate(glm::mat4(1.0f), angle*4.0f, glm::vec3(0, 0, 1));   // Z axis

    glm::vec3 object_position = glm::vec3(8.75, 1.35, 3.5);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), object_position);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 4.0, 0.0), object_position, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 10.0f);
    
    glUseProgram(program);
    cube.object2world = model*anim ;
    
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 v_inv = glm::inverse(view);
    glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));
    
    glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(projection));
    
    glutPostRedisplay();
    glUniform1i(uniform_use_bump,use_bump);
    glUniform1i(uniform_use_parallax,use_parallax);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalmap_id);
    glUniform1i(uniform_normalmap, /*GL_TEXTURE*/0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, heightmap_id);
    glUniform1i(uniform_heightmap, /*GL_TEXTURE*/1);
    
    cube.draw();
    }

    

//SDL_GL_SwapBuffers( );
	glutSwapBuffers();
}

void AppManager::update()
{
	cam->update();	// update position and stuff
//    tmpMirror->prepareMirror(cam->getPos());
//    tmpMirror->createTexture();
//    tmpMirror->draw();
	// store the value of the (potentially) new room number in a temporary
	// integer. Used to set new state values
	int newRoomNo = cam->roomNumber();

	if (state == 1)
	{
		// if we've, in this state, opened the door to the window in room 3, 
		// create a snowman and change state to 2
		if (!westWindowDoor->isClosed())
		{
			snowman = new Snowman(8.0, 0.0, 0.0);
			snowman->look();
            
			state = 2;
		}
	}

	else if (state == 2)
	{
		// if we've moved into the western corridor in this state, remove
		// the snowman and change state to 3
		if (newRoomNo == 5)
		{
			delete snowman;
			snowman = NULL;
			state = 3;
		}
	}

	else if (state == 3)
	{
		// if we've moved into the eastern corridor in this state, create
		// a creepy demon flash and change state to 4. This is also a good
		// time to place the key in room 1.
		if (newRoomNo == 4)
		{
			flash = new Flash(textures[DEMON], false, 0.003, 0.4);
			key = new Key(8.0, 0.1, 0.0);
			state = 4;
		}
	}

	else if (state == 4)
	{
		// if the player has moved into the first room, change state to 5 and
		// set perspective stuff to weird shit yo.
		if (newRoomNo == 1)
		{
			freakoutAspectAngle = 0.0;
			freakoutAspectAmplitude = 0.8;
			freakoutAspectPeriod = 0.006;
			freakoutPerspAngle = 0.0;
			freakoutPerspAmplitude = 8.0;
			freakoutPerspPeriod = 0.01;
			state = 5;
		}
	}

	else if (state == 5)
	{
		if (hasKey)
		{
			freakoutAspectPeriod = 0.01;
			freakoutPerspPeriod = 0.03;
			state = 6;
		}

		else
			freakOutPersp();
	}

	else if (state == 6)
	{
		if (newRoomNo == 4)
		{
			snowman = new Snowman(8.75, 0.0, -11.0);
			snowman->charge();
			state = 7;
		}

		else
			freakOutPersp();
	}

	else if (state == 7)
	{
		if (snowman->isDone())
		{
			resetPersp();
			delete snowman;
			snowman = NULL;
			flash = new Flash(textures[BLACK], 0, 0.001, 1.0);
			state = 8;
		}
		
		else
			freakOutPersp();
	}

	else if (state == 8)
	{
		if (cam->getPos().getY() < -100.0)	// here, the character dies. harsch.
		{
			glClearColor(0.8, 0.0, 0.0, 0.5);	// set clear color to red

			glMatrixMode(GL_PROJECTION);		// make sure we don't see anything of the stuff, only red
			glLoadIdentity();
			gluPerspective(FIELD_OF_VIEW, ASPECT_RATIO, 0.1, 10);
			glMatrixMode(GL_MODELVIEW);

			delete skybox;	// make sure that we don't see the skybox, no matter what settings we used for it
			skybox = NULL;
			state = 9;
		}
	}

	// update the room number
	roomNo = newRoomNo;

	for (unsigned int i = 0; i < objects.size(); i++)
		objects[i]->update();

	for (unsigned int i = 0; i < windows.size(); i++)
		windows[i]->update();


	ghost->update(cam->getPos());

	if (snowman != NULL)
		snowman->update(cam->getPos());

	if (skybox != NULL)
		skybox->update(cam->getPos());

	if (flash != NULL)
	{
		flash->update();

		if (flash->isDone())
		{
			delete flash;
			flash = NULL;
		}
	}
}

void AppManager::keybDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':	cam->moveForward(true);		break;	// move camera forward
	case 's':	cam->moveBack(true);		break;	// move camera back
	case 'a':	cam->moveLeft(true);		break;	// move camera left
	case 'd':	cam->moveRight(true);		break;	// move camera right
    case 'h':	if(!flagg)
                {
                    flagg=true;
                    
                    cam1 = new Camera(-cam->getPos().getX(), cam->getPos().getY(), -cam->getPos().getZ());
                    break;
                }
                else
                {
                    flagg=false;
                    delete cam1;
                    break;
                }
	case 'f':	flashlightOn = !flashlightOn;	flashlightOn ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0); break;
	case 'e':	interact();	break;	// interact with the environment; check interact()
	case '0':	fillPolygons = !fillPolygons;	glPolygonMode(GL_FRONT_AND_BACK, fillPolygons ? GL_FILL : GL_LINE);	break;	// fill polygons, or just draw the lines?
	case 'm':	carousel->addRotSpeed(1.0);	break;
	case 27:	exit(0);					break;	// exit program if escape is pressed
	}
}


void AppManager::keybUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':	cam->moveForward(false);	break;	// stop moving the camera forward
	case 's':	cam->moveBack(false);		break;	// stop moving the camera back
	case 'a':	cam->moveLeft(false);		break;	// stop moving the camera left
	case 'd':	cam->moveRight(false);		break;	// stop moving the camera right
	}
}


void AppManager::mouseFunc(int button, int state, int x, int y)
{
	cam->rotLeftRight( -((WINDOW_WIDTH / 2) - x));
	cam->rotUpDown( -((WINDOW_HEIGHT / 2) - y));
}

void AppManager::interact()
{
	// any doors to be opened?
	for (unsigned int i = 0; i < doors.size(); i++)
		doors[i]->open(cam->getPos());

	// only try to open the locked door if we have the key
	if (hasKey)
		lockedDoor->open(cam->getPos());

	// if we've managed to pick up the key, then we, yeah, have the key!
	if (key != NULL && key->pickup(cam->getPos()))
	{
		hasKey = true;
		delete key;
		key = NULL;
	}
}


void AppManager::freakOutPersp()
{
	freakoutAspectAngle += freakoutAspectPeriod;
	float aspectOffset = sin(freakoutAspectAngle) * freakoutAspectAmplitude;

	freakoutPerspAngle += freakoutPerspPeriod;
	float perspOffset = sin(freakoutPerspAngle) * freakoutPerspAmplitude;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW + perspOffset, ASPECT_RATIO + aspectOffset, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
}


void AppManager::resetPersp()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, ASPECT_RATIO, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
}

// This is the god-awful method which creates the environment and most objects in the program. I've tried to comment it
// well to make it as painless as possible, but there are lots of "numbers which are just there".
void AppManager::initWorld()
{
	// CREATE THE GHOST IN ROOM 2 OMG!
	ghost = new Ghost(2.5, 1.0, -10.0);
    //cube= new Cube(0.0, 0.0, 2.0);
    init_resources();
	// create skybox
	skybox = new SkyBox(100, textures[SKY_UP], textures[SKY_DOWN], textures[SKY_NORTH], textures[SKY_SOUTH], textures[SKY_EAST], textures[SKY_WEST]);

	// create the carousel
	carousel = new Carousel(0.0, 0.0, 2.0, NULL, materials[METAL_DOOR]);
	objects.push_back(carousel);

	// create the grandfather clock in room 3
	clock = new GrandfatherClock(-8.75, 1.0, 3.5, NULL, NULL);
	clock->rotate(180, 0, 1, 0);
	objects.push_back(clock);

	BoundingBox * tmpBox;
	Wall * tmpWall;
	Door * tmpDoor;
    

	// Start building walls and stuff for room 1

	// floor START
	tmpBox = new BoundingBox(8.75, 0, 0, 4.0, 0.1, 7.0, 1);
	tmpWall = new Wall(8.75, 0, 0, tmpBox, materials[FLOOR], 4.0, 7.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);

	objects.push_back(tmpWall);
	// floor END

	// roof START
	tmpBox = new BoundingBox(8.75, 2.7, 0, 4.0, 0.1, 7.0);
	tmpWall = new Wall(8.75, 2.7, 0, tmpBox, materials[FLOOR], 4.0, 7.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// roof END

	// south wall START
	tmpBox = new BoundingBox(8.75, 1.35, 3.5, 4.0, 2.7, 0.1);
//	tmpWall = new Wall(8.75, 1.35, 3.5, tmpBox, materials[WALL], 4.0, 2.7, textures[WALL], 0.5, 0.3);
//    tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
//    tmpWall->rotate(180, 1, 0, 0);
    tmpMirror= new Mirror(8.75, 0,0, 4.0, 7.0, 180.0, tmpBox);
    //Vector3f a = (8.75, 1.0, -3.5);
//    tmpMirror->prepareMirror(cam->getPos());
//    tmpMirror->createTexture();
    objects.push_back(tmpMirror);
//    objects.push_back(tmpWall);
	// south wall END

	// north wall START
	tmpBox = new BoundingBox(10.125, 1.35, -3.5, 1.25, 2.7, 0.1);
	tmpWall = new Wall(10.125, 1.35, -3.5, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(7.375, 1.35, -3.5, 1.25, 2.7, 0.1);
	tmpWall = new Wall(7.375, 1.35, -3.5, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(8.75, 2.35, -3.5, 1.5, 0.7, 0.1);
	tmpWall = new Wall(8.75, 2.35, -3.5, tmpBox, materials[WALL], 1.5, 0.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(8.125 , 1.0, -3.5, 0.25, 2.0, 0.1);
	tmpWall = new Wall(8.125, 1.0, -3.5, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(9.375, 1.0, -3.5, 0.25, 2.0, 0.1);
	tmpWall = new Wall(9.375, 1.0, -3.5, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	objects.push_back(tmpWall);
	// north wall END

	// east wall START
	tmpBox = new BoundingBox(10.75, 1.35, 0.0, 0.1, 2.7, 7.0);
	tmpWall = new Wall(10.75, 1.35, 0.0, tmpBox, materials[WALL], 7.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);

	objects.push_back(tmpWall);
	// east wall END

	// west wall START
	tmpBox = new BoundingBox(6.75, 0.575, 0.0, 0.1, 1.15, 7.0);
	tmpWall = new Wall(6.75, 0.575, 0.0, tmpBox, materials[WALL], 7.0, 1.15, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(6.75, 1.6, 1.975, 0.1, 0.9, 3.05);
	tmpWall = new Wall(6.75, 1.6, 1.975, tmpBox, materials[WALL], 3.05, 0.9, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(6.75, 1.6, -1.975, 0.1, 0.9, 3.05);
	tmpWall = new Wall(6.75, 1.6, -1.975, tmpBox, materials[WALL], 3.05, 0.9, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(6.75, 2.375, 0.0, 0.1, 0.65, 0.7);
	tmpWall = new Wall(6.75, 2.375, 0.0, tmpBox, materials[WALL], 7.0, 0.65, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// west wall END


	// door room1-eastern corridor START
	tmpBox = new BoundingBox(8.75, 1.0, -3.5, 1.0, 2.0, 0.1);
	tmpDoor = new PivotDoor(8.75, 1.0, -3.5, 9.25, 1.0, -3.5, 1.0, 2.0, tmpBox, materials[WOOD_DOOR], textures[WOOD_DOOR], 1.0, 2.0);
	objects.push_back(tmpDoor);
	doors.push_back(tmpDoor);
	// door room1-eastern corridor END

	// window for room 1 START. Window without texture.
	windows.push_back(new Window(6.75, 1.6, 0.0, 0.9, 0.9, NULL, materials[WINDOW]));
	windows[windows.size()-1]->rotate(90, 0, 1, 0);
	// window for room 1 END

	// Done building walls and stuff for room 1




	// Start building walls and stuff for room 2

	// floor START
	tmpBox = new BoundingBox(0, 0, -8.75, 7, 0.1, 4, 2);
	tmpWall = new Wall(0, 0, -8.75, tmpBox, materials[FLOOR], 7.0, 4.0, textures[FLOOR], 1.0, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// floor END

	// roof START
	tmpBox = new BoundingBox(0, 2.7, -8.75, 7, 0.1, 4);
	tmpWall = new Wall(0, 2.7, -8.75, tmpBox, materials[FLOOR], 7.0, 4.0, textures[FLOOR], 1.0, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// roof END

	// north wall START
	tmpBox = new BoundingBox(0.0, 1.35, -10.75, 7.0, 2.7, 0.1);
	tmpWall = new Wall(0.0, 1.35, -10.75, tmpBox, materials[WALL], 7.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);
	// north wall END


	// south wall START
	tmpBox = new BoundingBox(0, 1.35, -6.75, 7, 2.7, 0.1);
	tmpWall = new Wall(0, 1.35, -6.75, tmpBox, materials[WALL], 7.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(180, 0, 1, 0);
	objects.push_back(tmpWall);
	// south wall END

	// east wall START
	tmpBox = new BoundingBox(3.5, 1.35, -10.125, 0.1, 2.7, 1.25);
	tmpWall = new Wall(3.5, 1.35, -10.125, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(270, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(3.5, 1.35, -7.375, 0.1, 2.7, 1.25);
	tmpWall = new Wall(3.5, 1.35, -7.375, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(270, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(3.5, 2.35, -8.75, 0.1, 0.7, 1.5);
	tmpWall = new Wall(3.5, 2.35, -8.75, tmpBox, materials[WALL], 1.5, 0.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	tmpWall->rotate(270, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(3.5 , 1.0, -8.125, 0.1, 2.0, 0.25);
	tmpWall = new Wall(3.5, 1.0, -8.125, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	tmpWall->rotate(270, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(3.5, 1.0, -9.375, 0.1, 2.0, 0.25);
	tmpWall = new Wall(3.5, 1.0, -9.375, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	tmpWall->rotate(270, 0, 1, 0);
	objects.push_back(tmpWall);
	// east wall END

	// west wall START
	tmpBox = new BoundingBox(-3.5, 1.35, -10.125, 0.1, 2.7, 1.25);
	tmpWall = new Wall(-3.5, 1.35, -10.125, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-3.5, 1.35, -7.375, 0.1, 2.7, 1.25);
	tmpWall = new Wall(-3.5, 1.35, -7.375, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-3.5, 2.35, -8.75, 0.1, 0.7, 1.5);
	tmpWall = new Wall(-3.5, 2.35, -8.75, tmpBox, materials[WALL], 1.5, 0.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-3.5 , 1.0, -8.125, 0.1, 2.0, 0.25);
	tmpWall = new Wall(-3.5, 1.0, -8.125, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-3.5, 1.0, -9.375, 0.1, 2.0, 0.25);
	tmpWall = new Wall(-3.5, 1.0, -9.375, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// west wall END

	// door eastern corridor-room2 START
	tmpBox = new BoundingBox(3.5, 1.0, -8.75, 0.1, 2.0, 1.0);
	tmpDoor = new PivotDoor(3.5, 1.0, -8.75, 3.5, 1.0, -8.25, 1.0, 2.0, tmpBox, materials[WOOD_DOOR], textures[WOOD_DOOR], 1.0, 2.0);
	objects.push_back(tmpDoor);
	doors.push_back(tmpDoor);
	// door eastern corridor-room2 END

	// door eastern corridor-room2 START
	tmpBox = new BoundingBox(-3.5, 1.0, -8.75, 0.1, 2.0, 1.0);
	tmpDoor = new PivotDoor(-3.5, 1.0, -8.75, -3.5, 1.0, -9.25, 1.0, 2.0, tmpBox, materials[WOOD_DOOR], textures[WOOD_DOOR], 1.0, 2.0);
	objects.push_back(tmpDoor);
	doors.push_back(tmpDoor);
	// door eastern corridor-room2 END

	// the surveillance camera in room 2 START
	survCam = new SurvCamera(-3.3, 2.5, -6.95, 2.0, -1.0, -1.0, 0.0, 1.165, -7.3, 1.2, 180);
	// the surveillance camera in room 2 END

	// table and chairs for surveillance camera in room 2 START
	tmpBox = new BoundingBox(0.0, 0.6, -7.3, 3.0, 1.2, 0.9);
	objects.push_back(new Table(0.0, 0.0, -7.3, 3.0, 1.165, 0.9, tmpBox, materials[WALL], textures[WALL], 0.5, 0.5));
    tmpBox = new BoundingBox(2.0, 0.6, -7.3, 1.0, 1.2, 0.9);
    objects.push_back(new Table(2.0, 0.0, -7.3, 1.0, 0.5, 0.8, tmpBox, materials[WALL], textures[WALL], 0.5, 0.5));
    tmpBox = new BoundingBox(-2.0, 0.6, -7.3, 1.0, 1.2, 0.9);
    objects.push_back(new Table(-2.0, 0.0, -7.3, 1.0, 0.5, 0.8, tmpBox, materials[WALL], textures[WALL], 0.5, 0.5));
	// table and chairs for surveillance camera in room 2 END

	// Done building walls and stuff for room 2




	// Start building walls and stuff for room 3

	// floor START
	tmpBox = new BoundingBox(-8.75, 0, 0, 4.0, 0.1, 7.0, 3);
	tmpWall = new Wall(-8.75, 0, 0, tmpBox, materials[FLOOR], 4.0, 7.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// floor END

	// roof START
	tmpBox = new BoundingBox(-8.75, 2.7, 0, 4.0, 0.1, 7.0);
	tmpWall = new Wall(-8.75, 2.7, 0, tmpBox, materials[FLOOR], 4.0, 7.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);	
	// roof END

	// south wall START
	tmpBox = new BoundingBox(-8.75, 1.35, 3.5, 4.0, 2.7, 0.1);
	tmpWall = new Wall(-8.75, 1.35, 3.5, tmpBox, materials[WALL], 4.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(180, 0, 1, 0);
	objects.push_back(tmpWall);
	// south wall END

	// north wall START
	tmpBox = new BoundingBox(-10.125, 1.35, -3.5, 1.25, 2.7, 0.1);
	tmpWall = new Wall(-10.125, 1.35, -3.5, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-7.375, 1.35, -3.5, 1.25, 2.7, 0.1);
	tmpWall = new Wall(-7.375, 1.35, -3.5, tmpBox, materials[WALL], 1.25, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-8.75, 2.35, -3.5, 1.5, 0.7, 0.1);
	tmpWall = new Wall(-8.75, 2.35, -3.5, tmpBox, materials[WALL], 1.5, 0.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-8.125 , 1.0, -3.5, 0.25, 2.0, 0.1);
	tmpWall = new Wall(-8.125, 1.0, -3.5, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-9.375, 1.0, -3.5, 0.25, 2.0, 0.1);
	tmpWall = new Wall(-9.375, 1.0, -3.5, tmpBox, materials[WALL], 0.25, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[WALL], textures[WALL], 0.5, 0.3);
	objects.push_back(tmpWall);
	// north wall END

	// west wall START
	tmpBox = new BoundingBox(-10.75, 1.0, 2.0, 0.1, 2.0, 3.0);
	tmpWall = new Wall(-10.75, 1.0, 2.0, tmpBox, materials[WALL], 3.01, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-10.75, 1.0, -2.0, 0.1, 2.0, 3.0);
	tmpWall = new Wall(-10.75, 1.0, -2.0, tmpBox, materials[WALL], 3.01, 2.0, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-10.75, 2.35, 0.0, 0.1, 0.7, 7.0);
	tmpWall = new Wall(-10.75, 2.35, 0.0, tmpBox, materials[WALL], 7.0, 0.71, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// west wall END

	// east wall START
	tmpBox = new BoundingBox(-6.75, 0.575, 0.0, 0.1, 1.15, 7.0);
	tmpWall = new Wall(-6.75, 0.575, 0.0, tmpBox, materials[WALL], 7.0, 1.15, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-6.75, 1.6, 1.975, 0.1, 0.9, 3.05);
	tmpWall = new Wall(-6.75, 1.6, 1.975, tmpBox, materials[WALL], 3.05, 0.9, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-6.75, 1.6, -1.975, 0.1, 0.9, 3.05);
	tmpWall = new Wall(-6.75, 1.6, -1.975, tmpBox, materials[WALL], 3.05, 0.9, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);
	objects.push_back(tmpWall);

	tmpBox = new BoundingBox(-6.75, 2.375, 0.0, 0.1, 0.65, 7.0);
	tmpWall = new Wall(-6.75, 2.375, 0.0, tmpBox, materials[WALL], 7.0, 0.65, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);
	objects.push_back(tmpWall);
	// east wall END


	// door room3-western corridor START
	tmpBox = new BoundingBox(-8.75, 1.0, -3.5, 1.0, 2.0, 0.1);
	tmpDoor = new PivotDoor(-8.75, 1.0, -3.5, -8.25, 1.0, -3.5, 1.0, 2.0, tmpBox, materials[WOOD_DOOR], textures[WOOD_DOOR], 1.0, 2.0);
	objects.push_back(tmpDoor);
	doors.push_back(tmpDoor);
	// door room3-western corridor END

	
	// sliding door from room 3 to freedom START
	tmpBox = new BoundingBox(-10.751, 1.0, 0.0, 0.1, 2.0, 1.0);
	lockedDoor = new SlideDoor(-10.751, 1.0, 0.0, 1.0, 2.0, tmpBox, materials[METAL_DOOR], textures[METAL_DOOR], 1.0, 2.0, true);
	lockedDoor->rotate(90, 0, 1, 0);
	objects.push_back(lockedDoor);
	// sliding door from room 3 to freedom END

	// window for room 3 START
	windows.push_back(new Window(-6.75, 1.6, 0.0, 0.9, 0.9, NULL, materials[WINDOW], textures[WINDOW]));
	windows[windows.size()-1]->rotate(270, 0, 1, 0);
	// window for room 3 END

	// door for window in room 3 START
	westWindowDoor = new PivotDoor(-6.747, 1.6, 0.0, -6.747, 1.6, -0.45, 0.9, 0.9, NULL, materials[WOOD_DOOR], textures[WOOD_DOOR], -0.9, 0.9, true);
	westWindowDoor->rotate(180, 0, 1, 0);
	objects.push_back(westWindowDoor);
	doors.push_back(westWindowDoor);
	// door for window in room 3 END

	// Done building walls and stuff for room 3



	// Start building walls and stuff for eastern corridor
	
	// south floor START
	tmpBox = new BoundingBox(8.75, 0.0, -6.5, 1.5, 0.1, 6.0, 4);
	tmpWall = new Wall(8.75, 0.0, -6.5, tmpBox, materials[FLOOR], 1.5, 6.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// south floor END


	// south roof START
	tmpBox = new BoundingBox(8.75, 2.7, -6.5, 1.5, 0.1, 6.0);
	tmpWall = new Wall(8.75, 2.7, -6.5, tmpBox, materials[FLOOR], 1.5, 6.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// south roof END

	// south-western wall START
	tmpBox = new BoundingBox(8.0, 1.35, -5.75, 0.1, 2.7, 4.5);
	tmpWall = new Wall(8.0, 1.35, -5.75, tmpBox, materials[WALL], 4.5, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// south-western wall END

	// south-eastern wall START
	tmpBox = new BoundingBox(9.5, 1.35, -6.5, 0.1, 2.7, 6.0);
	tmpWall = new Wall(9.5, 1.35, -6.5, tmpBox, materials[WALL], 6.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);
	objects.push_back(tmpWall);
	// south-eastern wall STOP


	// west floor START
	tmpBox = new BoundingBox(5.75, 0.0, -8.75, 4.5, 0.1, 1.5, 4);
	tmpWall = new Wall(5.75, 0.0, -8.75, tmpBox, materials[FLOOR], 4.5, 1.5, textures[FLOOR], 0.5, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// west floor END

	// west roof START
	tmpBox = new BoundingBox(5.75, 2.7, -8.75, 4.5, 0.1, 1.5);
	tmpWall = new Wall(5.75, 2.7, -8.75, tmpBox, materials[FLOOR], 4.5, 1.5, textures[FLOOR], 0.5, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// west roof END

	// north-north wall START
	tmpBox = new BoundingBox(6.5, 1.35, -9.5, 6.0, 2.7, 0.1);
	tmpWall = new Wall(6.5, 1.35, -9.5, tmpBox, materials[WALL], 6.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);
	// north-north wall END

	// north-south wall START
	tmpBox = new BoundingBox(5.75, 1.35, -8.0, 4.5, 2.7, 0.1);
	tmpWall = new Wall(5.75, 1.35, -8.0, tmpBox, materials[WALL], 4.5, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(180, 0, 1, 0);
	objects.push_back(tmpWall);
	// north-south wall END

	// Done building walls and stuff for eastern corridor




	// Start building walls and stuff for western corridor
	
	// south floor START
	tmpBox = new BoundingBox(-8.75, 0.0, -6.5, 1.5, 0.1, 6.0, 5);
	tmpWall = new Wall(-8.75, 0.0, -6.5, tmpBox, materials[FLOOR], 1.5, 6.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// south floor END


	// south roof START
	tmpBox = new BoundingBox(-8.75, 2.7, -6.5, 1.5, 0.1, 6.0);
	tmpWall = new Wall(-8.75, 2.7, -6.5, tmpBox, materials[FLOOR], 1.5, 6.0, textures[FLOOR], 0.5, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// south roof END

	// south-western wall START
	tmpBox = new BoundingBox(-8.0, 1.35, -5.75, 0.1, 2.7, 4.5);
	tmpWall = new Wall(-8.0, 1.35, -5.75, tmpBox, materials[WALL], 4.5, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(-90, 0, 1, 0);
	objects.push_back(tmpWall);
	// south-western wall END

	// south-eastern wall START
	tmpBox = new BoundingBox(-9.5, 1.35, -6.5, 0.1, 2.7, 6.0);
	tmpWall = new Wall(-9.5, 1.35, -6.5, tmpBox, materials[WALL], 6.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// south-eastern wall STOP


	// east floor START
	tmpBox = new BoundingBox(-5.75, 0.0, -8.75, 4.5, 0.1, 1.5, 5);
	tmpWall = new Wall(-5.75, 0.0, -8.75, tmpBox, materials[FLOOR], 4.5, 1.5, textures[FLOOR], 0.5, 0.3);
	tmpWall->setBackside(materials[FLOOR], 0, 0.5, 0.3);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// west floor END

	// east roof START
	tmpBox = new BoundingBox(-5.75, 2.7, -8.75, 4.5, 0.1, 1.5);
	tmpWall = new Wall(-5.75, 2.7, -8.75, tmpBox, materials[FLOOR], 4.5, 1.5, textures[FLOOR], 0.5, 0.3);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// east roof END

	// north-north wall START
	tmpBox = new BoundingBox(-6.5, 1.35, -9.5, 6.0, 2.7, 0.1);
	tmpWall = new Wall(-6.5, 1.35, -9.5, tmpBox, materials[WALL], 6.0, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	objects.push_back(tmpWall);
	// north-north wall END

	// north-south wall START
	tmpBox = new BoundingBox(-5.75, 1.35, -8.0, 4.5, 2.7, 0.1);
	tmpWall = new Wall(-5.75, 1.35, -8.0, tmpBox, materials[WALL], 4.5, 2.7, textures[WALL], 0.5, 0.3);
	tmpWall->setBackside(materials[OUTER_WALL], textures[OUTER_WALL], 0.5, 0.3);
	tmpWall->rotate(180, 0, 1, 0);
	objects.push_back(tmpWall);
	// north-south wall END

	// Done building walls and stuff for western corridor



	// The final, white corridor START

	// floor START
	tmpWall = new Wall(-15.75, 0.0, 0.0, NULL, materials[FINAL_CORRIDOR], 9.98, 2.01, 0, 0.0, 0.0);
	tmpWall->rotate(-90, 1, 0, 0);
	objects.push_back(tmpWall);
	// floor END
	
	// roof START
	tmpWall = new Wall(-15.75, 2.7, 0.0, NULL, materials[FINAL_CORRIDOR], 9.98, 2.01, 0, 0.0, 0.0);
	tmpWall->rotate(90, 1, 0, 0);
	objects.push_back(tmpWall);
	// roof END

	// north wall START
	tmpWall = new Wall(-15.75, 1.35, -1.0, NULL, materials[FINAL_CORRIDOR], 9.98, 2.71, 0, 0.0, 0.0);
	//tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// north wall END

	// south wall START
	tmpWall = new Wall(-15.75, 1.35, 1.0, NULL, materials[FINAL_CORRIDOR], 9.98, 2.71, 0, 0.0, 0.0);
	tmpWall->rotate(180, 0, 1, 0);
	objects.push_back(tmpWall);
	// south wall END

	// west wall START
	tmpWall = new Wall(-20.73, 1.35, 0.0, NULL, materials[FINAL_CORRIDOR], 2.01, 2.71, 0, 0.0, 0.0);
	tmpWall->rotate(90, 0, 1, 0);
	objects.push_back(tmpWall);
	// west wall END
	
	// THE CAKE.. which is just a texture... bah! START
	objects.push_back(new TexturedSurface(-17.0, 1.35, 0.0, 1.5, 1.5, materials[CAKE], textures[CAKE]));
	objects[objects.size()-1]->rotate(90, 0, 1, 0);
	// THE CAKE END

	// The final, white corridor END
}