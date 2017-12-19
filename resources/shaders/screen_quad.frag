#version 150 core

in vec2 pass_texcoord;
in vec4 effectMode;

uniform sampler2D texFramebuffer;

out vec4 outColor;


const float pixelsize_x = 1.0 / 1024.0;
const float pixelsize_y = 1.0 / 768.0;
vec2  texCoord          = pass_texcoord;
const mat3  kernel      = mat3(0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847);

void main() {
    // from here: https://open.gl/framebuffers
    outColor = texture(texFramebuffer, pass_texcoord);

    if(effectMode.y == 1){
      texCoord.x = 1 - texCoord.x;
      outColor = texture(texFramebuffer, texCoord);
    }

    if(effectMode.z == 1){
      texCoord.y = 1 - texCoord.y;
      outColor = texture(texFramebuffer, texCoord);
    }

    if(effectMode.w == 1){
      vec4 sum = vec4(0.0);

      int j = 0;
      for (int x = -1; x <= 1; x++){
        int k = 0;
          for (int y = -1; y <= 1; y++){
              sum += texture(texFramebuffer, vec2(texCoord.x + x * pixelsize_x, texCoord.y + y * pixelsize_y)) * kernel[j][k];
              ++k;
          }
          ++j;
      }
      outColor = sum;
    }

    if(effectMode.x == 1){
      float avg = 0.2126 * outColor.r + 0.7152 * outColor.g + 0.0722 * outColor.b;
      outColor = vec4(avg, avg, avg, 1.0);
    }
}
