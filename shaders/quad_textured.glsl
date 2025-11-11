@vs vs
in vec3 position;
in vec2 texcoord0;
out vec2 uv;

void main() {
    gl_Position = vec4(position, 1.0);
    uv = texcoord0;
}
@end

@fs fs
in vec2 uv;
out vec4 FragColor;
layout(binding=0) uniform texture2D tex;
layout(binding=0) uniform sampler samp;

void main() {
    FragColor = texture(sampler2D(tex, samp), uv);
}
@end

@program quad_textured vs fs