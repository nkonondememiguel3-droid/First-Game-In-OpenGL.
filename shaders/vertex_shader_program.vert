#version 410 core

// vertex 3D volume(clip space) for our vertecis.
// in means here that our vertecis comes from the grahic hardware memory also called: vertex buffers.
in vec3 vertex_position;

// the current time. uniform is like 'const' in C.
// this time variable will come from our C program.
uniform float time;

// here this out keyword tells GLSL that this variable is going to be send to the next shader stage in the pipeline.
out vec3 position;

void main()
{
    position = vertex_position;
    // position.y += sin( time );

    position.x += sin( time + position.x * 5.0 ) * 0.2;
    position.y += cos( time + position.y * 3.0 ) * 0.1;

    float echel = 1.3;
    gl_Position = vec4( position * echel, 1.0 ); // we position our vertecises int the clip era.
}
