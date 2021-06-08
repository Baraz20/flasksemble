opening: .string "HI, this is a tester"
var1: .data 4  , +23 , 	-24
var2: .data -3
;mov part with all variations
mov #-1 ,	var1
mov #-2 ,	r7
mov var1 ,	var1
mov var1 ,	r7
mov r5 ,	var1
mov r6 ,	r7

;cmp part with all variations
cmp #+3	,	#-4
cmp #+5	,	var2
cmp #+6	,	r3
cmp var2,	#-7
cmp var2,	var2
cmp var2,	r3
cmp r3	,	#8
cmp r3	,	var2
cmp r3	,	r3

;add part with all variations
add #-3 ,	var4
add #-4 ,	r4
add var2 ,	var1
add var2 ,	r4
add r2 ,	var4
add r3 ,	r4


var3: .string "T"
.entry var1
.entry var3
.extern var4


;sub part with all variations
sub #-32 ,	var3
sub #+12 ,	r5
sub var1 ,	var1
sub var3 ,	r6
sub r5 ,	var3
sub r4 ,	r0

;lea part with all variations
lea var4 ,	var3
lea var2 ,	r5

;clr part with all variations
clr var4
clr r6

;not part with all variations
not var2
not r3

;inc part with all variations
inc var1
inc r1

;dec part with all variations
dec var3
dec r7

;jmp part with all variations
jmp var1
jmp var1

;bne part with all variations
bne var2
bne var1

;jsr part with all variations
jsr var3
jsr var1

;red part with all variations
red var3
red r2

;prn part with all variations
prn #-65
prn var1
prn r4

;rts part with all variations
rts

;stop part with all variations
stop
