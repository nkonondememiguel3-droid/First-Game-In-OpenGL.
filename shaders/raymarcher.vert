#version 410 core
in vec3 vertex_position;
out vec3 position;

void main() {
    position = vertex_position;
    gl_Position = vec4(vertex_position, 1.0);
}

