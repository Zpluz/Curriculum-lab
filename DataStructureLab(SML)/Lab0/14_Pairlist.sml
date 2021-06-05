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

fun find((x::xs : (int*int) list),idn:int):int = 
    if (#2 x) <> idn then find(xs,idn)
    else  #1 x;

val num = getInt();
val name: int list = getIntTable(num);
val idnumber:int list = getIntTable(num);
val pairlist = ListPair.zip(name,idnumber);
val idn:int = getInt();
val p = find(pairlist,idn);
printInt(p);