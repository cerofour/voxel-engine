#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
layout (location = 2) in float aVoxelId;

uniform mat4 MVP;
uniform mat4 modelMatrix;

out vec3 normal;
out vec3 fragPos;
//out vec2 texCoords;
out vec3 vertexColor;

vec3 hash31(float p) {
    vec3 p3 = fract(vec3(p * 21.2) * vec3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.xxy + p3.yzz) * p3.zyx) + 0.05;
}

void main() {
    gl_Position = MVP * vec4(aPosition, 1.0f);
    fragPos = vec3(modelMatrix * vec4(aPosition, 1.0f));
    normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
	//texCoords = aTexCoords;
    vertexColor = hash31(aVoxelId);
}
