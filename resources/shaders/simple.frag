#version 150

in  vec3 pass_Normal;
// in  vec3 pass_Color;
in  vec3 vert_pos;
in  vec3 vert_pos_world;
in  vec2 pass_TexCoord;
in  vec3 pass_Tangent;

uniform sampler2D ColorTex;
uniform sampler2D NormalTex;

out vec4 out_Color;


const vec3 light_pos = vec3(0, 0, 0);
vec3 ambient_color = vec3(texture(ColorTex, pass_TexCoord)).xyz * 0.01;
vec3 diffuse_color = vec3(texture(ColorTex, pass_TexCoord)).xyz;
vec3 normal_color  = vec3(texture(NormalTex, pass_TexCoord)).rgb * 2.0f - 1.0f;
const vec3 spec_color = vec3 (1, 1, 1);
const float shininess = 30.0;
const float screenGamma = 2.2;

void main() {
    vec3 bi_tangent = cross(pass_Normal, pass_Tangent);
    mat3 tangent    = mat3(pass_Tangent, bi_tangent, pass_Normal);
    vec3 normal     = normalize(tangent * normal_color).rgb;
    vec3 light_dir  = normalize(  (vec4(light_pos - vert_pos_world, 0.0)).xyz);  //calculating direction light comes from(important for specular)
    vec3 view_dir   = normalize( -(vec4(vert_pos_world, 0.0)).xyz);              //calculating direction form where we look on (important for diffuse)

    float diffuse   = max(dot(light_dir, normal),0.0);
    float specular  = 0.0;

    if(diffuse > 0.0) {
      vec3 half_dir    = normalize(light_dir + view_dir);
      float spec_angle = max(dot(half_dir, normal), 0.0);
      specular = pow(spec_angle, shininess);
    }
    vec3 color_linear     = ambient_color + diffuse * diffuse_color + specular * spec_color;
    vec3 color_gamma_corr = pow(color_linear, vec3(1.0/screenGamma));

    out_Color = vec4(color_gamma_corr, 1.0);
}
