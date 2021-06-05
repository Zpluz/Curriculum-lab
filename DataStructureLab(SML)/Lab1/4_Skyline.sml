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

type coordinate = int * int * int
fun printEndOfLine () =
    print("\n");
fun cmpc((c1:coordinate),(c2:coordinate)) =  (*定义坐标优先级，横坐标升序排列，横坐标相同，按h降序排列*)
    if #1 c1 <> #1 c2 then 
        if #1 c1 > #1 c2 then GREATER
        else LESS
    else 
        if #2 c1 > #2 c2 then LESS
        else if #2 c1 = #2 c2 then EQUAL
        else GREATER

fun cmph ((h1:int),(h2:int)) = 
    if h1<h2 then GREATER
    else if h1 = h2 then EQUAL
    else LESS


fun printCoordinate([]) = ()
    |printCoordinate (co::colist:coordinate list) = 
    let 
        val m = printInt(#1 co)
        val n = printInt(#2 co)
        val p = printEndOfLine()
    in printCoordinate(colist) end;

val amount = getInt();

fun classify(0,bs) = bs
    |classify ((amount:int),(bs:coordinate list)) = 
    let val l = getInt()
        val h = getInt()
        val r = getInt()
        val lc = (l,h,0)
        val rc = (r,h,1)            (*0代表是l端点,1是代表r端点*)
    in 
        classify(amount-1,lc::rc::bs)
    end;

val ibsi = classify(amount,nil);
val ubsi = ArraySequence.fromList(ibsi);
val kbsi = (ArraySequence.sort cmpc) ubsi;
val bsi = ArraySequence.toList(kbsi);
val olist:(coordinate list) = [];
val highlist = [0];

fun scanline([],[0],_,olist) = olist  (*扫描线算法*)
    |scanline((b::bsis:coordinate list),(highlist:int list),(himax:int),(olist:coordinate list)) = 
    if #3 b = 0 then 
        let val h = #2 b
            val nhli = ArraySequence.fromList(h::highlist)
            val nhlk = (ArraySequence.sort cmph) nhli
            val nhl = ArraySequence.toList(nhlk)
        in
            if h > himax then scanline(bsis,nhl,h,b::olist)
            else scanline(bsis,nhl,himax,olist)
        end
    else 
        let fun remove(h::highlist) = if h = #2 b then []@highlist else h::remove(highlist)
            val nhl = remove highlist
            val nhd = List.hd(nhl)
        in 
            if nhd <> himax then let val nb:coordinate=((#1 b),nhd,1) in scanline(bsis,nhl,nhd,nb::olist) end
            else scanline(bsis,nhl,himax,olist)
        end;
val coolist = List.rev(scanline(bsi,highlist,0,olist));
printCoordinate(coolist);