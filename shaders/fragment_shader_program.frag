#version 410 core

out vec4 frag_colour;
// the uniform variable are global the the entire shader program.
// so the time variable here is the same as the one in the vertex shader.
uniform float time;

in vec3 position;

void main()
{
    float wild = sin( time ) * 0.5 + 0.5;
    frag_colour.rga = vec3( 1.0 );
    frag_colour.g = wild;

    //frag_colour = vec4( position, 1.0 );
}
