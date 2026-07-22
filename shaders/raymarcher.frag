#version 410 core

out vec4 frag_colour;

// Remplacement des entrées globales de Shadertoy par vos Uniforms C
uniform vec3 iResolution;
uniform float iTime;

#define h21(p) fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123)
#define path(z) vec2(sin(z * .15) * 2.4, cos(z * .25) * 1.7)
#define openCave(t)  (tanh(cos(t * .05) * 12. + 10.) * .5 + .5)
#define ballMovez(t)  tanh(cos(t * .5) * 6. + 4.) * 3. + 7.
#define ballMoveXY(t) tanh(sin(t) * 5. - 3.) * 4.
#define tri(x) abs(x - floor(x) - .5)
#define trap(x, pf) (tri(x - pf * .125) + tri(x + pf * .125)) * .5
#define t iTime

bool G;
float FAR = 40., ID, svObjID;
vec2 e = vec2(0, 1);
vec3 lp;

// Récupération de la position interpolée venant du Vertex Shader plein écran
in vec3 position;

float waterSurf(vec3 p) {
    p += vec3(0, 0, t * 4.);
    return dot(sin(p + sin(p.yzx * 2.) * cos(p.y)), vec3(.2)) + 1.;
}

float map(vec3 p) {
    float sf = dot(trap(p * .384 + trap(p.yzx * .192, .75), .75), vec3(.666));
    sf = (1. - sf) * (cos(p.z * .4) + 1.02);

    p.xy -= path(p.z);

    float tunel = 1.5 - length(p.xy * vec2(.5, .7071)) * openCave(t) + sf,
          ball = length(p - lp) - 1.;

    tunel = min(tunel, ball);

    if(G){
        ID = 1.;
        return tunel;
    }

    p.y += waterSurf(p);
    ID = step(tunel, p.y);
    return min(tunel, p.y);
}

vec3 getNormal(vec3 p, inout float edge) {
    vec2 e_norm = vec2(6. / iResolution.y, 0);

    edge = abs(map(p + e_norm.xyy) + map(p - e_norm.xyy) - map(p) * 2.)
           + abs(map(p + e_norm.yxy) + map(p - e_norm.yxy) - map(p) * 2.)
           + abs(map(p + e_norm.yyx) + map(p - e_norm.yyx) - map(p) * 2.);

    edge = smoothstep(0., 1., sqrt(edge / e_norm.x * 2.));

    e_norm = vec2(.01, 0);
    return normalize(
               vec3(
                   map(p + e_norm.xyy) - map(p - e_norm.xyy),
                   map(p + e_norm.yxy) - map(p - e_norm.yxy),
                   map(p + e_norm.yyx) - map(p - e_norm.yyx)
               )
           );
}

vec3 doColor(vec3 sp, vec3 rd, vec3 sn, vec3 lp, float edge, float t_val) {
    vec3 col = vec3(0.0);

    if(t_val < 40.) {
        vec3 ld = lp - sp;
        float lDist = max(length(ld), .001);
        ld /= lDist;

        float atten = 1.5 / (1. + lDist * .125 + lDist * lDist * .025);

        float diff = max(dot(sn, ld), 0.);
        diff = pow(diff, 4.) * .66  +  pow(diff, 8.) * .34;

        float spec = pow(max(dot(reflect(-ld, sn), -rd), .0), 32.);

        vec3 objCol = vec3(.6, .8, 1);
        if(svObjID > .5)
            objCol = vec3(1, .55, .35);

        col = (objCol * (diff + .7 + vec3(1, .6, .2) * spec * 2.));
        col *= 1. - edge * .9;
        col *= atten;
    }

    return col;
}

float calculateAO(vec3 pos, vec3 nor) {
    float sca = 2., occ = 0.0;

    for(int i = 0; i < 4; i ++) {
        float hr = .01 + float(i) * .5 / 4.;
        float dd = map(nor * hr + pos);
        occ +=(hr - dd) * sca;
        sca *= .6;
    }
    return clamp(1. - occ, .0, 1.);
}

float march(vec3 ro, vec3 rd) {
    float t_val = 0.0, d = 0.0, i = 0.0;
    while(i < 96.) {
        i++;
        d = map(ro + rd * t_val);
        if(abs(d) < .001 * (t_val * .5 + 1.) || t_val > FAR) break;
        t_val += d * .8;
    }
    return t_val;
}

float noise(vec3 q) {
    vec2 p_noise = vec2(length(q.zy), q.x);
    vec2 i = floor(p_noise), f = fract(p_noise), c = vec2(1, 0);
    f *= f * (3. - 2. * f);
    return mix(mix(h21(i + c.yy), h21(i + c.xy), f.x), mix(h21(i + c.yx), h21(i + c.xx), f.x), f.y);
}

float bumpFunc(vec3 p) {
    vec2 e_bump = vec2(t, 0);
    return (noise(p * 5. + e_bump.yxy * -12.) + noise(p * 4. + e_bump.xyy * -4.) + noise(p * 6. + e_bump.xyy * 7.)) / 2.;
}

vec3 bumpMap(vec3 p, vec3 n, float bumpfactor) {
    vec2 e_bump = vec2(.002, 0);
    vec3 grad = vec3(bumpFunc(p - e_bump.xyy), bumpFunc(p - e_bump.yxy), bumpFunc(p - e_bump.yyx)) - bumpFunc(p);
    return normalize((1. - n * dot(n, n)) * grad * bumpfactor / e_bump.x + n);
}

void main() {
    // Utilisation des coordonnées normalisées de la fenêtre basées sur votre Vertex plein écran
    vec2 u = position.xy * (iResolution.xy / iResolution.y) * 0.5;

    vec3 ro = vec3(0, 0, t);
    vec3 lk = ro + vec3(0, 0, .25);
    lp = ro + vec3(0, .25, ballMovez(t));

    ro.xy += path(ro.z);
    lk.xy += path(lk.z);
    lp.xy += path(lp.z + ballMoveXY(t)) - vec2(0, 1);

    vec3 forward = normalize(lk - ro);
    vec3 right = normalize(vec3(forward.z, 0., - forward.x));
    vec3 up = cross(forward, right);
    vec3 rd = normalize(forward + (u.x * right + u.y * up));

    float t_val = march(ro, rd);

    svObjID = ID;
    float oSvObjID = svObjID;
    vec3 sp = ro + rd * t_val;

    float edge;
    vec3 sn = getNormal(sp, edge);

    if(oSvObjID < .5)
        sn = bumpMap(sp, sn, .04);

    vec3 col = doColor(sp, rd, sn, lp, edge, t_val);

    float fog = t_val / FAR + .2;
    float ao = calculateAO(sp, sn);

    vec3 refr = refract(rd, sn, 1. / 1.33);
    float fr = clamp(1. + dot(rd, sn), 0., 1.);

    svObjID = ID;

    if(oSvObjID < .5) {
        G = true;
        t_val = march(sp, refr);
        svObjID = ID;
        vec3 refSp = sp + refr * t_val;
        sn = getNormal(refSp, edge);

        vec3 refrColor = doColor(refSp, refr, sn, lp, edge, t_val);
        col = col * .2 + mix(refrColor, col, pow(fr, 5.) * .4 + .2);
    }
    else {
        col = pow(col, vec3(.5)) * (ao + .5) * .5 - fog * .3;
    }

    vec3 final_color = pow(smoothstep(.05, .4, col * fog), vec3(.45));
    frag_colour = vec4(final_color, 1.0);
}
