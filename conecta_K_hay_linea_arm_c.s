    AREA datos, DATA, READWRITE
deltas_fila     EQU     0x00ffff01
deltas_columna  EQU     0xff00ffff
n_deltas        EQU     4
k_size          EQU     4

	AREA codigo,CODE, READONLY
	;
	IMPORT conecta_K_buscar_alineamiento_c
	EXPORT conecta_K_hay_linea_arm_c
	PRESERVE8


conecta_K_hay_linea_arm_c
	STMDB     R13!,{R4-R10,R14}
	SUB 	R13,R13,#0x0010
	MOV       R9,R0
	MOV       R7,R1
	MOV       R8,R2
	MOV       R10,R3
	LDR       R0,=deltas_fila			;	 int8_t deltas_fila[N_DELTAS] = {0, -1, -1, 1};
	STR       R0,[R13,#0x000C]
	LDR       R0,=deltas_columna		;	 int8_t deltas_columna[N_DELTAS] = {-1, 0, -1, -1};
	STR       R0,[R13,#0x0008]
	MOV       R4,#0x00000000			;	 unsigned int i = 0;
	MOV       R5,#0x00000000			;	 uint8_t linea = FALSE;
	MOV       R6,#0x00000000			;	 uint8_t long_linea = 0;
	NOP       
	B         salto1
salto8	ADD       R0,R13,#0x00000008	;	 long_linea = conecta_K_buscar_alineamiento_c(t, fila, columna, color, deltas_fila[i], deltas_columna[i]);
	LDRSB     R0,[R0,R4]
	ADD       R1,R13,#0x0000000C
	LDRSB     R1,[R1,R4]
	MOV       R3,R10
	MOV       R2,R8
	STR       R1,[R13]
	MOV       R1,R7
	STR       R0,[R13,#0x0004]
	MOV       R0,R9
	BL        conecta_K_buscar_alineamiento_c
	MOV       R6,R0						;	linea = long_linea >= K_SIZE;
	CMP       R6,#k_size				
	BLT       salto2
	MOV       R0,#0x00000001
	B         salto3
salto2	MOV       R0,#0x00000000			
salto3	MOV       R5,R0
	CMP       R5,#0x00000000			;	if (linea) {continue;}
	BEQ       salto4
	B         salto5
salto4	ADD       R0,R13,#0x00000008	;	       long_linea += conecta_K_buscar_alineamiento_c(t, fila-deltas_fila[i],columna-deltas_columna[i], color, -deltas_fila[i], -deltas_columna[i]);
	LDRB      R0,[R0,R4]
	RSB       R0,R0,#0x00000000
	MOV       R0,R0,LSL #24
	MOV       R0,R0,ASR #24
	ADD       R1,R13,#0x0000000C
	LDRB      R1,[R1,R4]
	RSB       R1,R1,#0x00000000
	MOV       R1,R1,LSL #24
	MOV       R1,R1,ASR #24
	STR       R0,[R13,#0x0004]
	STR       R1,[R13]
	ADD       R0,R13,#0x00000008
	LDRB      R0,[R0,R4]
	SUB       R0,R8,R0
	AND       R2,R0,#0x000000FF
	ADD       R0,R13,#0x0000000C
	LDRB      R0,[R0,R4]
	SUB       R0,R7,R0
	AND       R1,R0,#0x000000FF
	MOV       R3,R10
	MOV       R0,R9
	BL        conecta_K_buscar_alineamiento_c
	ADD       R0,R0,R6							;	linea = long_linea >= K_SIZE;
	AND       R6,R0,#0x000000FF
	CMP       R6,#k_size
	BLT       salto6
	MOV       R0,#0x00000001
	B         salto7
salto6	MOV       R0,#0x00000000	
salto7	MOV       R5,R0
	NOP       
salto5	ADD       R4,R4,#0x00000001				;	for(i=0; (i < N_DELTAS) && (linea == FALSE); ++i)
salto1	CMP       R4,#n_deltas				
	BCS       salto9
	CMP       R5,#0x00000000
	BEQ       salto8
salto9	MOV       R0,R5						;	   return linea;
	ADD       R13,R13,#0x00000010
	LDMIA     R13!,{R4-R10,R14}
	BX        R14
	
	END
