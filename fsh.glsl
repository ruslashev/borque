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

vec3 estimateNormal(vec3 p) {
  return normalize(vec3(
    scene(vec3(p.x + EPS, p.y, p.z)) - scene(vec3(p.x - EPS, p.y, p.z)),
    scene(vec3(p.x, p.y + EPS, p.z)) - scene(vec3(p.x, p.y - EPS, p.z)),
    scene(vec3(p.x, p.y, p.z + EPS)) - scene(vec3(p.x, p.y, p.z - EPS))
  ));
}

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
    vec3 lightPos, vec3 lightIntensity) {
  vec3 N = estimateNormal(p);
  vec3 L = normalize(lightPos - p);
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
    return lightIntensity * (k_d * dotLN);
  }
  return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
  const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
  vec3 color = ambientLight * k_a;

  vec3 light1Pos = vec3(4.0 * sin(time),
      2.0,
      4.0 * cos(time));
  vec3 light1Intensity = vec3(0.4, 0.4, 0.4);

  color += phongContribForLight(k_d, k_s, alpha, p, eye,
      light1Pos,
      light1Intensity);

  vec3 light2Pos = vec3(2.0 * sin(0.37 * time),
      2.0 * cos(0.37 * time),
      2.0);
  vec3 light2Intensity = vec3(0.4, 0.4, 0.4);

  color += phongContribForLight(k_d, k_s, alpha, p, eye,
      light2Pos,
      light2Intensity);
  return color;
}

float sinn(float x) {
  return ((sin(x) + 1.0) / 2.0);
}

void main() {
  vec3 dir = ray_direction(45.0, resolution, gl_FragCoord.xy);
  vec3 eye = vec3(0.0, 0.0, 5.0);
  float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);

  if (dist > MAX_DIST - EPS) {
    // no hit
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    return;
  }

  vec3 p = eye + dist * dir;

  vec3 K_a = vec3(0.2, 0.2, 0.2);
  vec3 K_d = vec3(0.7, 0.2, 0.2);
  vec3 K_s = vec3(1.0, 1.0, 1.0);
  float shininess = 10.0;

  vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, eye);

  gl_FragColor = vec4(color, 1.0);
}

