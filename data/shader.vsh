; Outputs
.out outpos position
.out outtc0 texcoord0
.out outclr color

; Inputs
.alias inpos v0
.alias inarg v1

; Uniforms
.fvec projection[4]

; Constants
.constf RGBA8_TO_FLOAT4(0.00392156862, 0, 0, 0)

.proc main
	; outpos = projection * in.pos
	dp4 outpos.x, projection[0].wzyx, inpos
	dp4 outpos.y, projection[1].wzyx, inpos
	dp4 outpos.z, projection[2].wzyx, inpos
	dp4 outpos.w, projection[3].wzyx, inpos

	; outtc0 = in.texcoord
	mov outtc0, inarg

	; outclr = RGBA8_TO_FLOAT4(in.color)
	mul outclr, RGBA8_TO_FLOAT4.xxxx, inarg

	end
.end