varying vec2 v_texCoord;
uniform sampler2D qt_Texture0;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
uniform int pixFmt;
void main(void)
{
    //    gl_FragColor = texture2D(qt_Texture0, v_texCoord);
    vec3 yuv;
    vec3 rgb;
    //YUV420P
    if (pixFmt == 0) {
        yuv.x = texture2D(tex_y, v_texCoord).r;
        yuv.y = texture2D(tex_u, v_texCoord).r - 0.5;
        yuv.z = texture2D(tex_v, v_texCoord).r - 0.5;
        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;
    } else if (pixFmt == 1) {
    //YUV444P
//        yuv.x = texture2D(tex_y, v_texCoord).r;
//        yuv.y = texture2D(tex_u, v_texCoord).r - 0.5;
//        yuv.z = texture2D(tex_v, v_texCoord).r - 0.5;

//        rgb.x = smoothstep(0, 1, yuv.x + 1.402 *yuv.z);
//        rgb.y = smoothstep(0, 1, yuv.x - 0.34414 * yuv.y - 0.71414 * yuv.z);
//        rgb.z = smoothstep(0, 1, yuv.x + 1.772 * yuv.y);
        yuv.x = texture2D(tex_y, v_texCoord).r;
        yuv.y = texture2D(tex_u, v_texCoord).r - 0.5;
        yuv.z = texture2D(tex_v, v_texCoord).r - 0.5;
        rgb = mat3( 1,       1,         1,
                    0,       -0.39465,  2.03211,
                    1.13983, -0.58060,  0) * yuv;

    }
    gl_FragColor = vec4(rgb, 1);

}
