#version 330 core

in vec4 color;
in vec3 fragPos;
in vec3 normal;

out vec4 FragColor;

uniform vec3 camPos;

void main() {
    // Base light / sun parameters
    vec3 sunColor = vec3(1.0, 0.5, 0.3);
    vec3 sunPos = vec3(0.0, 6.0, -50.0);

    // Base water color (blue)
    vec3 waterColor = color.xyz;

    // Ambient and diffuse stay blue
    float ambientStrength = 0.20;
    vec3 ambient = ambientStrength * waterColor;

    vec3 lightDir = normalize(sunPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * waterColor * 0.4;

    // Specular is orange
    vec3 viewDir = normalize(camPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);

    float horizonFade = pow(max(dot(normal, vec3(0.5,1,0)), 0.0), 0.7);
    vec3 specular = spec * sunColor * horizonFade;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, color.w);
}
