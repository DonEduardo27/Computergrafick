#version 150 core

in vec2 pass_texcoord;
flat in int effectMode;

uniform sampler2D texFramebuffer;

out vec4 outColor;


const float pixelsize_x = 1.0 / 1024.0;
const float pixelsize_y = 1.0 / 768.0;
const mat3  kernel      = mat3(0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847);

void main() {
    // from here: https://open.gl/framebuffers
    outColor = texture(texFramebuffer, pass_texcoord);

    if(effectMode == 6){
      outColor = texture(texFramebuffer, pass_texcoord);
    }

    if(effectMode == 7){
      float avg = 0.2126 * outColor.r + 0.7152 * outColor.g + 0.0722 * outColor.b;
      outColor = vec4(avg, avg, avg, 1.0);
    }

    if(effectMode == 8){
      float x = 1 - pass_texcoord.x;
      outColor = texture(texFramebuffer, vec2(x, pass_texcoord.y));
    }

    if(effectMode == 9){
      float y = 1 - pass_texcoord.y;
      outColor = texture(texFramebuffer, vec2(pass_texcoord.x, y));
    }

    if(effectMode == 0){
      vec4 sum = vec4(0.0);

      int j = 0;
      for (int x = -1; x <= 1; x++){
        int k = 0;
          for (int y = -1; y <= 1; y++){
              sum += texture(texFramebuffer, vec2(pass_texcoord.x + x * pixelsize_x, pass_texcoord.y + y * pixelsize_y)) * kernel[j][k];
              ++k;
          }
          ++j;
      }
      outColor = sum;
    }
}
