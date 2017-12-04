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
  std::cout << "loadPlanets()                 complete" << std::endl;
  loadTextures();
  std::cout << "loadTextures()                complete" << std::endl;
  initializeGeometry();
  std::cout << "initializeGeometry()          complete" << std::endl;
  initializeStars();
  std::cout << "initializeStars()             complete" << std::endl;
  initializeRings();
  std::cout << "initializeRings()             complete" << std::endl;
  initializeTextures();
  std::cout << "initializeTextures()          complete" << std::endl;
  initializeSkyBox();
  std::cout << "initializeSkybox()            complete" << std::endl;
  initializeShaderPrograms();
  std::cout << "initializeShaderPrograms()    complete" << std::endl;
  shaderMode = 1;
  std::cout << "initialization                complete" << std::endl;
  std::cout << "--------------------------------------\n" << std::endl;
}

//draws all planets in planet container, rings and stars
void ApplicationSolar::render() const {

  for (auto i : planet_container ){
    upload_planet_transforms(*i);
    do_Rings(*i);
  }
  du_wirst_sehen_stars();
  // draw_skybox();
}

//gives planets model and normal matrix
void ApplicationSolar::upload_planet_transforms(planet const& Planet) const {

  if(Planet.m_dis_org == 0){

    glUseProgram(m_shaders.at("sun").handle);

    glm::fmat4 model_matrix;
    // std::cout << Planet.m_color.x << ", " << Planet.m_color.y << ", " << Planet.m_color.z << std::endl;
    model_matrix = glm::rotate(glm::fmat4{}, (float(glfwGetTime())/2) * Planet.m_rot, glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f * Planet.m_dis_org});
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime()/2) * Planet.m_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    glm::vec3 p_size {Planet.m_size,Planet.m_size,Planet.m_size};
    model_matrix = glm::scale(model_matrix, p_size);
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    // glm::vec3 colors {1,1,1};
    float r = Planet.m_color.x;
    float g = Planet.m_color.y;
    float b = Planet.m_color.z;

    glUniform3f(m_shaders.at("sun").u_locs.at("ColorVec3"), r, g, b);
    // glUniform3fv(m_shaders.at("planet").u_locs.at("ColorVec3"), 1, glm::value_ptr(Planet.m_color));
    glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glUniform1i(m_shaders.at("sun").u_locs.at("ShaderMode"), shaderMode);
    glUniform1i(m_shaders.at("sun").u_locs.at("ColorTex"), 0);
    // bind the VAO to draw
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_objects[Planet.m_k].handle);
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  }

  else{

    glm::fmat4 model_matrix;
    // std::cout << Planet.m_color.x << ", " << Planet.m_color.y << ", " << Planet.m_color.z << std::endl;

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

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    //glm::vec3 colors {1,1,1};
    float r = Planet.m_color.x;
    float g = Planet.m_color.y;
    float b = Planet.m_color.z;

    if(shaderMode == 1){
      glUseProgram(m_shaders.at("planet").handle);
      glUniform3f(m_shaders.at("planet").u_locs.at("ColorVec3"), r, g, b);
      // glUniform3fv(m_shaders.at("planet").u_locs.at("ColorVec3"), 1, glm::value_ptr(Planet.m_color));
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));
      glBindVertexArray(planet_object.vertex_AO);

      glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"), 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tex_objects[Planet.m_k].handle);
    }
    else if(shaderMode == 2){

      glUseProgram(m_shaders.at("toon").handle);
      glUniform3f(m_shaders.at("toon").u_locs.at("ColorVec3"), r, g, b);
      //glUniform3fv(m_shaders.at("planet").u_locs.at("ColorVec3"), 1, glm::value_ptr(Planet.m_color));
      glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
      glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));
      glBindVertexArray(planet_object.vertex_AO);
   }

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  }

}

// do stars
void ApplicationSolar::du_wirst_sehen_stars() const {
  glUseProgram(m_shaders.at("stars").handle);
  glBindVertexArray(star_object.vertex_AO);

  // draw bound vertex array using bound shader
  glPointSize(2.0);
  glDrawArrays(star_object.draw_mode, 0, (int)star_container.size());
}

