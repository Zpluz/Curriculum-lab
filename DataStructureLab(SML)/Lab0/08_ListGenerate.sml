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

fun output_list ([]) = ()
    | output_list(x::xs) =
        let val xout=printIntInf(x);
        in output_list(xs) end;

fun generate((n:IntInf.int),(m:IntInf.int)):IntInf.int list =
    if n>m then nil
    else  n::generate(n+1,m);

val st:IntInf.int = getIntInf();
val ed:IntInf.int = getIntInf();
val lst:IntInf.int list = generate(st,ed);
output_list(lst);


