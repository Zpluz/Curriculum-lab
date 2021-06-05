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

val n=getInt()
and m=getInt(); 

val hx=getInt()
and hy=getInt();

fun numofpath (0,0) = 0
  | numofpath((x:int),(y:int)) = 
        if x = hx andalso y = hy then 0
        else if x = hx - 2 andalso y = hy - 1 then 0
        else if x = hx + 1 andalso y = hy - 2 then 0
        else if x = hx + 2 andalso y = hy - 1 then 0
        else if x = hx - 1 andalso y = hy + 2 then 0
        else if x = hx - 2 andalso y = hy + 1 then 0
        else if x = hx + 1 andalso y = hy + 2 then 0
        else if x = hx + 2 andalso y = hy + 1 then 0
        else if x = hx - 1 andalso y = hy - 2 then 0
        else if x = 0 andalso y = 1 then 1
        else if y = 0 andalso x = 1 then 1
        else if x = 0 andalso y > 1 then numofpath(x,y-1)
        else if y = 0 andalso x > 1 then numofpath(x-1,y)
        else numofpath(x-1,y) + numofpath(x,y-1);

val result = numofpath(n,m);
printInt(result);
