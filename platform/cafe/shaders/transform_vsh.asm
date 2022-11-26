; $MODE = "UniformRegister"

; $ATTRIB_VARS[0].name = "position"
; $ATTRIB_VARS[0].type = "vec4"
; $ATTRIB_VARS[0].location = 0
; $ATTRIB_VARS[1].name = "color"
; $ATTRIB_VARS[1].type = "vec4"
; $ATTRIB_VARS[1].location = 1

; $SPI_VS_OUT_CONFIG.VS_EXPORT_COUNT = 0
; $NUM_SPI_VS_OUT_ID = 1
; $SPI_VS_OUT_ID[0].SEMANTIC_0 = 0

00 CALL_FS NO_BARRIER
01 EXP_DONE: POS0, R1
02 EXP_DONE: PARAM0, R2 NO_BARRIER
END_OF_PROGRAM
