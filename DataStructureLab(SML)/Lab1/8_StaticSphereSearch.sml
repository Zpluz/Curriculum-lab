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
fun printArrayRow (Arr:int Array2.array,k:int) = 
    let
	val ccur = ref 0
	val col = Array2.nCols(Arr)
    in
	while !ccur < col
	do
	(
        printInt(Array2.sub(Arr,k,!ccur));
        printEndOfLine;
        ccur := !ccur + 1)
    end;

fun printArrayMatrix ( Arr:int Array2.array ) =
    let
	val rcur = ref 0
	val row = Array2.nRows(Arr)
    in
	while (!rcur < row)
	do
	(
        printArrayRow(Arr,!rcur);
        rcur := !rcur + 1)
    end;


type interval = int * int

val n = getInt();
val m = getInt();

fun genarr(arr,~1) = arr
    |genarr ((arr:int array),(sub:int)) = 
    let 
        val k = getInt()
        val u = Array.update(arr,(n-1)-sub,k);
    in genarr(arr,sub-1) end;

val iniarr:int array = Array.array(n,0);
val arr = genarr(iniarr,n-1);
val ininterval = (0,0)
val inintervalarr:interval array= Array.array(m,ininterval);

fun genintervalarr(intervalarr,~1) = intervalarr
    |genintervalarr ((intervalarr:interval array),(sub:int)) = 
    let 
        val subl = getInt()-1;
        val subr = getInt();
        val u = Array.update(intervalarr,(m-1)-sub,(subl,subr))
    in genintervalarr(intervalarr,sub-1) end;

val intervalarr:interval array = genintervalarr(inintervalarr,m-1);

fun maximum ((subr:int),(sub:int),(maxV:int)) = 
    if sub < subr andalso Array.sub(arr,sub) > maxV then maximum(subr,sub+1,Array.sub(arr,sub))
    else if sub < subr andalso (Array.sub(arr,sub) < maxV orelse Array.sub(arr,sub) = maxV) then maximum(subr,sub+1,maxV)
    else maxV;

fun genmaxarr(intervalarr,maxarr,~1) = maxarr
    |genmaxarr ((intervalarr:interval array),(maxarr:int array),(sub:int)) = 
        let val i = Array.sub(intervalarr,sub)
            val l = #1 i
            val r = #2 i
            val m = maximum(r,l,0)
            val u = Array.update(maxarr,sub,m)
        in genmaxarr(intervalarr,maxarr,sub-1) end;

val inimaxarr = Array.array(m,0);
val resultarr = genmaxarr(intervalarr,inimaxarr,m-1);
printArray(resultarr);