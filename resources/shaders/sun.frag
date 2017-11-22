#version 150

in vec3 pass_Normal;
in vec3 pass_Color;
in vec3 vert_pos_cam;
flat in int shaderMode;
out vec4 out_Color;

void main() {
  vec3 color = pass_Color;

  if(shaderMode == 2){
    vec3 normal = normalize(pass_Normal);
    vec3 eye_dir = normalize(- vert_pos_cam);
    float view_angle = dot(eye_dir, normal);

    if(abs(view_angle) < 0.5) {
      color = vec3 (1,0,0);
    }
  }

  out_Color = vec4(color, 1.0);
}
