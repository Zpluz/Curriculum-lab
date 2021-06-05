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


(*当 p为素数 且 p>2 时，x^2 mod p与1同余则 x 必与1或-1 mod p 同余。*)
(*当 p为素数，则p-1作为偶数可以分解为 2^s*r *)
(*由Fermat's a^p-1 与 1 mod p 同余，则p是素数*)

val ip:IntInf.int = getIntInf();

fun double (b:IntInf.int) = b * IntInf.fromInt(2);
fun half (b:IntInf.int) = b div IntInf.fromInt(2);

fun separate (r:IntInf.int,(s:int)) = 
    if  r mod 2 = 0 then separate(half(r),s+1)
    else (r,s);
    
fun powermod((a:IntInf.int),(n:IntInf.int),m:IntInf.int):IntInf.int = 
    if n = 1 then a mod m
    else if n mod 2 = 0 then let val intermedia = powermod(a,half(n),m) in (intermedia*intermedia) mod m end
    else (a * powermod(a,n-1,m)) mod m;

fun MBPtest ((a:IntInf.int),(e:IntInf.int),(k:int),(s:int)) = 
    if  powermod(a,e,ip) <> ip-1 andalso k<s then MBPtest(a,double(e),k+1,s)
    else if powermod(a,e,ip) = ip-1 then 1
    else 0;

fun comeout (a) = 
    if ip = 2 then 1
    else if ip <> 2 andalso ip mod 2 = 0 then 0
    else 
        let val t = separate(ip-1,0);
            val c = MBPtest(a,#1 t,0,#2 t);
        in 
            if powermod(a,#1 t,ip) = 1 orelse c = 1 then 1
            else 0
        end;

val alist:IntInf.int list= [2,3,5,7,11,13,17,19,23,29,31,33,37,41];
val resultlist = map comeout alist;
fun checkresult(nil) = 1
    |checkresult (x::xs:int list) = 
    if x = 1 then checkresult(xs)
    else 0;

val checkpoint = checkresult(resultlist);
if checkpoint = 1 then printString("True")
else printString("False");