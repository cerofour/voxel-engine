#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(aPos, 1.0);
	fragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
	texCoords = aTexCoord;
}