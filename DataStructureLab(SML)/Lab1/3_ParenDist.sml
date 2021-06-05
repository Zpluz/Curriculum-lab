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

fun printEndOfLine () =
    print("\n");

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
  | getIntInfTable ( N:int) = getIntInf()::getIntInfTable(N-1);

fun getIntVector ( 0 ) =  Vector.fromList []
  | getIntVector ( N:int) = Vector.fromList(getIntTable(N));

fun getIntInfVector ( 0 ) = Vector.fromList []
  | getIntInfVector ( N:int) = Vector.fromList(getIntInfTable(N));

fun printArray ( Arr ) =
    let
	val cur = ref 0
	val len = Array.length(Arr)
    in
	while !cur < len
	do
	(
	  printInt(Array.sub(Arr,!cur));
	  cur := !cur + 1)
    end;
    
fun transtion (x : int) = if x = 0 then 1 else ~1;
fun match(1::ps,0,0) = match(ps,1,1)
    | match ((~1)::ps,0,0) = 0
    | match(_,0,hi:int):int =  hi
    | match([],record,hi:int):int = 0
    | match(p::ps,record,hi:int):int = 
    if p = 1 then match(ps,record+1,hi+1)
    else match(ps,record-1,hi+1);

fun parendist([],max) = max
    | parendist(p::ps,max) = 
    let val intermedia = match(p::ps,0,0)
    in  parendist(ps,intermedia::max) end;

val num = getInt();
val table = getIntTable(num);
val list = map transtion table;
val maxlist = parendist(list,nil);
val maxsub = ListSequence.argmax Int.compare maxlist;
printInt(ListSequence.nth maxlist maxsub);