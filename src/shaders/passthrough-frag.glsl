#version 120

//ray. This is specialized for overlays
varying vec2 frag_ray;

// original texture
uniform sampler2D texture;

// distortion maps
uniform sampler2D distortion;

// controls
uniform float gamma;
uniform float brightness;
uniform float use_color;

// debayer
float width = 672.0;
float height = 600.0;
float rscale = 1.23;
float gscale = 1.04;
float bscale = 0.73;

float corr_ir_g = 0.2;
float corr_ir_rb = 0.2;
float corr_r_b = 0.1;
float corr_g_rb = 0.1;

vec2 r_offset = vec2(0, -0.5);
vec2 g_offset = vec2(0.5, -0.5);
vec2 b_offset = vec2(0.5, 0);

void main(void) {
  vec2 texCoord = texture2D(distortion, frag_ray).xy;
  gl_FragColor.rgb = brightness*vec3(pow(texture2D(texture, texCoord).r, gamma));
  gl_FragColor.a = 1.0;
}
