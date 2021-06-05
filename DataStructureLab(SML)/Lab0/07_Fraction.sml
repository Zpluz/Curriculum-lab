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

fun gcd((n:IntInf.int),(m:IntInf.int)):IntInf.int=
 if(m=0) then n
 else gcd(m,n mod m);
val soff1:IntInf.int = getIntInf();
val moff1:IntInf.int = getIntInf();
val soff2:IntInf.int = getIntInf();
val moff2:IntInf.int = getIntInf();
val mo = (moff1*moff2) div (gcd(moff1,moff2));
val tim1 = mo div moff1;
val tim2 = mo div moff2;
val so = soff1*tim1 + soff2*tim2;
printIntInf(so div gcd(so,mo));
printIntInf(mo div gcd(so,mo));