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

fun half (b:IntInf.int) = b div IntInf.fromInt(2);
fun powermod ((a :IntInf.int),(b:IntInf.int),m):IntInf.int = 
    if b=1 then a mod m
    else if((b mod 2)=0) then let val interhalf = powermod(a,half(b),m) in (interhalf*interhalf) mod m end
    else (a * powermod(a,b-1,m)) mod m;
val a:IntInf.int = getIntInf();
val n:IntInf.int = getIntInf();
val m:IntInf.int = getIntInf();
val result = powermod(a,n,m);
printIntInf(result);
