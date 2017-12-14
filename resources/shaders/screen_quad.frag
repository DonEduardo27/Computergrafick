#version 150 core

in vec2 pass_texcoord;
flat in int effectMode;

uniform sampler2D texFramebuffer;

out vec4 outColor;

const float blurSizeH = 1.0 / 300.0;
const float blurSizeV = 1.0 / 200.0;

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
      for (int x = -2; x <= 2; x++){
          for (int y = -2; y <= 2; y++){
              sum += texture(texFramebuffer, vec2(pass_texcoord.x + x * blurSizeH, pass_texcoord.y + y * blurSizeV)) / 30.0;
          }
      }
      outColor = sum;
    }
}
