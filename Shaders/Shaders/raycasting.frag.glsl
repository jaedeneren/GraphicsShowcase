R"(
#version 330 core

layout (location = 0) out vec4 fragColor;

uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_camPos; 
uniform vec2 u_res;

// intersection functions
float intersectPlane(vec3 ro, vec3 rd, vec3 normal, float dist) {
    float denom = dot(normal, rd);
    if (abs(denom) > 1e-6) {
        float t = (dist - dot(normal, ro)) / denom;
        return (t >= 0.0) ? t : 1e30;
    }
    return 1e30;
}

float intersectBox(vec3 ro, vec3 rd, vec3 boxMin, vec3 boxMax, out vec3 normal) {
    vec3 invDir = 1.0 / rd;
    vec3 t0 = (boxMin - ro) * invDir;
    vec3 t1 = (boxMax - ro) * invDir;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);
    
    float tNear = max(max(tmin.x, tmin.y), tmin.z);
    float tFar  = min(min(tmax.x, tmax.y), tmax.z);
    
    if (tNear <= tFar && tFar >= 0.0) {
        vec3 hit = ro + rd * tNear;
        vec3 center = (boxMin + boxMax) * 0.5;
        vec3 halfs = (boxMax - boxMin) * 0.5;
        vec3 v = (hit - center) / halfs;
        vec3 a = abs(v);
        if (a.x > a.y && a.x > a.z) normal = vec3(sign(v.x), 0.0, 0.0);
        else if (a.y > a.z) normal = vec3(0.0, sign(v.y), 0.0);
        else normal = vec3(0.0, 0.0, sign(v.z));
        return tNear;
    }
    return 1e30;
}

// Helper to get solid colors for reflection 
vec3 getShapeColor(int id) {
    if (id == 1) return vec3(0.6, 0.1, 0.1); // Red Wall
    if (id == 2) return vec3(0.1, 0.6, 0.1); // Green Wall
    if (id == 3) return vec3(0.1, 0.1, 0.6); // Blue Wall
    if (id == 4) return vec3(0.5, 0.5, 0.2); // Yellow Wall
    if (id == 6) return vec3(0.1, 0.1, 0.3); // Cube 1 Base
    if (id == 7) return vec3(0.3, 0.3, 0.1); // Cube 2 Base
    if (id == 8) return vec3(0.4, 0.4, 0.4); // Ceiling
    return vec3(0.05, 0.05, 0.1);           // Sky
}

