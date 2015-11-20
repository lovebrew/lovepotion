; Outputs
.out outpos position
.out outtc0 texcoord0
.out outclr color

; Inputs
.alias inpos v0
.alias inarg v1

; Uniforms
.fvec projection[4]

.proc main
	; outpos = projection * in.pos
	dp4 outpos.x, projection[0].wzyx, inpos
	dp4 outpos.y, projection[1].wzyx, inpos
	dp4 outpos.z, projection[2].wzyx, inpos
	dp4 outpos.w, projection[3].wzyx, inpos

	; outtc0 = in.texcoord
	mov outtc0, inarg

	; outclr = in.color
	mov outclr, inarg

	end
.end
