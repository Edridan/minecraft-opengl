uniform sampler2D Texture0;
uniform sampler2D Texture1;

uniform float screen_width;
uniform float screen_height;

const vec4 moon_color = vec4(1.f,1.f,1.f,1.f);

#define OUTLINE 1.5F
#define DEPTH_DETOURAGE 0.1F

float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

// Get the distance for the pixel
float GetPixelDepth(in vec2 pixelPos)
{
	float depth = texture2D( Texture1 , pixelPos).r;
	return LinearizeDepth(depth);
}

// Detourage
vec4 GetColor(float pixelDepth)
{
	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );

	if (color == moon_color)	// Expect the moon and the star color
		return color;

	vec2 pos = vec2( gl_TexCoord[0] );

	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;

	int i = 0;

	for (int x = -1; x <= 1; x++)
		for (int y =- 1; y <=  1; y++)
		{
			if (x != 0 && y != 0)
				if (GetPixelDepth(vec2(pos.x + (xstep * OUTLINE * x),pos.y + (ystep * OUTLINE * y))) - (DEPTH_DETOURAGE * pixelDepth) > pixelDepth)
					i++;
		}

	if (i > 1)
		color = vec4(0,0,0,1);
	


	return color;
}

void main (void)
{
	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec2 pos = vec2(gl_TexCoord[0]);

	vec4 color = texture2D( Texture0 , pos  );
	
	//Permet de scaler la profondeur
	float depth = GetPixelDepth(pos);
	
	if (depth == 1)
		color = vec4(0,0,0,1);

	gl_FragColor = GetColor(depth);
}