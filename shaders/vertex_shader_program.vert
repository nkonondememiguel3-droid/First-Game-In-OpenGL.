#version 410 core

// vertex 3D volume(clip space) for our vertecis.
// in means here that our vertecis comes from the grahic hardware memory also called: vertex buffers.
in vec3 vertex_position;

// the current time. uniform is like 'const' in C.
// this time variable will come from our C program.
uniform float time;

void main()
{
    vec3 position = vertex_position;
    position.y += sin( time );

    gl_Position = vec4( position, 1.0 ); // we position our vertecises int the clip era.
}
