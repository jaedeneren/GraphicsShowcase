R"(
#version 330 core

layout (location = 0) out vec4 fragColor;

uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_camPos; 
uniform vec2 u_res;
uniform float iTime; // Used for orbiting math

// Sphere intersection: returns distance t
float intersectSphere(vec3 ro, vec3 rd, vec3 center, float radius) {
    vec3 oc = ro - center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;
    float h = b * b - c;
    if (h < 0.0) return 1e30; 
    h = sqrt(h);
    float t = -b - h;
    return (t > 0.0) ? t : 1e30;
}

//  Diffuse shading based on light position
vec3 computeShading(vec3 hitPos, vec3 normal, vec3 lightPos, vec3 baseColor) {
    vec3 lightDir = normalize(lightPos - hitPos);
    float diff = max(dot(normal, lightDir), 0.1); // 0.1 ambient floor
    return baseColor * diff;
}

void main() {
    // Generate Rays
    vec2 uv = (gl_FragCoord.xy / u_res) * 2.0 - 1.0;
    vec4 rayClip = vec4(uv, -1.0, 1.0);
    vec4 rayEye  = inverse(u_proj) * rayClip;
    rayEye = vec4(rayEye.xy, -1.0, 0.0);
    vec3 rd = normalize((inverse(u_view) * rayEye).xyz);
    vec3 ro = u_camPos;

    // Positions
    vec3 earthPos = vec3(0.0, 0.0, 0.0);
    
    // Sun Orbits Earth
    float sunDist = 10.0;
    vec3 sunPos = vec3(sunDist * cos(iTime * 0.5), 0.0, sunDist * sin(iTime * 0.5));
    
    // Moon Orbits Earth 
    float moonDist = 5.0;
    vec3 moonPos = vec3(moonDist * cos(iTime * 0.8), 0.5 * sin(iTime * 0.8), moonDist * sin(iTime * 0.8));

    // Intersections
    float tEarth = intersectSphere(ro, rd, earthPos, 2.0);
    float tSun   = intersectSphere(ro, rd, sunPos, 0.8);
    float tMoon  = intersectSphere(ro, rd, moonPos, 0.5);

    // Find Closest 
    float t = 1e30;
    int id = 0; // 1: Earth, 2: Sun, 3: Moon
    if (tEarth < t) { t = tEarth; id = 1; }
    if (tSun < t)   { t = tSun;   id = 2; }
    if (tMoon < t)  { t = tMoon;  id = 3; }

    if (id == 0) {
        fragColor = vec4(0.01, 0.01, 0.02, 1.0); // Space background
        return;
    }

    vec3 hitPos = ro + rd * t;
    vec3 color = vec3(0.0);

    if (id == 1) { // Earth Shading 
        vec3 normal = normalize(hitPos - earthPos);
        color = computeShading(hitPos, normal, sunPos, vec3(0.1, 0.3, 0.8));
    } 
    else if (id == 2) { // Sun (Emissive light center) [cite: 145]
        color = vec3(1.0, 0.9, 0.3);
    } 
    else if (id == 3) { // Moon Shading 
        vec3 normal = normalize(hitPos - moonPos);
        color = computeShading(hitPos, normal, sunPos, vec3(0.7, 0.7, 0.7));
    }

    fragColor = vec4(color, 1.0);
}
)"