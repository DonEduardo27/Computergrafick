#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 vert_pos;
in  vec3 vert_pos_cam;
in  vec3 vert_pos_world;
out vec4 out_Color;

const vec3 light_pos = vec3(0, 0, 0);
const vec3 ambient_color = vec3(0.1, 0, 0);
vec3 diffuse_color = pass_Color;
const vec3 spec_color = vec3 (1, 1, 1);

const float screenGamma = 2.2;

void main() {
  vec3 normal = normalize(pass_Normal); //Length of normal is 1

  vec3 light_dir = normalize(  (vec4(light_pos - vert_pos_world, 0.0)).xyz);  //calculating direction light comes from(important for specular)
  vec3 view_dir  = normalize( -(vec4(vert_pos_world, 0.0)).xyz);              //calculating direction form where we look on (important for diffuse)

  float diffuse = max(dot(light_dir, normal),0.0);
  float specular = 0.0;

  if(diffuse > 0.0){

    if(diffuse > 0.66){diffuse = 1;}
    else if (diffuse > 0.33){diffuse = 0.66;}
    else if (diffuse > 0.0){diffuse = 0.33;}

    const float shininess = 100.0;
    vec3 half_dir = normalize(light_dir + view_dir);
    float spec_angle = max(dot(half_dir, normal), 0.0);

    if(spec_angle > 0.995){spec_angle = 1;}
    else{spec_angle = 0;}

    specular = pow(spec_angle, shininess);
  }

  vec3 color_linear = ambient_color + diffuse * diffuse_color + specular * spec_color;
  vec3 color_gamma_corr = pow(color_linear, vec3(1.0/screenGamma));

  vec3 eye_dir = normalize( -vert_pos_cam);
  float view_angle = dot(eye_dir, normal);

  if(abs(view_angle) < 0.2) {
    color_gamma_corr = vec3 (1,1,1);
  }

  out_Color = vec4(color_gamma_corr, 1.0);
}
