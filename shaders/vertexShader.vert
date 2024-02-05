#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform ivec2 maxCoords;

void main() {
    if (aPos.x == 1.0 || aPos.y == 1.0 || aPos.x == -1.0 || aPos.y == -1.0) {
        vec2 coords = aPos.xy * mat2(
            cos(90.0), -sin(90.0),
            sin(90.0), cos(90.0)
        );
        gl_Position = vec4(coords, aPos.z, 1.0);
    } else {
        gl_Position = vec4(aPos.x + 0.05, aPos.y + 0.05, aPos.z, 1.0);
    }
    ourColor = aColor;
    TexCoord = aTexCoord;
}

/*

uniform float angleInput;

void main()
{
    float angle = angleInput;

    mat3 rotationX = mat3(
        1, 0, 0,
        0, cos(angle), -sin(angle),
        0, sin(angle), cos(angle)
    );

    mat3 rotationY = mat3(
        cos(angle), 0, sin(angle),
        0, 1, 0,
        -sin(angle), 0, cos(angle)
    );

    mat3 rotationZ = mat3(
        cos(angle), -sin(angle), 0,
        sin(angle), cos(angle), 0,
        0, 0, 1
    );

    vec3 rotatedCoords = aPos * rotationX * rotationY * rotationZ;
    gl_Position = vec4(rotatedCoords, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}

*/