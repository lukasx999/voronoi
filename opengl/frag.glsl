#version 410 core





uniform vec2 window_size;

#define POINTS_LEN 2
uniform vec2 points[POINTS_LEN];

out vec4 frag_color;





void main() {
    vec2 pos = gl_FragCoord.xy;

    bool flag = false;
    for (int i=0; i < POINTS_LEN; ++i) {
        if (pos == points[i]) {
            flag = true;
        }
    }

    frag_color = vec4(points[1], 1.0, 1.0);

    // if (flag) {
    //     frag_color = vec4(1.0, 1.0, 1.0, 1.0);
    // } else {
    //     frag_color = vec4(0.0, 0.0, 0.0, 1.0);
    // }

    // vec2 pos = gl_FragCoord.xy / window_size;
    // pos *= 10;
    // pos = fract(pos);
    // frag_color = vec4(pos, pos.y, 1.0);

}
