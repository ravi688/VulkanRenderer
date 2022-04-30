

vec3 theColor;
vec2 st = vST;

st.s = fract(st.s * s_Mag_Factor);
st.t = fract(st.t * t_Mag_Factor);

if(colorTest(st))
	theColor = vec3(0.8, 0.3, 0.0); 	// color of brick
else
	theColor = vec3(0.9, 0.6, 0.4); 	// color of mortar
fFragColor = vec4(theColor, 1);

// And, as before, we quickly note that the lines

st.s = fract(st.s * s_Mag_Factor);
st.t = fract(st.t * t_Mag_Factor);

// could be written more efficiently as 
st = fract(st * vec2(s_Mag_Factor, t_Mag_Factor));
