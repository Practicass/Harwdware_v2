    AREA datos, DATA, READWRITE
num_filas       EQU     7
num_columnas    EQU     7
max_no_cero     EQU     6

	AREA codigo,CODE, READONLY
	
	IMPORT tablero_buscar_color
	EXPORT conecta_K_buscar_alineamiento_arm
	PRESERVE8
		



;conecta_K_buscar_alineamiento_arm

conecta_K_buscar_alineamiento_arm 
    STMDB     R13!,{R4-R10,R14}; GUARDA ESTADO ANTERIOR
	MOV  	R8, R1
	MOV		R9,R2
    MOV       R4,#0				;	uint8_t longitud = 0;
hop9    CMP       R8,#num_filas	;	if( fila2 >= NUM_FILAS || columna2 >= NUM_COLUMNAS) {
    BGE       hop1	
    CMP       R9,#num_columnas
    BLT       hop2
hop1 MVN       R5,#0			;	res = -1;
    B         hop3
hop2    MOV R6, #0				;	size_t col = 0;
    B hop8
hop5 ADD       R6,R6,#1			;	for(; (col < MAX_NO_CERO) && (t->columnas[fila2][col] != columna2); ++col);
hop8 CMP       R6,#max_no_cero
	BCS       hop4
	;---
	ADD       R7,R8,R8,LSL #1
	ADD       R7,R6,R7,LSL #1
	LDRB      R7,[R0,R7]
	;---
	CMP       R7,R9
	BNE       hop5
hop4 CMP      R6,#max_no_cero				;	if(col == MAX_NO_CERO) {
    BNE       hop6

    MVN       R5,#0
    B       hop3 
hop6 ADD R7,R8,R8,LSL #1		;	if((t->no_ceros[fila2][col] & 0x03) == color) {
    ADD R10,R0,#0x00002A
    ADD R7,R6,R7,LSL #1
    LDRB R7,[R10,R7]
    ;????
    AND R7,R7,#0x0003		
    CMP R7,R3
    BNE hop7

    MOV R5,#0					;	res =  0;
    ADD R7,R4,#1				;	longitud++;
    AND R4,R7,#0x0FF

    LDRB R7,[R13,#0x020]		;	fila2 += delta_fila2;
    ADD R7,R8,R7
    AND R8,R7,#0x00000FF 

    LDR R7,[R13,#0x024]			;	columna2 += delta_columna2;
    ADD R7,R9,R7
    AND R9,R7,#0x00000FF 
    B hop3

hop7 MVN R5,#0					;	res= -1;

hop3 CMP R5, #0					;	while ( res == EXITO);
    BEQ hop9
    
    MOV R0,R4					;	return longitud;
    LDMIA R13!,{R4-R10,R14}
    BX R14

	
	END
