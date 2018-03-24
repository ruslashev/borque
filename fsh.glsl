#version 120

uniform vec2 mouse;
uniform vec2 resolution;
uniform float time;

const int MAX_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPS = 0.0001;

float sphere(vec3 p, float s) {
  return length(p) - s;
}

float ubox(vec3 p, vec3 b) {
  return length(max(abs(p) - b, 0.0));
}

float rect(vec3 p, vec3 b) {
  vec3 d = abs(p) - b;
  float inside_distance = min(max(d.x, max(d.y, d.z)), 0.0);
  float outside_distance = length(max(d, 0.0));
  return inside_distance + outside_distance;
}

float scene(vec3 p) {
  return rect(p, vec3(1.0, 1.0, 1.0));
}

vec3 ray_direction(float vfov, vec2 screen_size, vec2 frag_coord) {
  vec2 xy = frag_coord - screen_size / 2.0;
  float z = screen_size.y / tan(radians(vfov) / 2.0);
  return normalize(vec3(xy, -z));
}

float distance_to_scene(vec3 eye, vec3 dir, float start, float end) {
  float depth = start;
  for (int i = 0; i < MAX_STEPS; i++) {
    float dist = scene(eye + depth * dir);
    if (dist < EPS) {
      return depth;
    }
    depth += dist;
    if (depth >= end) {
      return end;
    }
  }
  return end;
}

vec3 estimate_normal(vec3 p) {
  return normalize(vec3(
    scene(vec3(p.x + EPS, p.y, p.z)) - scene(vec3(p.x - EPS, p.y, p.z)),
    scene(vec3(p.x, p.y + EPS, p.z)) - scene(vec3(p.x, p.y - EPS, p.z)),
    scene(vec3(p.x, p.y, p.z + EPS)) - scene(vec3(p.x, p.y, p.z - EPS))
  ));
}

vec3 phong_contrib_for_light(vec3 Kd, vec3 Ks, float alpha, vec3 p, vec3 eye,
    vec3 pos, vec3 intensity) {
  vec3 N = estimate_normal(p);
  vec3 L = normalize(pos - p);
  vec3 V = normalize(eye - p);
  vec3 R = normalize(reflect(-L, N));
  float dotLN = dot(L, N);
  float dotRV = dot(R, V);

  if (dotLN < 0.0) {
    // Light not visible from this point on the surface
    return vec3(0.0, 0.0, 0.0);
  }

  if (dotRV < 0.0) {
    // Light reflection in opposite direction as viewer, apply only diffuse
    // component
    return intensity * (Kd * dotLN);
  }

  return intensity * (Kd * dotLN + Ks * pow(dotRV, alpha));
}

vec3 phong_illuminate(vec3 Ka, vec3 Kd, vec3 Ks, float alpha, vec3 p, vec3 eye) {
  const vec3 ambient_light = 0.5 * vec3(1.0, 1.0, 1.0);
  vec3 color = ambient_light * Ka;

  vec3 light1_pos = vec3(4.0 * sin(time), 2.0, 4.0 * cos(time));
  vec3 light1_intensity = vec3(0.4, 0.4, 0.4);
  color += phong_contrib_for_light(Kd, Ks, alpha, p, eye, light1_pos,
      light1_intensity);

  vec3 light2_pos = vec3(2.0 * sin(0.37 * time), 2.0 * cos(0.37 * time), 2.0);
  vec3 light2_intensity = vec3(0.4, 0.4, 0.4);
  color += phong_contrib_for_light(Kd, Ks, alpha, p, eye, light2_pos,
      light2_intensity);

  return color;
}

float sinn(float x) {
  return (sin(x) + 1.0) / 2.0;
}

void main() {
  vec3 dir = ray_direction(45.0, resolution, gl_FragCoord.xy);
  vec3 eye = vec3(0.0, 0.0, 5.0);
  float dist = distance_to_scene(eye, dir, MIN_DIST, MAX_DIST);

  if (dist > MAX_DIST - EPS) {
    // no hit
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    return;
  }

  vec3 p = eye + dist * dir;

  vec3 Ka = vec3(0.2, 0.2, 0.2);
  vec3 Kd = vec3(0.7, 0.2, 0.2);
  vec3 Ks = vec3(1.0, 1.0, 1.0);
  float shininess = 10.0;
  vec3 color = phong_illuminate(Ka, Kd, Ks, shininess, p, eye);

  gl_FragColor = vec4(color, 1.0);
}

