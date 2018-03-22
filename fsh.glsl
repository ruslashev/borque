#version 120

uniform vec2 mouse;
uniform vec2 resolution;
uniform float time;

const int MAX_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

float sphere(vec3 p, float s) {
  return length(p) - s;
}

float scene(vec3 p) {
  return sphere(p, 1.0);
}

vec3 ray_direction(float vfov, vec2 screen, vec2 frag_coord) {
  vec2 xy = frag_coord - screen / 2.0;
  float z = screen.y / tan(radians(vfov) / 2.0);
  return normalize(vec3(xy, -z));
}

float shortestDistanceToSurface(vec3 eye, vec3 dir, float start, float end) {
  float depth = start;
  for (int i = 0; i < MAX_STEPS; i++) {
    float dist = scene(eye + depth * dir);
    if (dist < EPSILON) {
      return depth;
    }
    depth += dist;
    if (depth >= end) {
      return end;
    }
  }
  return end;
}

void main() {
  vec3 dir = ray_direction(45.0, resolution, gl_FragCoord.xy);
  vec3 eye = vec3(0.0, 0.0, 5.0);
  float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);

  if (dist > MAX_DIST - EPSILON) {
    // no hit
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    return;
  }

  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

