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
fun gcd((n:IntInf.int),(m:IntInf.int)):IntInf.int = 
    if(m=0) then n
    else gcd(m,n mod m);
val a=getIntInf();
val b=getIntInf();
val c=gcd(a,b);
printIntInf(c);