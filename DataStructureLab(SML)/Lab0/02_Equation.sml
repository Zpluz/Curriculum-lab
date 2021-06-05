fun printInt (a:int) = 
    print(Int.toString(a)^" ");
fun printReal (a:real) = 
    print(Real.toString(a)^" ");
fun printString(a:string) =
    print(a^" ");

fun getInt () =
    Option.valOf (TextIO.scanStream(Int.scan StringCvt.DEC) TextIO.stdIn);
fun getReal () =
    Option.valOf (TextIO.scanStream(Real.scan) TextIO.stdIn);
 type vector = int * int;

 fun det((v1:vector), (v2:vector)):int =
    (#1 v1 * #2 v2 - #1 v2 * #2 v1);

fun calcus (v1,v2,v) = Real.fromInt(det(v,v2))/Real.fromInt(det(v1,v2));
val a=getInt();
val b=getInt();
val k1=getInt();
val c=getInt();
val d=getInt();
val k2=getInt();
val v1 = (a , c) ;
val v2 = (b , d) ;
val v  = (k1 , k2) ;
if det(v1,v2)=0 then printString("No Solution")
else  printInt(floor(calcus(v1,v2,v)));
      

