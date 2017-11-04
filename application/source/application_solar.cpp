#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <sstream>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
{
  loadPlanets();
  initializeGeometry();
  initializeStars();
  initializeRings();
  initializeShaderPrograms();

}

//loads .txt with planet discription (Coputergrafick/planets.txt)
void ApplicationSolar::loadPlanets() {
    std::string line;
    std::cout << "Es wird ../planets.txt genutzt" << "\n";
    std::ifstream myfile("../planets.txt");

    if (myfile.is_open()){
      while ( getline (myfile,line) ){
        std::stringstream ss;
        ss<<line;
        float rot,size,dist,speed;
        int surrround;

        ss>>size;
        ss>>speed;
        ss>>dist;
        ss>>rot;
        ss>>surrround;

        std::shared_ptr<planet> new_planet = std::make_shared<planet>(planet{float(rot) ,float(size), float(speed), float(dist), surrround});
        planet_container.push_back(new_planet);
      }
      myfile.close();
    }
    else {
      std::cout << "----------------------------------------"<< "\n";
      std::cout << "ERROR: Unable to load file"<< "\n";
      std::cout << "Loading default objkts: Sun, Earth, Moon"<< "\n";
      planet sonne{0.5, 8, 0, 0, 0}, earth{1, 1, 1.4f, 17, 0}, moon{1, 0.2f, 17, 1.5f, 1};
      auto sun = std::make_shared<planet> (sonne);
      auto erde = std::make_shared<planet> (earth);
      auto mond = std::make_shared<planet> (moon);
      planet_container.insert(std::end(planet_container), {sun, erde, mond});
    }
}

//draws all planets in planet container
void ApplicationSolar::render() const {

  for (auto i : planet_container ){
    upload_planet_transforms(*i);
    do_Rings();
  }
  du_wirst_sehen_stars();
}

//gives planets model and normal matrix
void ApplicationSolar::upload_planet_transforms(planet const& Planet) const {

  glUseProgram(m_shaders.at("planet").handle);
  glm::fmat4 model_matrix;

  model_matrix = glm::rotate(glm::fmat4{}, (float(glfwGetTime())/2) * Planet.m_rot, glm::fvec3{0.0f, 1.0f, 0.0f});

  //if planet is not orbiting around the sun (origin)
  if(Planet.m_surroundet != 0) {
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f * planet_container[Planet.m_surroundet]->m_dis_org});
    model_matrix = glm::rotate(model_matrix, (float(glfwGetTime())/2) * Planet.m_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  }

  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f * Planet.m_dis_org});
  model_matrix = glm::rotate(model_matrix, float(glfwGetTime()/2) * Planet.m_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  glm::vec3 p_size {Planet.m_size,Planet.m_size,Planet.m_size};
  model_matrix = glm::scale(model_matrix, p_size);


  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

void ApplicationSolar::du_wirst_sehen_stars() const
{
  glUseProgram(m_shaders.at("stars").handle);
  glBindVertexArray(star_object.vertex_AO);

  // draw bound vertex array using bound shader
  glPointSize(4);
  glDrawArrays(star_object.draw_mode, 0, (int)star_container.size());
}
void ApplicationSolar::do_Rings() const
{
//  glUseProgram(m_shaders.at("ring").handle);

  glm::fmat4 model_matrix = {1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            0,0,0,1};

  //glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
  //glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  //glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));


  glBindVertexArray(ring_object.vertex_AO);

  glDrawArrays(ring_object.draw_mode, 0, 400);
}


void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("stars").handle);
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ViewMatrix"),  1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("ring").handle);
  glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ViewMatrix"),  1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("stars").handle);
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ProjectionMatrix"),  1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("ring").handle);
  glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ProjectionMatrix"),  1, GL_FALSE, glm::value_ptr(m_view_projection));

}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  updateView();
  updateProjection();
}

// handle key input W A S D
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_W) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -1.0f});
    updateView();
  }
  else if (key == GLFW_KEY_S) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 1.0f});
    updateView();
  }
  else if (key == GLFW_KEY_A) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_D) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
    updateView();
  }
}

//handle delta mouse movement input, Y-axis not rotating but up/down movement
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
  m_view_transform = glm::rotate(m_view_transform, float(pos_x)/100, glm::fvec3{0.0f, -1.0f, 0.0f});
  m_view_transform = glm::translate(m_view_transform,glm::fvec3{0.0f, float(pos_y)/100, 0.0f});
  // upload matrix to gpu
  updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});

  m_shaders.emplace("stars", shader_program{m_resource_path + "shaders/star.vert",
                                           m_resource_path + "shaders/star.frag"});

  m_shaders.emplace("ring", shader_program{m_resource_path + "shaders/ring.vert",
                                           m_resource_path + "shaders/ring.frag"});

  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("stars").u_locs["ViewMatrix"] = -1;
  m_shaders.at("stars").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("ring").u_locs["ViewMatrix"] = -1;
  m_shaders.at("ring").u_locs["ModelMatrix"] = -1; //ring need to be transformed
  m_shaders.at("ring").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);
  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);
  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);
   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);
  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

// load stars
void ApplicationSolar::initializeStars(){

  for(int i=0; i<6*10000; i++){
    star_container.push_back(static_cast <float>(rand()%200)-100);
  }

  glGenVertexArrays(1, &star_object.vertex_AO);
  glBindVertexArray(star_object.vertex_AO);
  glGenBuffers(1, &star_object.vertex_BO);
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * star_container.size(), star_container.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 3);
  glGenBuffers(1, &star_object.element_BO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
  star_object.draw_mode = GL_POINTS;
  star_object.num_elements = GLsizei(star_container.size());
}
void ApplicationSolar::initializeRings()
{
    std::vector<glm::vec4> ringPoints;

    for (unsigned i = 0; i < 400; i++)
    {
      float tempx,tempy;
      tempx = float(sin( ((2.0 * M_PI) / 100.0) * float(i) ) );
      tempy = float(cos( ((2.0 * M_PI) / 100.0) * float(i) ) );

      ringPoints.push_back(glm::vec4(tempx,0,tempy,1));
    }

    glGenVertexArrays(1, &ring_object.vertex_AO);
    glBindVertexArray(ring_object.vertex_AO);
    glGenBuffers(1, &ring_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, ring_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ringPoints.size(), ringPoints.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 3);
    glGenBuffers(1, &ring_object.element_BO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ring_object.element_BO);
    ring_object.draw_mode = GL_LINE_LOOP;
    ring_object.num_elements = GLsizei(ringPoints.size());
}

// deconstructor
ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}
