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

val num=getInt();
val checktime=getInt();
val ordVec= getIntVector(num);
val checklist:int list = getIntTable(checktime);

fun binsearch ((ordVec:int vector),(lo:int),(hi:int),(key:int)) = 
    if lo <= hi andalso key = Vector.sub(ordVec,(hi+lo) div 2) then (hi+lo) div 2
    else if lo <=hi andalso key > Vector.sub(ordVec,(hi+lo) div 2) then binsearch(ordVec,((hi+lo) div 2) + 1,hi,key)
    else if lo <=hi andalso key < Vector.sub(ordVec,(hi+lo) div 2) then binsearch(ordVec,lo,((hi+lo) div 2) - 1, key)
    else ~1;

fun oversearch (key:int) = binsearch(ordVec,0,num-1,key);

val resultTb = map oversearch checklist;

printIntTable(resultTb);
