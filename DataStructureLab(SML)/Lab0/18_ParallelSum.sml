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
fun getIntVector ( 0 ) =  Vector.fromList []
  | getIntVector ( N:int) = Vector.fromList(getIntTable(N));
fun getIntInfVector ( 0 ) = Vector.fromList []
  | getIntInfVector ( N:IntInf.int) = Vector.fromList(getIntInfTable(N));

fun getsum([]) = 0
    | getsum (x::xs) = x + getsum(xs);

val num = getInt();
val halfnum = num div 2;
val tbleft = getIntTable(halfnum);
val tbright = getIntTable(num - halfnum);
val result = getsum(map getsum [tbleft,tbright]);

printInt(result);