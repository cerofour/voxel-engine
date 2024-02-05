#version 330 core
 
in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;
out vec4 FragColor; // output a color to the fragment shader

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D texture0;

void main()
{
    vec4 texColor = texture(texture0, texCoords);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 objectColor = vec3(0.20, 0.88, 0.24);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosition - fragPos);
    float diffuse = max(dot(norm, lightDir), 0.0f);
    vec3 diffuseLight = diffuse * lightColor;

    float specularStrength = 1.0f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 128);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuseLight + specular) * texColor.xyz;

    FragColor = vec4(result, 1.0f);
}