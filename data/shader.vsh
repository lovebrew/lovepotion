; setup constants
	.const c20, 0.0, 0.0, 0.0, 1.0

; setup outmap
	.out o0, result.position, 0xF
	.out o1, result.texcoord0, 0x3
	.out o2, result.color, 0xF

; setup uniform map (not required)
	.uniform c0, c3, projection

	.vsh vmain, end_vmain

;code
	vmain:
		; result.pos = projMtx * in.pos
		dp4 o0, c0, v0 (0x0)
		dp4 o0, c1, v0 (0x1)
		dp4 o0, c2, v0 (0x2)
		dp4 o0, c3, v0 (0x3)
		; result.texcoord = in.texcoord
		mov o1, v1 (0x5)
		; result.color = in.color
		mov o2, v1 (0x5)
		nop
		end
	end_vmain:
 
;operand descriptors
	.opdesc x___, xyzw, xyzw ; 0x0
	.opdesc _y__, xyzw, xyzw ; 0x1
	.opdesc __z_, xyzw, xyzw ; 0x2
	.opdesc ___w, xyzw, xyzw ; 0x3
	.opdesc xyz_, xyzw, xyzw ; 0x4
	.opdesc xyzw, xyzw, xyzw ; 0x5
	.opdesc x_zw, xyzw, xyzw ; 0x6
	.opdesc xyzw, yyyw, xyzw ; 0x7
	.opdesc xyz_, wwww, wwww ; 0x8
	.opdesc xyz_, yyyy, xyzw ; 0x9
