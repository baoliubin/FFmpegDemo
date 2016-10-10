varying vec2 v_texCoord;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
uniform int pixFmt;
void main(void)
{
    vec3 yuv;
    vec3 rgb;
    if (pixFmt == 0) {
        //yuv420p
        float nx, ny;
        nx = v_texCoord.x;
        ny = v_texCoord.y;

        yuv.x = texture2D(tex_y, v_texCoord).r;
        yuv.x = 1.1643 *(yuv.x - 0.0625);
        yuv.y = texture2D(tex_u, vec2(nx/ 2.0, (ny)/2.0)).r - 0.5;
        yuv.z = texture2D(tex_v, vec2(nx/ 2.0, (ny)/2.0)).r - 0.5;

        rgb.x = clamp( yuv.x + 1.5958 *yuv.z, 0, 1);
        rgb.y = clamp( yuv.x - 0.39173 * yuv.y - 0.81290 * yuv.z, 0, 1);
        rgb.z = clamp( yuv.x + 2.017 * yuv.y, 0, 1);
//        yuv.x = texture2D(tex_y, v_texCoord).r;
//        yuv.y = texture2D(tex_u, vec2(nx/ 2.0, (ny)/2.0)).r - 0.5;
//        yuv.z = texture2D(tex_v, vec2(nx/ 2.0, (ny)/2.0)).r - 0.5;

//        rgb.x = clamp( yuv.x + 1.402 *yuv.z, 0, 1);
//        rgb.y = clamp( yuv.x - 0.34414 * yuv.y - 0.71414 * yuv.z, 0, 1);
//        rgb.z = clamp( yuv.x + 1.772 * yuv.y, 0, 1);
    } else {
        //YUV444P
        yuv.x = texture2D(tex_y, v_texCoord).r;
        yuv.y = texture2D(tex_u, v_texCoord).r - 0.5;
        yuv.z = texture2D(tex_v, v_texCoord).r - 0.5;

        rgb.x = clamp( yuv.x + 1.402 *yuv.z, 0, 1);
        rgb.y = clamp( yuv.x - 0.34414 * yuv.y - 0.71414 * yuv.z, 0, 1);
        rgb.z = clamp( yuv.x + 1.772 * yuv.y, 0, 1);
    }
    gl_FragColor = vec4(rgb, 1);
}

