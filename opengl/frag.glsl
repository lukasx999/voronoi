#version 410 core





uniform vec2 window_size;

#define POINTS_LEN 6
uniform vec2 points[POINTS_LEN];

out vec4 frag_color;





void main() {
    vec2 pos = gl_FragCoord.xy / window_size;

    float circle_radius = 0.01;

    vec2 nearest_point = window_size + 1;
    bool matched = false;

    for (int i=0; i < POINTS_LEN; ++i) {
        vec2 p = points[i];

        float dist = distance(pos, p);
        if (dist < distance(pos, nearest_point))
            nearest_point = p;

        // if (dist <= circle_radius) {
        //     matched = true;
        //     frag_color = vec4(1.0, 1.0, 1.0, 1.0);
        // }

    }

    if (!matched)
        frag_color = vec4(nearest_point, 0.0, 1.0);


}