// assigne ModelMatrix to Orbit rings
void ApplicationSolar::do_Rings(planet const& Planet) const {
  glUseProgram(m_shaders.at("ring").handle);

  glm::fmat4 model_matrix;
  if(Planet.m_surroundet != 0) {
    model_matrix = glm::rotate(model_matrix, (float(glfwGetTime())/2) * Planet.m_rot, glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f * planet_container[Planet.m_surroundet]->m_dis_org});
  }
  glm::vec3 scale_dir{Planet.m_dis_org,0,Planet.m_dis_org};
  model_matrix = glm::scale(model_matrix, scale_dir);

  glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
  glBindVertexArray(ring_object.vertex_AO);
  glDrawArrays(ring_object.draw_mode, 0, 361); //genauer hinschaun
}

// draw Skybox
void ApplicationSolar::draw_skybox() const {
  glDepthMask(GL_FALSE);
  glUseProgram(m_shaders.at("skybox").handle);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex_obj.handle);
  glBindVertexArray(skybox_object.vertex_AO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthMask(GL_TRUE);
}

// update Viewnmatrix
void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("toon").handle);
  glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("sun").handle);
  glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("stars").handle);
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ViewMatrix"),  1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("ring").handle);
  glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ViewMatrix"),  1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"),  1, GL_FALSE, glm::value_ptr(view_matrix));
}

// update Projectionmatrix
void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("toon").handle);
  glUniformMatrix4fv(m_shaders.at("toon").u_locs.at("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("sun").handle);
  glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("stars").handle);
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ProjectionMatrix"),  1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("ring").handle);
  glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ProjectionMatrix"),  1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"),  1, GL_FALSE, glm::value_ptr(m_view_projection));
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
  else if (key == GLFW_KEY_1) {
    shaderMode = 1;
    //updateView();
  }
  else if (key == GLFW_KEY_2) {
    shaderMode = 2;
    //updateView();
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

  m_shaders.emplace("toon", shader_program{m_resource_path + "shaders/toon.vert",
                                          m_resource_path + "shaders/toon.frag"});

  m_shaders.emplace("sun", shader_program{m_resource_path + "shaders/sun.vert",
                                           m_resource_path + "shaders/sun.frag"});

  m_shaders.emplace("stars", shader_program{m_resource_path + "shaders/star.vert",
                                           m_resource_path + "shaders/star.frag"});

  m_shaders.emplace("ring", shader_program{m_resource_path + "shaders/ring.vert",
                                           m_resource_path + "shaders/ring.frag"});

  m_shaders.emplace("skybox", shader_program{m_resource_path + "shaders/skybox.vert",
                                            m_resource_path + "shaders/skybox.frag"});

  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["ColorVec3"] = -1;
  m_shaders.at("planet").u_locs["ColorTex"] = -1;

  m_shaders.at("toon").u_locs["NormalMatrix"] = -1;
  m_shaders.at("toon").u_locs["ModelMatrix"] = -1;
  m_shaders.at("toon").u_locs["ViewMatrix"] = -1;
  m_shaders.at("toon").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("toon").u_locs["ColorVec3"] = -1;

  m_shaders.at("sun").u_locs["NormalMatrix"] = -1;
  m_shaders.at("sun").u_locs["ModelMatrix"] = -1;
  m_shaders.at("sun").u_locs["ViewMatrix"] = -1;
  m_shaders.at("sun").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("sun").u_locs["ColorVec3"] = -1;
  m_shaders.at("sun").u_locs["ShaderMode"] = -1;
  m_shaders.at("sun").u_locs["ColorTex"] = -1;

  m_shaders.at("stars").u_locs["ViewMatrix"] = -1;
  m_shaders.at("stars").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("ring").u_locs["ViewMatrix"] = -1;
  m_shaders.at("ring").u_locs["ModelMatrix"] = -1; //1 ring need to be transformed
  m_shaders.at("ring").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);
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
  // activate thirt attribute on gpu
  glEnableVertexAttribArray(2);
  // third attribute is TextCoord
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);
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

  /*
    // cube for skybox
    float points[] = {
      -10.0f,  10.0f, -10.0f,
      -10.0f, -10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
       10.0f,  10.0f, -10.0f,
      -10.0f,  10.0f, -10.0f,

      -10.0f, -10.0f,  10.0f,
      -10.0f, -10.0f, -10.0f,
      -10.0f,  10.0f, -10.0f,
      -10.0f,  10.0f, -10.0f,
      -10.0f,  10.0f,  10.0f,
      -10.0f, -10.0f,  10.0f,

       10.0f, -10.0f, -10.0f,
       10.0f, -10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,

      -10.0f, -10.0f,  10.0f,
      -10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f, -10.0f,  10.0f,
      -10.0f, -10.0f,  10.0f,

      -10.0f,  10.0f, -10.0f,
       10.0f,  10.0f, -10.0f,
       10.0f,  10.0f,  10.0f,
       10.0f,  10.0f,  10.0f,
      -10.0f,  10.0f,  10.0f,
      -10.0f,  10.0f, -10.0f,

      -10.0f, -10.0f, -10.0f,
      -10.0f, -10.0f,  10.0f,
       10.0f, -10.0f, -10.0f,
       10.0f, -10.0f, -10.0f,
      -10.0f, -10.0f,  10.0f,
       10.0f, -10.0f,  10.0f
     };
    glGenVertexArrays(1, &skybox_object.vertex_AO);
    glBindVertexArray(planet_object.vertex_AO);
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_AO);
    glGenBuffers(1, &skybox_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  */
}

