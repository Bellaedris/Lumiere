#version 460

in vec3 normal;
in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D albedo;

void main() {
    vec3 lightDir = normalize(vec3(0, -1, -1));
    float cosTheta = max(.0f, dot(normal, -lightDir));
    vec3 color = cosTheta * texture(albedo, texcoord).xyz;

    FragColor = vec4(color, 1.0);
}