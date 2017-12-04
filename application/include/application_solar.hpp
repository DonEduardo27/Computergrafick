#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "texture_loader.hpp"

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

  void loadTextures();

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void initializeStars();
  void initializeRings();
  void initializeTextures();
  void initializeSkyBox();
  void updateView();

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  model_object ring_object;
  texture_object skybox_tex_obj;
  std::vector<texture_object> tex_objects;
  std::vector<std::shared_ptr<planet>> planet_container;
  std::vector<float> star_container;
  std::vector<texture> texture_container;
  std::vector<texture> skybox_container;
  int shaderMode;
};

#endif