// load stars
void ApplicationSolar::initializeStars() {

  for(int i=0; i<6*10000; i++){
    star_container.push_back((float)(rand()%500)-250);
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

// load Orbit Rings
void ApplicationSolar::initializeRings() {
    std::vector<float> ringPoints;
    for (unsigned i = 0; i < 361; i++){
      ringPoints.push_back((float)cos((i * M_PI)/180));
      ringPoints.push_back(0.0);
      ringPoints.push_back((float)-sin((i * M_PI)/180));
      ringPoints.push_back(1.0);
    }

    glGenVertexArrays(1, &ring_object.vertex_AO);
    glBindVertexArray(ring_object.vertex_AO);
    glGenBuffers(1, &ring_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, ring_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ringPoints.size(), ringPoints.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glGenBuffers(1, &ring_object.element_BO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ring_object.element_BO);
    ring_object.draw_mode = GL_LINE_LOOP;
    ring_object.num_elements = GLsizei(ringPoints.size()/3);
}

// load Textures and assign to apropriat container
void ApplicationSolar::initializeTextures(){
  for(unsigned int i = 0; i < texture_container.size(); ++i){
      texture_object tex;
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &tex.handle);
      glBindTexture(GL_TEXTURE_2D, tex.handle);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, texture_container[i].m_pixelData.channels, (GLsizei)texture_container[i].m_pixelData.width,
                  (GLsizei)texture_container[i].m_pixelData.height, 0, texture_container[i].m_pixelData.channels,
                  texture_container[i].m_pixelData.channel_type, texture_container[i].m_pixelData.ptr());

      tex_objects.push_back(tex);
    }
}

// load Skybox
void ApplicationSolar::initializeSkyBox(){
      glActiveTexture(GL_TEXTURE0);
      glGenTextures(1, &skybox_tex_obj.handle);
      glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex_obj.handle);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      for(unsigned int idx = 0; idx < skybox_container.size(); ++idx){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, skybox_container[idx].m_pixelData.channels, (GLsizei)skybox_container[idx].m_pixelData.width,
                    (GLsizei)skybox_container[idx].m_pixelData.height, 0, skybox_container[idx].m_pixelData.channels,
                    skybox_container[idx].m_pixelData.channel_type, skybox_container[idx].m_pixelData.ptr());
      }

      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
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
        glm::vec3 color;
        int k;

        ss>>size;
        ss>>speed;
        ss>>dist;
        ss>>rot;
        ss>>surrround;
        ss>>color.x;
        ss>>color.y;
        ss>>color.z;
        ss>>k;

        std::shared_ptr<planet> new_planet = std::make_shared<planet>(planet{float(rot) ,float(size), float(speed), float(dist), int(surrround), color, k});
        planet_container.push_back(new_planet);
      }
      myfile.close();
    }
    else {
      std::cout << "----------------------------------------"<< "\n";
      std::cout << "ERROR: Unable to load file"<< "\n";
      std::cout << "Loading default objkts: Sun, Earth, Moon"<< "\n";
      glm::vec3 color{255, 255, 255};
      planet sonne{0.5, 8, 0, 0, 0, color, 0}, earth{1, 1, 1.4f, 17, 0, color, 0}, moon{1, 0.2f, 17, 1.5f, 1, color, 0};
      auto sun = std::make_shared<planet> (sonne);
      auto erde = std::make_shared<planet> (earth);
      auto mond = std::make_shared<planet> (moon);
      planet_container.insert(std::end(planet_container), {sun, erde, mond});
    }
}

