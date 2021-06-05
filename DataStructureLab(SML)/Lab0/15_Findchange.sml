fun printInt (a:int) =
    print(Int.toString(a)^" ");
fun printIntInf (a:IntInf.int) =
    print(IntInf.toString(a)^" ");
fun printReal (a:real) =
    print(Real.toString(a)^" ");
fun printString (a:string) =
    print(a^" ");
fun getInt () =
    Option.valOf (TextIO.scanStream (Int.scan StringCvt.DEC) TextIO.stdIn);
fun getIntInf () =
    Option.valOf (TextIO.scanStream (IntInf.scan StringCvt.DEC) TextIO.stdIn);
fun getReal () =
    Option.valOf (TextIO.scanStream (Real.scan) TextIO.stdIn);
fun printIntTable ( [] ) = ()
  | printIntTable ( x::xs ) = 
    let
	val tmp = printInt(x)
    in
	printIntTable(xs)
    end;
fun printIntInfTable ( [] ) = ()
  | printIntInfTable ( x::xs ) = 
    let
	val tmp = printIntInf(x)
    in
	printIntInfTable(xs)
    end;
fun getIntTable ( 0 ) = []
  | getIntTable ( N:int) = getInt()::getIntTable(N-1);
fun getIntInfTable ( 0 ) = []
  | getIntInfTable ( N:IntInf.int) = getIntInf()::getIntInfTable(N-1);

fun findchange((v::cv:(IntInf.int list)),(cost:IntInf.int),(n:IntInf.int)):IntInf.int = 
    if cost>0 andalso v>cost then findchange(cv,cost,n)
    else if cost > 0 andalso v<=cost then findchange(v::cv,cost-v,n+1)
    else n;

val num:IntInf.int = getIntInf();
val cv:IntInf.int list = getIntInfTable(num);
val cost:IntInf.int = getIntInf();
val result:IntInf.int = findchange(cv,cost,0);
printIntInf(result);
