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

fun getIntList ( 0 ) = []
  | getIntList ( N:int) = getInt()::getIntList(N-1);

fun getIntInfTable ( 0 ) = []
  | getIntInfTable ( N:int) = getIntInf()::getIntInfTable(N-1);

fun getIntVector ( 0 ) =  Vector.fromList []
  | getIntVector ( N:int) = Vector.fromList(getIntList(N));

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


fun getIntList(0,list) = list
    |getIntList((N:int),(list:int list)) = getIntList(N-1,getInt()::list);

fun append([],result) = result
    |append((fnb::fnbs:int list),(result:int list)) = 
    append(fnbs,fnb::result);
    
fun fillzeros((m:int list),(sub:int)) = 
    if sub > 0 then fillzeros(0::m,sub-1)
    else m;

fun checkzeros((r::result:int list),(save:int list)) = 
    if r = 0 andalso List.length(result) > 0 then checkzeros(result,result)
    else save

val n1 = getInt();
val fnb = if n1 = 0 then [0] else getIntList(n1,nil);
val n2 = getInt();
val snb = if n2 = 0 then [0] else getIntList(n2,nil);

fun plus ((fnb::fnbs:int list),(snb::snbs:int list),(carry:int),(result:int list)) = plus(fnbs,snbs,(fnb+snb+carry) div 10, ((fnb+snb+carry) mod 10 )::result)
    |plus(fnb::fnbs,[],carry,result) = if (fnb+carry) < 10 then let val k = fnb+carry in append(fnbs,(k::result)) end else plus(fnbs,[],(fnb+carry) div 10,((fnb+carry) mod 10)::result)
    |plus([],[],carry,result) = if carry > 0 then carry::result else result;

val plusresult = plus(fnb,snb,0,nil);

fun minus((fnb::fnbs:int list),(snb::snbs:int list),(carry:int),(result:int list)) = 
    if (fnb+carry)<snb then minus(fnbs,snbs,~1,(fnb-snb+carry+10)::result)
    else  minus(fnbs,snbs,0,(fnb-snb+carry)::result)
    | minus (fnb::fnbs,[],carry,result) = if fnb+carry = 0 then result else let val k = fnb+carry in append(fnbs,k::result) end
    | minus([],[],carry,result) = result

val minusresult = minus(fnb,snb,0,nil);

fun bitmultiply([],snb,carry,result) = if carry > 0 then carry::result else result
    |bitmultiply((fnb::fnbs:int list),(snb:int),(carry:int),(result:int list)) = 
    bitmultiply(fnbs,snb,(fnb*snb+carry) div 10,((fnb*snb+carry) mod 10)::result);


fun multiply(snb::snbs,[]) = multiply(snbs,bitmultiply(fnb,snb,0,nil))
    |multiply([],result) = result
    |multiply((snb::snbs:int list),(result:int list)) = 
    let val m = bitmultiply(fnb,snb,0,nil)
    in multiply(snbs,plus(0::List.rev(result),List.rev(m),0,nil)) end;
        
    
val multiplyresult = multiplyresult(List.rev(snb),nil);

printIntTable(checkzeros(plusresult,plusresult));
printEndOfLine();
printIntTable(checkzeros(minusresult,minusresult));
printEndOfLine();
printIntTable(checkzeros(multiplyresult,multiplyresult));