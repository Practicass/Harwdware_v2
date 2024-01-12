    AREA datos, DATA, READWRITE
deltas_fila     EQU     0x00ffff01
n_deltas        EQU     4
k_size          EQU     4
num_filas       EQU     7
num_columnas    EQU     7
max_no_cero     EQU     6

	AREA codigo,CODE, READONLY
	EXPORT conecta_K_hay_linea_arm_arm
	PRESERVE8
	;ENTRY
conecta_K_hay_linea_arm_arm
	STMDB     R13!,{R4-R10,R14}			
	SUB       R13,R13,#0x00000024
	STR       R0,[R13,#0X0018]
	MOV       R4,R1
	MOV       R5,R2
	MOV       R7,R3
	;...
	STR       R7,[R13,#0x00020]
	;...
	LDR       R0,=deltas_fila		;   int8_t deltas_fila[N_DELTAS] = {0, -1, -1, 1};
	STR       R0,[R13,#0x0010]
	MVN 	  R0, #0x00FF0000			;LDR       R0,=deltas_columna	;   int8_t deltas_columna[N_DELTAS] = {-1, 0, -1, -1};
	STR       R0,[R13,#0x000C]
	MOV       R1,#0x00000000		;   unsigned int i = 0;
	;...
	STR       R1,[R13,#0x0001c]
	;...
	MOV       R10,#0x00000000		;   uint8_t linea = FALSE;
	MOV       R8,#0x00000000		;   uint8_t long_linea = 0;
	MOV       R9,#0x00000000		;   unsigned int aux = 0;
	;...
	STR       R9,[R13,#0X0014]
	;...
	MOV       R2,#0x00000000		;   uint8_t fila2=0;
	MOV       R3,#0x00000000		;   uint8_t columna2=0;
	MOV       R0,#0x00000000
	STR       R0,[R13,#0x0008]		;   uint8_t delta_columna2=0;
	STR       R0,[R13,#0x0004]		;	uint8_t delta_fila2=0;
	STR       R0,[R13]				;	int8_t res=0;
jump14  LDR R1,[R13,#0x001c]		;  	while((i < N_DELTAS) && (linea == FALSE))
	CMP       R1,#n_deltas
	BCS       jump13
	CMP       R10,#0x00000000
	BNE       jump13
	LDR       R9,[r13,#0x14]		;	if(aux % 2)
	AND       R0,R9,#0x00000001
	CMP       R0,#0x00000000
	BEQ       jump2

	ADD       R0,R13,#0x00000010	;	fila2 = fila-deltas_fila[i];
	LDR       R1,[R13,#0x001c]
	RSB	   R1,R1,#0x003
	LDRB      R0,[R0,R1]
	SUB       R0,R4,R0
	AND       R2,R0,#0x000000FF

	ADD       R0,R13,#0x0000000C	;	columna2 = columna-deltas_columna[i];
	LDRB      R0,[R0,R1]
	SUB       R0,R5,R0
	AND       R3,R0,#0x000000FF

	ADD       R0,R13,#0x00000010	;	delta_fila2 = -deltas_fila[i];
	LDRB      R0,[R0,R1]
	RSB       R0,R0,#0x00000000
	AND       R0,R0,#0x000000FF
	STR       R0,[R13,#0x0004]

	ADD       R0,R13,#0x0000000C	;	delta_columna2 = -deltas_columna[i];
	LDRB      R0,[R0,R1]
	RSB       R0,R0,#0x00000000
	AND       R0,R0,#0x000000FF
	STR       R0,[R13,#0x0008]

	RSB	   R1,R1,#0x003				;	i++;
	ADD       R1,R1,#0x00000001
	STR		  R1,[R13,#0x001c]
	B         jump3
jump2 MOV       R2,R4				;	fila2 = fila;
	MOV       R3,R5					;	columna2 = columna;
	ADD       R0,R13,#0x00000010	;	delta_fila2 = deltas_fila[i];
	LDR       R1,[R13,#0x001c]
	RSB	   R1,R1,#0x003
	LDRB      R0,[R0,R1]
	STR       R0,[R13,#0x0004]

	ADD       R0,R13,#0x0000000C	;	delta_columna2 = deltas_columna[i];
	LDRB      R0,[R0,R1]
	STR       R0,[R13,#0x0008]
	MOV       R8,#0x00000000		;	long_linea = 0;
jump3 LDR R9,[R13,#0x14]			;	aux ++;
	ADD       R9,R9,#0x00000001
	STR 	  R9,[R13,#0x00014]
	MOV       R6,#0x00000000		;	uint8_t longitud = 0;
jump10 CMP       R2,#num_filas		;	 if( fila2 >= NUM_FILAS || columna2 >= NUM_COLUMNAS)
	BGE       jump17
	CMP       R3,#num_columnas
	BLT       jump4
jump17 MVN       R0,#0x00000000		;	res = -1;
	STR       R0,[R13]
	B         jump15
jump4 MOV       R0,#0x00000000		;	size_t col = 0;
	B         jump5
jump6 ADD       R0,R0,#0x00000001	;	for(; (col < MAX_NO_CERO) && (t->columnas[fila2][col] != columna2); ++col);
jump5 CMP       R0,#max_no_cero
	BCS       jump16
	;---
	LDR       R9,[R13,#0x0018]
	ADD       R1,R2,R2,LSL #1
	ADD       R1,R9,R1,LSL #1
	LDRB      R1,[R1,R0]
	;---
	CMP       R1,R3
	BNE       jump6
jump16 CMP       R0,#max_no_cero	;	if(col == MAX_NO_CERO)
	BNE       jump7

	MVN       R1,#0x00000000		;	res =  -1;
	STR       R1,[R13]
	B         jump8

	;----
jump7 ADD       R1,R2,R2,LSL #1		;	if((t->no_ceros[fila2][col] & 0x03) == color)
	ADD       R7,R9,#0x0000002A
	ADD       R1,R7,R1,LSL #1
	LDRB      R1,[R1,R0]
	AND       R1,R1,#0x00000003
	LDR       R7,[R13,#0x00020]
	CMP       R1,R7
	BNE       jump9
	;----

	MOV       R1,#0x00000000		;	res =  0;
	STR       R1,[R13]
									   
	ADD       R1,R6,#0x00000001		;	longitud++;
	AND       R6,R1,#0x000000FF
									   
	LDR       R1,[R13,#0x0004]		;	fila2 += delta_fila2;
	ADD       R1,R1,R2
	AND       R2,R1,#0x000000FF

	LDR       R1,[R13,#0x0008]		;	columna2 += delta_columna2;
	ADD       R1,R1,R3
	AND       R3,R1,#0x000000FF
	B         jump8
jump9 MVN       R1,#0x00000000		;	res= -1;
	STR       R1,[R13]
jump8 NOP  
jump15 LDR       R0,[R13]			;	while ( res == EXITO);
	CMP       R0,#0x00000000	
	BEQ       jump10

	ADD       R0,R8,R6				;	long_linea += longitud;
	AND       R8,R0,#0x000000FF

	CMP       R8,#k_size			;	linea = long_linea >= K_SIZE;
	BLT       jump11
	MOV       R0,#0x00000001
	B         jump12
jump11 MOV       R0,#0x00000000
jump12 MOV       R10,R0
	NOP
	B jump14
jump13 MOV       R0,R10				;	   return linea;
	ADD       R13,R13,#0x00000024
	LDMIA     R13!,{R4-R10,R14}
	BX        R14
	
	END