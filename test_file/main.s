addi $28, $0, 9
addi $29, $29, 2047
addi $8, $0, 0
sw $8, 0($0)
addi $9, $0, 1
sw $9, 1($0)
addi $10, $0, 2
sw $10, 2($0)
addi $11, $0, 3
sw $11, 3($0)
addi $12, $0, 0
sw $8, 4($0)
sw $9, 5($0)
sw $10, 6($0)
sw $11, 7($0)
sw $12, 8($0)
jal main
add $8, $0, $2
j "16"
pinMode:
addi $10, $0, 12288
add $10, $10, $4
sw $5, 0($10)
"1":
jr $31
digitalWrite:
addi $10, $0, 4096
add $10, $10, $4
sw $5, 0($10)
"4":
jr $31
digitalRead:
addi $9, $0, 4096
add $9, $9, $4
lw $2, 0($9)
"7":
jr $31
stepMotor:
addi $9, $0, 38250
addi $10, $0, 3
and $10, $4, $10
addi $11, $0, 0
"12":
blt $11, $10, "13"
j "15"
"13":
sra $12, $9, 4
add $9, $0, $12
"14":
addi $11, $11, 1
j "12"
"15":
addi $12, $0, 1
and $12, $9, $12
sra $13, $9, 1
addi $14, $0, 1
and $14, $13, $14
sra $13, $9, 2
addi $15, $0, 1
and $15, $13, $15
sra $13, $9, 3
addi $16, $0, 1
and $16, $13, $16
sw $12, 4096($0)
sw $14, 4097($0)
sw $15, 4098($0)
sw $16, 4099($0)
"10":
jr $31
main:
addi $8, $0, 0
addi $9, $0, 1
add $4, $0, $8
add $5, $0, $9
addi $29, $29, -3
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
jal pinMode
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
addi $29, $29, 3
addi $10, $0, 1
addi $11, $0, 1
add $4, $0, $10
add $5, $0, $11
addi $29, $29, -5
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
jal pinMode
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
addi $29, $29, 5
addi $12, $0, 2
addi $13, $0, 1
add $4, $0, $12
add $5, $0, $13
addi $29, $29, -7
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
jal pinMode
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
addi $29, $29, 7
addi $14, $0, 3
addi $15, $0, 1
add $4, $0, $14
add $5, $0, $15
addi $29, $29, -9
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
sw $14, 7($29)
sw $15, 8($29)
jal pinMode
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
lw $14, 7($29)
lw $15, 8($29)
addi $29, $29, 9
addi $16, $0, 0
addi $17, $0, 0
add $4, $0, $16
add $5, $0, $17
addi $29, $29, -11
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
sw $14, 7($29)
sw $15, 8($29)
sw $16, 9($29)
sw $17, 10($29)
jal digitalWrite
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
lw $14, 7($29)
lw $15, 8($29)
lw $16, 9($29)
lw $17, 10($29)
addi $29, $29, 11
addi $18, $0, 1
addi $19, $0, 0
add $4, $0, $18
add $5, $0, $19
addi $29, $29, -13
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
sw $14, 7($29)
sw $15, 8($29)
sw $16, 9($29)
sw $17, 10($29)
sw $18, 11($29)
sw $19, 12($29)
jal digitalWrite
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
lw $14, 7($29)
lw $15, 8($29)
lw $16, 9($29)
lw $17, 10($29)
lw $18, 11($29)
lw $19, 12($29)
addi $29, $29, 13
addi $20, $0, 2
addi $21, $0, 0
add $4, $0, $20
add $5, $0, $21
addi $29, $29, -15
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
sw $14, 7($29)
sw $15, 8($29)
sw $16, 9($29)
sw $17, 10($29)
sw $18, 11($29)
sw $19, 12($29)
sw $20, 13($29)
sw $21, 14($29)
jal digitalWrite
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
lw $14, 7($29)
lw $15, 8($29)
lw $16, 9($29)
lw $17, 10($29)
lw $18, 11($29)
lw $19, 12($29)
lw $20, 13($29)
lw $21, 14($29)
addi $29, $29, 15
addi $22, $0, 3
addi $23, $0, 0
add $4, $0, $22
add $5, $0, $23
addi $29, $29, -17
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
sw $14, 7($29)
sw $15, 8($29)
sw $16, 9($29)
sw $17, 10($29)
sw $18, 11($29)
sw $19, 12($29)
sw $20, 13($29)
sw $21, 14($29)
sw $22, 15($29)
sw $23, 16($29)
jal digitalWrite
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
lw $14, 7($29)
lw $15, 8($29)
lw $16, 9($29)
lw $17, 10($29)
lw $18, 11($29)
lw $19, 12($29)
lw $20, 13($29)
lw $21, 14($29)
lw $22, 15($29)
lw $23, 16($29)
addi $29, $29, 17
addi $24, $0, 18
"19":
addi $25, $0, 1
bne $25, $0, "20"
j "21"
"20":
add $24, $0, $24
add $4, $0, $24
addi $29, $29, -19
sw $31, 0($29)
sw $8, 1($29)
sw $9, 2($29)
sw $10, 3($29)
sw $11, 4($29)
sw $12, 5($29)
sw $13, 6($29)
sw $14, 7($29)
sw $15, 8($29)
sw $16, 9($29)
sw $17, 10($29)
sw $18, 11($29)
sw $19, 12($29)
sw $20, 13($29)
sw $21, 14($29)
sw $22, 15($29)
sw $23, 16($29)
sw $25, 17($29)
sw $24, 18($29)
jal stepMotor
lw $31, 0($29)
lw $8, 1($29)
lw $9, 2($29)
lw $10, 3($29)
lw $11, 4($29)
lw $12, 5($29)
lw $13, 6($29)
lw $14, 7($29)
lw $15, 8($29)
lw $16, 9($29)
lw $17, 10($29)
lw $18, 11($29)
lw $19, 12($29)
lw $20, 13($29)
lw $21, 14($29)
lw $22, 15($29)
lw $23, 16($29)
lw $24, 17($29)
lw $25, 18($29)
addi $29, $29, 19
addi $25, $25, 1

j "19"
"21":
addi $25, $0, 0
"17":
jr $31
"16":
add $0, $0, $0
