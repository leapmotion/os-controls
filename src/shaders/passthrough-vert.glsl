#version 120
attribute vec3 position;
uniform mat4 projection_times_model_view_matrix;
uniform vec2 ray_scale;
uniform vec2 ray_offset;
varying vec2 frag_ray;

void main(void) {
  gl_Position = projection_times_model_view_matrix * vec4(position, 1.0);
  vec2 ray;
  if( position.z != 0) {
    ray = vec2(-position.x/position.z, -position.y/position.z);
  }
  else {
    ray = vec2(position.x, position.y);
  }
  frag_ray = ray*ray_scale + ray_offset;
}
