Use the following command in linux terminal (Ubuntu 16.04) with gcc 5.4.0 and nasm 12.14.02 installed:
    nasm -felf64 code.asm && gcc code.o && ./a.out

ASSUMPTIONS:
1. (As of now) The iterator variable of FOR loop will have the value right bound + 1 after loop has terminated.
2. (Edit this!) SWITCH statement cannot have (dynamic) arrays declared in one of the cases. Because that array might be
    used in some other case and we do not know how much space to reserve on stack for this array.
3. If any output variable is assigned in any one of the cases it is assumed as assigned even though that case may not
    execute at runtime. Thus it is assumed that all the output list variables are assigned in the source code and this
    is not checked at runtime.
4. The variable bounds in dynamic arrays are assumed to be positive since for static they are NUM.
5. (remove maybe) In format strings, it is assumed that all the involved integers to be printed are of atmost 4 digits.