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

val a = IntInf.fromInt(0)
fun exchange (nil,_,_) = a::[]
    | exchange (([x]:IntInf.int list),(key:IntInf.int),(mplist:IntInf.int list)):IntInf.int list = key::x::mplist
    | exchange((x::xm::rl:IntInf.int list),(key:IntInf.int),(mplist:IntInf.int list)) = 
        if xm>key then x::exchange(x::rl,key,mplist)
        else (key::xm::rl)@(x::mplist);


val num = getIntInf();
val primitivetb = getIntInfTable(num);
fun sucpermute(_,nil) = a::[]
    |sucpermute((templist:IntInf.int list),(key::mplist:IntInf.int list)):IntInf.int list  = 
    if hd templist <= key then sucpermute(key::templist,mplist)
    else exchange(templist,key,mplist);
val oritem =  hd primitivetb::[];
val mplist = List.drop(primitivetb,1);
val result = sucpermute(oritem,mplist);
printIntInfTable(result);
