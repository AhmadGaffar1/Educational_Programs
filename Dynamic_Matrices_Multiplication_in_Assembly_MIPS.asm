.data
A: .word 0:100
B: .word 0:100
C: .word 0:100

A_row: .word 0
A_col: .word 0
B_row: .word 0
B_col: .word 0
C_row: .word 0
C_col: .word 0

mss0: .asciiz "This Program Is Multiplication Of Two Matrix A , B\n"
mss1: .asciiz "Enter The Number Of Rows Of Matrix A :: "
mss2: .asciiz "Enter The Number Of Cols Of Matrix A :: "
mss3: .asciiz "Enter The Number Of Rows Of Matrix B :: "
mss4: .asciiz "Enter The Number Of Cols Of Matrix B :: "
mss5: .asciiz "The Dimantions Is Not Valid, Must Matrix A Cols Equal To Matrix B Rows :: [X][Y] x [Y][Z] == [X][Z]\n Restarted Programe !!!\n"
mss6: .asciiz "A["
mss7: .asciiz "B["
mss8: .asciiz "]["
mss9: .asciiz "] == "
mss10: .asciiz "\n"
mss11: .asciiz "   "
mss12: .asciiz "Matrix A ==\n"
mss13: .asciiz "Matrix B ==\n"
mss14: .asciiz "Matrix C == A x B ==\n"

############################################################################################################################################
.text
main:
li $v0,4
la $a0,mss0
syscall

li $v0,4
la $a0,mss1
syscall
li $v0,5
syscall
addi $s1,$v0,0

li $v0,4
la $a0,mss2
syscall
li $v0,5
syscall
addi $s2,$v0,0

li $v0,4
la $a0,mss3
syscall
li $v0,5
syscall
addi $s3,$v0,0

li $v0,4
la $a0,mss4
syscall
li $v0,5
syscall
addi $s4,$v0,0

beq $s2,$s3,start
li $v0,4
la $a0,mss5
syscall
j main
start:

sw $s1,A_row
sw $s2,A_col
sw $s3,B_row
sw $s4,B_col
sw $s1,C_row
sw $s4,C_col

j input
INPUT:

j multiplication
MULTIPLICATION:

j output
OUTPUT:

li $v0,10
syscall
############################################################################################################################################
input:

la $s0,A
la $s1,A_row
lw $s1,0($s1)
la $s2,A_col
lw $s2,0($s2)
la $a1,mss6
li $t8,1
j input2
INPUT2:
li $v0,4
la $a0,mss10
syscall
syscall
syscall


la $s0,B
la $s1,B_row
lw $s1,0($s1)
la $s2,B_col
lw $s2,0($s2)
la $a1,mss7
li $t8,2
j input2
INPUT22:
li $v0,4
la $a0,mss10
syscall
syscall
syscall

j INPUT
############################################################################################################################################
input2:

li $t1,0
loop1:
	beq $t1,$s1,break1
	
	li $t2,0
	loop2:
		beq $t2,$s2,break2
		
		li $v0,4
		addi $a0,$a1,0
		syscall
		
		li $v0,1
		addi $a0,$t1,0
		syscall
		
		li $v0,4
		la $a0,mss8
		syscall
		
		li $v0,1
		addi $a0,$t2,0
		syscall
		
		li $v0,4
		la $a0,mss9
		syscall	
		
		li $v0,5
		syscall
		
		mul $v1,$t1,$s2
		add $v1,$v1,$t2
		sll $v1,$v1,2
		add $v1,$v1,$s0
		sw $v0,0($v1)	
		
		addi $t2,$t2,1
	j loop2
	break2:
	
	addi $t1,$t1,1
j loop1
break1:

beq $t8,1,INPUT2
beq $t8,2,INPUT22
j INPUT22
############################################################################################################################################
multiplication:
la $s0,A
la $s1,A_row
lw $s1,0($s1)
la $s2,A_col
lw $s2,0($s2)

la $s3,B
la $s4,B_row
lw $s4,0($s4)
la $s5,B_col
lw $s5,0($s5)

la $s6,C
li $t6,0


li $t1,0
Loop1:
	beq $t1,$s1,Break1
	
	li $t2,0
	Loop2:
		beq $t2,$s5,Break2
		
		li $t3,0
		Loop3:
			beq $t3,$s2,Break3
			
			mul $t4,$t1,$s2
			add $t4,$t4,$t3
			sll $t4,$t4,2
			add $t4,$t4,$s0
			lw $t4,0($t4)
			
			mul $t5,$t3,$s5
			add $t5,$t5,$t2
			sll $t5,$t5,2
			add $t5,$t5,$s3
			lw $t5,0($t5)
			
			mul $t4,$t4,$t5
			add $t6,$t6,$t4
			
			addi $t3,$t3,1
		j Loop3
		Break3:
		
		
		mul $t7,$t1,$s5
		add $t7,$t7,$t2
		sll $t7,$t7,2
		add $t7,$t7,$s6
		sw $t6,0($t7)
		li $t6,0
		
		
		addi $t2,$t2,1
	j Loop2
	Break2:
	
	addi $t1,$t1,1
j Loop1
Break1:

j MULTIPLICATION
############################################################################################################################################
output:

li $v0,4
la $a0,mss12
syscall
la $s0,A
la $s1,A_row
lw $s1,0($s1)
la $s2,A_col
lw $s2,0($s2)
li $t8,1
j output2
OUTPUT2:
li $v0,4
la $a0,mss10
syscall
syscall
syscall

li $v0,4
la $a0,mss13
syscall
la $s0,B
la $s1,B_row
lw $s1,0($s1)
la $s2,B_col
lw $s2,0($s2)
li $t8,2
j output2
OUTPUT22:
li $v0,4
la $a0,mss10
syscall
syscall
syscall


li $v0,4
la $a0,mss14
syscall
la $s0,C
la $s1,C_row
lw $s1,0($s1)
la $s2,C_col
lw $s2,0($s2)
li $t8,3
j output2
OUTPUT222:
li $v0,4
la $a0,mss10
syscall
syscall
syscall

j OUTPUT
############################################################################################################################################
output2:

li $t1,0
LOop1:
	beq $t1,$s1,BReak1
	
	li $t2,0
	LOop2:
		beq $t2,$s2,BReak2
		
		li $v0,1
		mul $a0,$t1,$s2
		add $a0,$a0,$t2
		sll $a0,$a0,2
		add $a0,$a0,$s0
		lw $a0,0($a0)
		syscall
		
		li $v0,4
		la $a0,mss11
		syscall
		
		addi $t2,$t2,1
	j LOop2
	BReak2:
	
	
	li $v0,4
	la $a0,mss10
	syscall
	
	
	add $t1,$t1,1
j LOop1
BReak1:

beq $t8,1,OUTPUT2
beq $t8,2,OUTPUT22
beq $t8,3,OUTPUT222
j OUTPUT222
############################################################################################################################################
