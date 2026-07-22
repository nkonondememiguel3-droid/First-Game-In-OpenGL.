#version 410 core

out vec4 frag_colour;
// the uniform variable are global the the entire shader program.
// so the time variable here is the same as the one in the vertex shader.
uniform float time;

in vec3 position;

void main()
{
    // float wild = sin( time ) * 0.5 + 0.5;
    // frag_colour.rga = vec3( 1.0 );
    // frag_colour.g = wild;

//     float r = sin(position.x * 4.0 + time) * 0.5 + 0.5;
//     float g = sin(position.y * 4.0 + time * 1.5) * 0.5 + 0.5;
//     float b = cos((position.x + position.y) * 4.0 - time) * 0.5 + 0.5;
//
//     frag_colour = vec4( r, g, b, 1.0 );

//    float dist = length(position.xy);
//    float pulse = sin(dist * 20.0 - time * 5.0) * 0.5 + 0.5;
//    vec3 neon_color = vec3(0.0, 0.8, 1.0) / (dist * 2.0 + 0.1);
//
//    frag_colour = vec4(neon_color * pulse, 1.0);

     float scanline = sin(position.y * 50.0 + time * 10.0);
     float intensity = scanline > 0.0 ? 1.0 : 0.3;
     vec3 amber = vec3(1.0, 0.7, 0.0) * intensity;

     frag_colour = vec4(amber, 1.0);

    //frag_colour = vec4( position, 1.0 );
}
