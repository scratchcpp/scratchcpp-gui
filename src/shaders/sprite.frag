// Ported from https://github.com/scratchfoundation/scratch-render/blob/4090e62e8abf427e55c83448da9b0df26120d2fb/src/shaders/sprite.frag

#undef lowp
#undef mediump
#undef highp

precision mediump float;

#ifdef ENABLE_color
uniform float u_color;
#endif // ENABLE_color

#ifdef ENABLE_brightness
uniform float u_brightness;
#endif // ENABLE_brightness

#ifdef ENABLE_ghost
uniform float u_ghost;
#endif // ENABLE_ghost

#ifdef ENABLE_fisheye
uniform float u_fisheye;
#endif // ENABLE_fisheye

#ifdef ENABLE_whirl
uniform float u_whirl;
#endif // ENABLE_whirl

#ifdef ENABLE_pixelate
uniform float u_pixelate;
uniform vec2 u_skinSize;
#endif // ENABLE_pixelate

#ifdef ENABLE_mosaic
uniform float u_mosaic;
#endif // ENABLE_mosaic

in vec2 v_texCoord;
out vec4 fragColor;
uniform sampler2D u_skin;

// Add this to divisors to prevent division by 0, which results in NaNs propagating through calculations.
// Smaller values can cause problems on some mobile devices.
const float epsilon = 1e-3;

#if defined(ENABLE_color)
// Branchless color conversions based on code from:
// http://www.chilliant.com/rgb2hsv.html by Ian Taylor
// Based in part on work by Sam Hocevar and Emil Persson
// See also: https://en.wikipedia.org/wiki/HSL_and_HSV#Formal_derivation


// Convert an RGB color to Hue, Saturation, and Value.
// All components of input and output are expected to be in the [0,1] range.
vec3 convertRGB2HSV(vec3 rgb)
{
	// Hue calculation has 3 cases, depending on which RGB component is largest, and one of those cases involves a "mod"
	// operation. In order to avoid that "mod" we split the M==R case in two: one for G<B and one for B>G. The B>G case
	// will be calculated in the negative and fed through abs() in the hue calculation at the end.
	// See also: https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
	const vec4 hueOffsets = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);

	// temp1.xy = sort B & G (largest first)
	// temp1.z = the hue offset we'll use if it turns out that R is the largest component (M==R)
	// temp1.w = the hue offset we'll use if it turns out that R is not the largest component (M==G or M==B)
	vec4 temp1 = rgb.b > rgb.g ? vec4(rgb.bg, hueOffsets.wz) : vec4(rgb.gb, hueOffsets.xy);

	// temp2.x = the largest component of RGB ("M" / "Max")
	// temp2.yw = the smaller components of RGB, ordered for the hue calculation (not necessarily sorted by magnitude!)
	// temp2.z = the hue offset we'll use in the hue calculation
	vec4 temp2 = rgb.r > temp1.x ? vec4(rgb.r, temp1.yzx) : vec4(temp1.xyw, rgb.r);

	// m = the smallest component of RGB ("min")
	float m = min(temp2.y, temp2.w);

	// Chroma = M - m
	float C = temp2.x - m;

	// Value = M
	float V = temp2.x;

	return vec3(
		abs(temp2.z + (temp2.w - temp2.y) / (6.0 * C + epsilon)), // Hue
		C / (temp2.x + epsilon), // Saturation
		V); // Value
}

vec3 convertHue2RGB(float hue)
{
	float r = abs(hue * 6.0 - 3.0) - 1.0;
	float g = 2.0 - abs(hue * 6.0 - 2.0);
	float b = 2.0 - abs(hue * 6.0 - 4.0);
	return clamp(vec3(r, g, b), 0.0, 1.0);
}

vec3 convertHSV2RGB(vec3 hsv)
{
	vec3 rgb = convertHue2RGB(hsv.x);
	float c = hsv.z * hsv.y;
	return rgb * c + hsv.z - c;
}
#endif // ENABLE_color

const vec2 kCenter = vec2(0.5, 0.5);

void main()
{
    vec2 texcoord0 = v_texCoord;

    #ifdef ENABLE_mosaic
    texcoord0 = fract(u_mosaic * texcoord0);
    #endif // ENABLE_mosaic

    #ifdef ENABLE_pixelate
    {
        // TODO: clean up "pixel" edges
        vec2 pixelTexelSize = u_skinSize / u_pixelate;
        texcoord0 = (floor(texcoord0 * pixelTexelSize) + kCenter) / pixelTexelSize;
    }
    #endif // ENABLE_pixelate

    #ifdef ENABLE_whirl
    {
        const float kRadius = 0.5;
        vec2 offset = texcoord0 - kCenter;
        float offsetMagnitude = length(offset);
        float whirlFactor = max(1.0 - (offsetMagnitude / kRadius), 0.0);
        float whirlActual = u_whirl * whirlFactor * whirlFactor;
        float sinWhirl = sin(whirlActual);
        float cosWhirl = cos(whirlActual);
        mat2 rotationMatrix = mat2(
            cosWhirl, -sinWhirl,
            sinWhirl, cosWhirl
        );

        texcoord0 = rotationMatrix * offset + kCenter;
    }
    #endif // ENABLE_whirl

    #ifdef ENABLE_fisheye
    {
        vec2 vec = (texcoord0 - kCenter) / kCenter;
        float vecLength = length(vec);
        float r = pow(min(vecLength, 1.0), u_fisheye) * max(1.0, vecLength);
        vec2 unit = vec / vecLength;

        texcoord0 = kCenter + r * unit * kCenter;
    }
    #endif // ENABLE_fisheye

    fragColor = texture(u_skin, texcoord0);

    #if defined(ENABLE_color) || defined(ENABLE_brightness)
    // Divide premultiplied alpha values for proper color processing
    // Add epsilon to avoid dividing by 0 for fully transparent pixels
    fragColor.rgb = clamp(fragColor.rgb / (fragColor.a + epsilon), 0.0, 1.0);

    #ifdef ENABLE_color
    {
        vec3 hsv = convertRGB2HSV(fragColor.rgb);

        // Force grayscale values to be slightly saturated
        const float minLightness = 0.11 / 2.0;
        const float minSaturation = 0.09;
        if (hsv.z < minLightness) hsv = vec3(0.0, 1.0, minLightness);
        else if (hsv.y < minSaturation) hsv = vec3(0.0, minSaturation, hsv.z);

        hsv.x = mod(hsv.x + u_color, 1.0);
        if (hsv.x < 0.0) hsv.x += 1.0;

        fragColor.rgb = convertHSV2RGB(hsv);
    }
    #endif // ENABLE_color

    #ifdef ENABLE_brightness
    fragColor.rgb = clamp(fragColor.rgb + vec3(u_brightness), vec3(0), vec3(1));
    #endif // ENABLE_brightness

    // Re-multiply color values
    fragColor.rgb *= fragColor.a + epsilon;

    #endif // defined(ENABLE_color) || defined(ENABLE_brightness)

    #ifdef ENABLE_ghost
    fragColor *= u_ghost;
    #endif // ENABLE_ghost
}
