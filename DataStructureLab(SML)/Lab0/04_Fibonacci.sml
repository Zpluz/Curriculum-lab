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

fun Fibonacci ((a:IntInf.int),(b:IntInf.int),(n:IntInf.int),(m:IntInf.int)):IntInf.int = 
    if n>2 then Fibonacci((a mod m)+(b mod m),a mod m,n-1,m)
    else a mod m;
val n:IntInf.int=getIntInf();
val m:IntInf.int=getIntInf();
val result:IntInf.int= Fibonacci(1,1,n,m);
printIntInf(result);