void main() {
    vec2 uv = (gl_FragCoord.xy / u_res) * 2.0 - 1.0;
    vec4 rayClip = vec4(uv, -1.0, 1.0);
    vec4 rayEye  = inverse(u_proj) * rayClip;
    rayEye = vec4(rayEye.xy, -1.0, 0.0);

    vec3 rd = normalize((inverse(u_view) * rayEye).xyz);
    vec3 ro = u_camPos;

    // Define object bounds
    vec3 bMin1 = vec3(-4.0, 0.0, -10.0), bMax1 = vec3(-1.0, 3.0, -7.0);
    vec3 bMin2 = vec3(1.5, 0.0, -12.0),  bMax2 = vec3(4.5, 6.0, -9.0);

    // Calculate all intersections
    float dLeft   = intersectPlane(ro, rd, vec3( 1, 0, 0), -30.0);
    float dRight  = intersectPlane(ro, rd, vec3(-1, 0, 0), -30.0);
    float dFront  = intersectPlane(ro, rd, vec3( 0, 0, 1), -30.0);
    float dBack   = intersectPlane(ro, rd, vec3( 0, 0,-1), -30.0);
    float dFloor  = intersectPlane(ro, rd, vec3( 0, 1, 0),   0.0);
    float dCeiling = intersectPlane(ro, rd, vec3( 0, -1, 0), -8.0);

    vec3 n1, n2;
    float dCube1 = intersectBox(ro, rd, bMin1, bMax1, n1);
    float dCube2 = intersectBox(ro, rd, bMin2, bMax2, n2);

    // assign IDs based on closest intersection
    float t = 1e30;
    int id = 0;

    if (dLeft < t)  { t = dLeft;  id = 1; }
    if (dRight < t) { t = dRight; id = 2; }
    if (dFront < t) { t = dFront; id = 3; }
    if (dBack < t)  { t = dBack;  id = 4; }
    if (dFloor < t) { t = dFloor; id = 5; }
    if (dCeiling < t) { t = dCeiling; id = 8; }
    if (dCube1 < t) { t = dCube1; id = 6; }
    if (dCube2 < t) { t = dCube2; id = 7; }

    if (id == 0) {
        fragColor = vec4(0.05, 0.05, 0.1, 1.0);
        return;
    }

    vec3 hitPos = ro + rd * t;
    vec3 color = vec3(0.0);

    // 3. Visual Representations based on ID
    if (id == 1) color = vec3(0.6, 0.1, 0.1); 
    else if (id == 2) color = vec3(0.1, 0.6, 0.1); 
    else if (id == 3) color = vec3(0.1, 0.1, 0.6); 
    else if (id == 4) color = vec3(0.5, 0.5, 0.2); 
    else if (id == 5) {
        // floor with reflections
        color = vec3(0.15); // Solid Base Floor Color
        
        vec3 reflectRd = reflect(rd, vec3(0, 1, 0));
        vec3 reflectRo = hitPos + vec3(0, 0.01, 0); // Offset to prevent self-hit

        // Check intersections for reflected ray (Walls and Cubes)
        float rl = intersectPlane(reflectRo, reflectRd, vec3(1,0,0), -30.0);
        float rr = intersectPlane(reflectRo, reflectRd, vec3(-1,0,0), -30.0);
        float rf = intersectPlane(reflectRo, reflectRd, vec3(0,0,1), -30.0);
        float rb = intersectPlane(reflectRo, reflectRd, vec3(0,0,-1), -30.0);
        float rceil = intersectPlane(reflectRo, reflectRd, vec3(0,-1,0), -8.0);
        vec3 dummyN;
        float rc1 = intersectBox(reflectRo, reflectRd, bMin1, bMax1, dummyN);
        float rc2 = intersectBox(reflectRo, reflectRd, bMin2, bMax2, dummyN);

        float rt = 1e30; int rid = 0;
        if (rl < rt) { rt = rl; rid = 1; } if (rr < rt) { rt = rr; rid = 2; }
        if (rf < rt) { rt = rf; rid = 3; } if (rb < rt) { rt = rb; rid = 4; }
        if (rceil < rt) { rt = rceil; rid = 8; }
        if (rc1 < rt) { rt = rc1; rid = 6; } if (rc2 < rt) { rt = rc2; rid = 7; }

        if (rid != 0) {
            vec3 reflectCol = getShapeColor(rid);
            float fade = exp(-0.05 * rt); // Faded reflection based on distance
            color = mix(color, reflectCol, 0.25 * fade);
        }
    }
    else if (id == 6) {
        vec2 grid = (abs(n1.x) > 0.5) ? hitPos.zy : (abs(n1.y) > 0.5) ? hitPos.xz : hitPos.xy;
        float checker = mod(floor(grid.x * 1.5) + floor(grid.y * 1.5), 2.0);
        color = mix(vec3(0.1, 0.1, 0.3), vec3(0.8, 0.8, 1.0), checker);
    }
    else if (id == 7) {
        vec2 grid = (abs(n2.x) > 0.5) ? hitPos.zy : (abs(n2.y) > 0.5) ? hitPos.xz : hitPos.xy;
        float stripes = step(0.5, fract(grid.y * 2.0)); 
        color = mix(vec3(0.3, 0.3, 0.1), vec3(1.0, 1.0, 0.7), stripes);
    }
    else if (id == 8) {
        color = vec3(0.4, 0.4, 0.4); // Grey ceiling
    }

    // Distance lighting effect
    color *= exp(-0.03 * t);
    fragColor = vec4(color, 1.0);
}
)"
