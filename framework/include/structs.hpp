#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <map>
#include <glbinding/gl/gl.h>
#include <glm/vec3.hpp>
#include <iostream>
// use gl definitions from glbinding
using namespace gl;

struct star {
  star():
    position{0.0, 0.0, 0.0},
    color{1.0,1.0,1.0}
    {
      position.x = float(rand());
      position.y = float(rand());
      position.z = float(rand());
    }
  glm::vec3 position {0.0, 0.0, 0.0};
  glm::vec3 color {1.0, 1.0, 1.0};
};

struct planet {
  //Default Constructor
  planet():
    m_rot{1.0},
    m_size{1.0},
    m_speed{1.0},
    m_dis_org{0.0},
    m_surroundet{0}
  {}

  //Constructor
  planet(float rot, float size, float speed, float dist, int sorroundet):
    m_rot{rot},
    m_size{size},
    m_speed{speed},
    m_dis_org{dist},
    m_surroundet{sorroundet}
  {}

  float m_rot;      //orbiting speed
  float m_size;     //diameter of a Planet
  float m_speed;    //self rotation
  float m_dis_org;  //Temperature on the Planet (distance from Centre)
  int m_surroundet; //orbit center as planet
};

// gpu representation of model
struct model_object {
  // vertex array object
  GLuint vertex_AO = 0;
  // vertex buffer object
  GLuint vertex_BO = 0;
  // index buffer object
  GLuint element_BO = 0;
  // primitive type to draw
  GLenum draw_mode = GL_NONE;
  // indices number, if EBO exists
  GLsizei num_elements = 0;
};

// gpu representation of texture
struct texture_object {
  // handle of texture object
  GLuint handle = 0;
  // binding point
  GLenum target = GL_NONE;
};

// shader handle and uniform storage
struct shader_program {
  shader_program(std::string const& vertex, std::string const& fragment)
   :vertex_path{vertex}
   ,fragment_path{fragment}
   ,handle{0}
   {}

  // path to shader source
  std::string vertex_path;
  std::string fragment_path;
  // object handle
  GLuint handle;
  // uniform locations mapped to name
  std::map<std::string, GLint> u_locs{};
};
#endif
