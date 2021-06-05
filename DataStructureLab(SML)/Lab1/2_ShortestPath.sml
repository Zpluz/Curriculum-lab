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

type vfo = int * int
val inf:int = 0xfffffff;
val nov = getInt();
val noe = getInt();
val s = getInt();

fun init ((admatrix:int Array2.array),~1,~1) = admatrix
    | init ((admatrix:int Array2.array),(x:int),(y:int)) =               (*将矩阵的对角线清零*)
        let val t = Array2.update(admatrix,x,y,0)
        in init(admatrix,x-1,y-1) end;


fun fillmatrix (admatrix,0) = admatrix                   (*将邻边信息填入矩阵中*)
    |fillmatrix ((admatrix:int Array2.array),noe:int) =
    let val x = getInt();
        val y = getInt();
        val dis = getInt();
        val checkdis = if dis > Array2.sub(admatrix,x-1,y-1) then Array2.sub(admatrix,x-1,y-1) else dis
        val m = Array2.update(admatrix,x-1,y-1,checkdis)
        val n = Array2.update(admatrix,y-1,x-1,checkdis)
    in  fillmatrix(admatrix,noe-1) end;

fun checkinclude(nil,target) = true              (*检查当前点是否已被纳入云中，若纳入云中则返回false,未纳入则返回true*)
    |checkinclude ((hd::includenode:int list),(target:int)) = 
        if target = hd then false
        else checkinclude(includenode,target);

fun minofrow((includenode:int list),rminusone,admatrix,min,~1,sub) = let val minfo:vfo = (sub,min) in minfo end           (*给定行数找到对应行中的最小数，即找到与该行数对应点相邻的最短路*)
    |minofrow ((includenode:int list),(rminusone:int),(admatrix:int Array2.array),(min:int),(col:int),(sub:int)) =
        if Array2.sub(admatrix,rminusone,col) < min  andalso checkinclude(includenode,col)
            then minofrow(includenode,rminusone,admatrix,(Array2.sub(admatrix,rminusone,col)),col-1,col)
        else minofrow(includenode,rminusone,admatrix,min,col-1,sub);

fun minclude(admatrix,nil,innode,disfs,min,col,subcol,sub) = let val addnode:vfo = (sub,min) in  (sub,if min < Array.sub(disfs,sub) then Array.update(disfs,sub,min) else Array.update(disfs,sub,Array.sub(disfs,sub))) end
    |minclude((admatrix:int Array2.array),(hd::includenode:int list),(innode:int list),(disfs:int array),(min:int),(col:int),(subcol:int),(sub:int)) =  
        let val minofonerow = minofrow(innode,hd,admatrix,min,col,subcol) 
        in
            if #2 minofonerow + Array.sub(disfs,hd) < min 
                then minclude(admatrix,includenode,innode,disfs,(#2 minofonerow) + Array.sub(disfs,hd),col,subcol,#1 minofonerow)
            else minclude(admatrix,includenode,innode,disfs,min,col,subcol,sub)
        end;

fun dijkstra(admatrix,includenode,disfs,novminusone,0) = includenode
    |dijkstra((admatrix:int Array2.array),(includenode:int list),(disfs:int array),(novminusone:int),(record:int)) = 
    let val intermedia = minclude(admatrix,includenode,includenode,disfs,inf,novminusone,novminusone,0)
        val newnode = #1 intermedia
    in dijkstra(admatrix,newnode::includenode,disfs,novminusone,record-1) end;

fun  checkresult(arr,~1) = arr
    |checkresult((arr:int array),sub) = 
    if Array.sub(arr,sub) = inf then let val a = Array.update(arr,sub,~1) in checkresult(arr,sub-1) end
    else checkresult(arr,sub-1);

val disfs = Array.array(nov,inf);
val a = Array.update(disfs,s-1,0);
val includenode:int list = [s-1];
val inimatrix:int Array2.array = Array2.array(nov,nov,inf);
val intermatrix = init(inimatrix,nov-1,nov-1);
val admatrix = fillmatrix(intermatrix,noe);
val preresult = dijkstra(admatrix,includenode,disfs,nov-1,nov-1);
val result = checkresult(disfs,nov-1);
val b = Array.update(disfs,s-1,0);
printArray(disfs);