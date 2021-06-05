fun printInt (a:int) = 
    print(Int.toString(a)^" ");
fun printReal (a:real) = 
    print(Real.toString(a)^" ");
fun printString(a:string) =
    print(a^" ");
fun printIntInf(a:IntInf.int) =
    print(IntInf.toString(a)^" ");

fun getInt () =
    Option.valOf (TextIO.scanStream(Int.scan StringCvt.DEC) TextIO.stdIn);
fun getReal () =
    Option.valOf (TextIO.scanStream(Real.scan) TextIO.stdIn);
fun getIntInf () =
    Option.valOf (TextIO.scanStream (IntInf.scan StringCvt.DEC) TextIO.stdIn);

type row_vec = IntInf.int * IntInf.int;
type col_vec =  IntInf.int * IntInf.int;
type vec = IntInf.int * IntInf.int;
type matrixLeft = row_vec * row_vec;
type matrixRight = col_vec * col_vec;
type matrix = vec * vec;
type vec = col_vec;
type vec = row_vec;
type matrix = matrixLeft;
type matrix = matrixRight;


fun half (pow:IntInf.int) = pow div IntInf.fromInt(2);
fun mul_v((v1:row_vec), (v2:col_vec)):IntInf.int =
    (#1 v1 * #1 v2 + #2 v1 * #2 v2);

fun mul_m ((m1:matrixLeft),(m2:matrixRight)):matrix = 
    ((mul_v(#1 m1,#1 m2),mul_v(#1 m1,#2 m2)),(mul_v(#2 m1,#1 m2),mul_v(#2 m1,#2 m2)));

fun vecmod ((vec:vec),(m:IntInf.int)):vec = 
    ((#1 vec mod m),(#2 vec mod m));
fun mtmod ((mat:matrixLeft),(m:IntInf.int)):matrix =
    (vecmod((#1 mat),m),vecmod((#2 mat),m));

fun mat_pow ((mat:matrix),(pow:IntInf.int),(m:IntInf.int)):matrix = 
    if pow=1 then mtmod(mat,m)
    else if pow mod 2 = 0 then let val interhalf = mtmod(mat_pow(mat,half(pow),m),m) in  mtmod(mul_m(interhalf,interhalf),m) end
    else mtmod(mul_m(mat,mat_pow(mat,pow-1,m)),m);


val ms_v11:IntInf.int = 1 and ms_v12:IntInf.int = 1;
val ms_v21:IntInf.int = 1 and ms_v22:IntInf.int = 0;
val mini_v11:IntInf.int = 1 and mini_v12:IntInf.int = 1;
val mini_v21:IntInf.int = 0 and mini_v22:IntInf.int = 0;
val ms_v1:row_vec = (ms_v11 , ms_v12);
val ms_v2:row_vec = (ms_v21 , ms_v22);
val mini_v1:col_vec = (mini_v11 , mini_v12);
val mini_v2:col_vec = (mini_v21 , mini_v22);
val ms:matrixLeft = (ms_v1,ms_v2);
val mini:matrixRight = (mini_v1,mini_v2);
val pow:IntInf.int = getIntInf();
val m:IntInf.int = getIntInf();
val intermatrix=mat_pow(ms,pow-1,m);
val finalmatrix = mtmod(mul_m(intermatrix,mtmod(mini,m)),m);
printIntInf(#1 (#2 finalmatrix));
