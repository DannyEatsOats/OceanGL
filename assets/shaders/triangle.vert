#version 330 core

layout(location = 0) in vec3 aPos;

out vec4 color;
out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

struct Wave {
    vec2 direction;  // normalized 2D direction
    float amplitude; // vertical amplitude
    float frequency; // wave number k
    float phase;     // angular speed
};

#define WAVE_COUNT 10

Wave waves[WAVE_COUNT] = Wave[](
    Wave(vec2(1.0, 0.3), 0.14, 1.1, 1.5),
    Wave(vec2(1.0, -0.3), 0.12, 1.1, 2.0),
    Wave(vec2(-1.0, 0.4), 0.10, 1.2, 2.2),
    Wave(vec2(-0.3, 1.0), 0.08, 1.4, 2.5),
    Wave(vec2(0.8, 0.3), 0.075, 1.5, 2.6),
    Wave(vec2(0.2, -0.7), 0.05, 2.5, 3.0),
    Wave(vec2(0.2, 0.4), 0.06, 3.5, 4.0),
    Wave(vec2(0.1, 0.5), 0.04, 5.0, 6.0),
    Wave(vec2(0.8, 0.8), 0.01, 6.8, 25.0),
    Wave(vec2(0.2, 0.7), 0.01, 8.4, 15.0)
);

float waveHeight(Wave w, vec2 pos) {
    float d = dot(pos, normalize(w.direction));
    return w.amplitude * exp(sin(d * w.frequency + time * w.phase) );
}

float waveDx(Wave w, vec2 pos) {
    vec2 dir = normalize(w.direction);
    float d = dot(pos, dir);
    float theta = d * w.frequency + time * w.phase;
    return cos(theta) * exp(sin(theta) ) * w.frequency * dir.x * w.amplitude;
}

float waveDz(Wave w, vec2 pos) {
    vec2 dir = normalize(w.direction);
    float d = dot(pos, dir);
    float theta = d * w.frequency + time * w.phase;
    return cos(theta) * exp(sin(theta) ) * w.frequency * dir.y * w.amplitude;
}

void main() {
    vec3 pos = aPos;

    vec2 domain = pos.xz;
    vec2 warp = vec2(0.0);

    float dx = 0.0;
    float dz = 0.0;

    float ampMod = 1.0;
    float freqMod = 1.0;

    for (int i = 0; i < WAVE_COUNT; ++i) {
        vec2 warped = domain + warp;

        Wave w = waves[i];
        w.amplitude *= ampMod;
        w.frequency *= freqMod;

        pos.y += waveHeight(w, warped);

        float ddx = waveDx(w, warped);
        float ddz = waveDz(w, warped);

        dx += ddx;
        dz += ddz;

        warp += vec2(ddx, ddz) * 0.35;

        ampMod *= 0.80;
        freqMod *= 1.22;
    }

    vec3 tangentX = vec3(1.0, dx, 0.0);
    vec3 tangentZ = vec3(0.0, dz, 1.0);

    normal = normalize(cross(tangentZ, tangentX));
    normal = normalize(transpose(inverse(mat3(model))) * normal);

    fragPos = vec3(model * vec4(pos, 1.0));
    color = vec4(0.2, 0.22, 0.27, 1.0);
    color.xyz *= 2.0;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}
