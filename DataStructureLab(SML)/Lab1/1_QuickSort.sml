fun printInt (a:int) =
    print(Int.toString(a)^" ");

fun getInt () =
    Option.valOf (TextIO.scanStream (Int.scan StringCvt.DEC) TextIO.stdIn);

fun printIntTable ( [] ) = ()
  | printIntTable ( x::xs ) = 
    let
	val tmp = printInt(x)
    in
	printIntTable(xs)
    end;

fun getIntTable ( 0 ) = []
  | getIntTable ( N:int) = getInt()::getIntTable(N-1);



fun quicksort [] = []
    | quicksort [p]=[p]
    | quicksort (p::ps) = 
        let  
        	fun separation ((llist:int list),(rlist:int list),nil) = (quicksort llist)@(p::(quicksort rlist))
            	| separation ((llist:int list),(rlist:int list),(r::rs):int list) = 
             	if  r>p then separation(llist,r::rlist,rs)
             	else separation(r::llist,rlist,rs);
        in separation(nil,nil,ps)
		end;

val length = getInt();
val list = getIntTable(length);
val result = quicksort(list);
printIntTable(result);