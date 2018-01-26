#version 150 core

in vec2 pass_texcoord;
in vec4 effectMode;

uniform sampler2D texFramebuffer;
uniform sampler2D firstPass;
uniform vec4 light_pos;

out vec4 outColor;


const float pixelsize_x = 1.0 / 1024.0;
const float pixelsize_y = 1.0 / 768.0;
vec2  texCoord          = pass_texcoord;
const mat3  kernel      = mat3(0.077847, 0.123317, 0.077847, 0.123317, 0.195346, 0.123317, 0.077847, 0.123317, 0.077847);

// from here: http://fabiensanglard.net/lightScattering/
float exposure = 0.5;
float decay = 0.96875;
float density = 0.5;
float weight = 0.1;
const int NUM_SAMPLES = 100;

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

    if(true){
        // from here: http://fabiensanglard.net/lightScattering/
		vec2 textCoo = pass_texcoord;
        vec2 lightPos = light_pos.xy/2 + vec2(0.5, 0.5);
		vec2 deltaTexCoord = (textCoo - lightPos) / float(NUM_SAMPLES) * density;
		float illuminationDecay = 1.0;

		vec4 god_rays = texture2D(firstPass, textCoo);

        for(int i=0; i < NUM_SAMPLES ; i++){
             textCoo -= deltaTexCoord;
             vec4 sample = texture2D(firstPass, textCoo );
             sample *= illuminationDecay * weight;
             god_rays += sample;
             illuminationDecay *= decay;
         }
         outColor += god_rays * exposure;
    }
}