// Textures to container
void ApplicationSolar::loadTextures() {
  texture earth     ("earth"    , texture_loader::file( m_resource_path + "textures/earth2k.png"));
  texture sun       ("sun"      , texture_loader::file( m_resource_path + "textures/sun2k.png"));
  texture jupiter   ("jupiter"  , texture_loader::file( m_resource_path + "textures/jupiter2k.png"));
  texture mars      ("mars"     , texture_loader::file( m_resource_path + "textures/mars2k.png"));
  texture mercury   ("mercury"  , texture_loader::file( m_resource_path + "textures/mercury2k.png"));
  texture neptun    ("neptun"   , texture_loader::file( m_resource_path + "textures/neptune2k.png"));
  texture saturn    ("saturn"   , texture_loader::file( m_resource_path + "textures/saturn2k.png"));
  texture uranus    ("uranus"   , texture_loader::file( m_resource_path + "textures/uranus2k.png"));
  texture venus     ("venus"    , texture_loader::file( m_resource_path + "textures/venus2k.png"));
  texture moon      ("moon"     , texture_loader::file( m_resource_path + "textures/moon2k.png"));

  texture right_box ("skybox_r"   , texture_loader::file( m_resource_path + "textures/right.png"));
  texture left_box  ("skybox_l"   , texture_loader::file( m_resource_path + "textures/left.png"));
  texture top_box   ("skybox_u"   , texture_loader::file( m_resource_path + "textures/up.png"));
  texture bottom_box("skybox_d"   , texture_loader::file( m_resource_path + "textures/down.png"));
  texture back_box  ("skybox_b"   , texture_loader::file( m_resource_path + "textures/back.png"));
  texture front_box ("skybox_f"   , texture_loader::file( m_resource_path + "textures/front.png"));

  texture_container.push_back(sun);
  texture_container.push_back(mercury);
  texture_container.push_back(venus);
  texture_container.push_back(earth);
  texture_container.push_back(moon);
  texture_container.push_back(mars);
  texture_container.push_back(jupiter);
  texture_container.push_back(saturn);
  texture_container.push_back(uranus);
  texture_container.push_back(neptun);

  skybox_container.push_back(right_box);
  skybox_container.push_back(left_box);
  skybox_container.push_back(top_box);
  skybox_container.push_back(bottom_box);
  skybox_container.push_back(back_box);
  skybox_container.push_back(front_box);
}

// deconstructor
ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  glDeleteBuffers(1, &ring_object.vertex_BO);
  glDeleteBuffers(1, &ring_object.element_BO);
  glDeleteVertexArrays(1, &ring_object.vertex_AO);

  glDeleteTextures(1, &skybox_tex_obj.handle);
  for (unsigned int i = 0; i < tex_objects.size(); ++i){
    glDeleteTextures(1, &tex_objects[i].handle);
  }




}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}
