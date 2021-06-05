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

fun sum((n:IntInf.int),(s:IntInf.int)):IntInf.int = 
if n>0 then 
    let
      val number = getIntInf()
    in
      sum(n-1,s+number)
    end
else s;

val n:IntInf.int = getIntInf();
val result:IntInf.int = sum(n,IntInf.fromInt(0));
printIntInf(result);