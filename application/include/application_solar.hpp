#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "texture_loader.hpp"
#include <glm/glm.hpp>

#include <memory>
#include <vector>

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void loadPlanets();

  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  // does the Matrix multiplication
  void upload_planet_transforms(planet const& Planet) const;

  void du_wirst_sehen_stars() const;

  void do_Rings(planet const& Planet) const;

  void draw_skybox() const;

  void draw_screen_quad() const;

  void loadTextures();

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void initializeStars();
  void initializeRings();
  void initializeTextures();
  void initializeFramebuffer();
  void initializeGodRayFramebuffer();
  void initializeSkyBox();
  void initializeScreenQuad();
  void updateView();

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  model_object ring_object;
  model_object skybox_object;
  model_object screen_quad_object;
  texture_object skybox_tex_obj, framebuffer_tex_obj, god_ray_framebuffer_tex_obj;
  framebuffer_object framebuffer_obj;
  framebuffer_object god_ray_framebuffer_obj;
  renderbuffer_object depthrenderbuffer_obj;
  renderbuffer_object god_ray_depthrenderbuffer_obj;
  particle_position_object part_pos_obj;
  particle_billbaord_object part_bill_obj;
  particle_color_object part_color_obj;
  std::vector<texture_object> tex_objects;
  std::vector<texture_object> tex_norm_objects;

  std::vector<std::shared_ptr<planet>> planet_container;
  std::vector<float> star_container;
  std::vector<texture> texture_container;
  std::vector<texture> normal_container;
  std::vector<texture> skybox_container;

  // is allowed to be changed
  mutable glm::fmat4 sun_mod_mat;
  mutable glm::fmat4 cam_pro_mat;
  mutable glm::fmat4 cam_view_mat;
  glm::vec4 effectMode;
  int shaderMode;
  bool rays;

};

#endif
