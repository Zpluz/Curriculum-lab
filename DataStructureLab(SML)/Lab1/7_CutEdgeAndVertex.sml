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
        printEndOfLine();
        rcur := !rcur + 1)
    end;


val nov:int = getInt();
val noe:int = getInt();
val inf:int = 0xfffffff;
val dfsn:int array = Array.array(nov,inf);
val lown:int array = Array.array(nov,inf);
val parent:int array = Array.array(nov,inf);
val dfnarr:int ref = ref ~1;
val children = ref 0
val visited:int array = Array.array(nov,inf); (*用来记录下标对应的点是否已经访问*)

val admatrix:int Array2.array = Array2.array(nov,nov,inf);

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

fun fillmatrix (admatrix,0) = admatrix                   (*将邻边信息填入矩阵中*)
    |fillmatrix ((admatrix:int Array2.array),noe:int) =
    let val x = getInt();
        val y = getInt();
        val m = Array2.update(admatrix,x-1,y-1,1)
        val n = Array2.update(admatrix,y-1,x-1,1)
    in  fillmatrix(admatrix,noe-1) end;

val d = fillmatrix(admatrix,noe);

fun findsons(row,~1,sons) = sons
    |findsons ((row:int),(col:int),(sons:int list)) = 
    if Array2.sub(admatrix,row,col) = 1 then findsons(row,col-1,col::sons)
    else findsons(row,col-1,sons);

fun getsource(row:int) = 
    if row < nov-1 andalso findsons(row,nov-1,[]) = [] then getsource(row+1)
    else if row < nov-1 andalso findsons(row,nov-1,[]) <> [] then row
    else ~1;

val source = getsource(0);

fun updateparents(p,[]) = ()
    |updateparents((p:int),(s::sons:int list)) = 
    let val m = Array.update(parent,s,p)
    in updateparents(p,sons) end;

fun getconnect(v,connectdfn,connect,~1) = (quicksort(connectdfn),connect)
    |getconnect((v:int),(connectdfn:int list),(connect:int list),(sub:int))  = (*获取与v相连的所有点的dfn值，并从小到大排序*)
    if Array2.sub(admatrix,v,sub) = 1 then getconnect(v,Array.sub(dfsn,sub)::connectdfn,sub::connect,sub-1)
    else getconnect(v,connectdfn,connect,sub-1);

fun searchdfsn(minconnectdfn,[]) = inf
    |searchdfsn((minconnectdfn:int),(c::connect:int list)) = 
    if minconnectdfn = Array.sub(dfsn,c)  then c
    else searchdfsn(minconnectdfn,connect);

fun searchdfsfather(v,[],connect) = source (*若找不到满足下列情况的点，只能是起点，因为将起点的父亲定义为起点，否则若将起点的父亲定义为nov以外的其他值，会引起下标越界*)
    |searchdfsfather((v:int),(cd::connectdfn:int list),(connect:int list)) = (*找到v在dfs树中的父节点*)
    if cd < Array.sub(dfsn,v) then searchdfsn(cd,connect)
    else searchdfsfather(v,connectdfn,connect);

fun updatelown((v:int),(vp:int),(clown:int)) = (*对v的父亲节点以及祖先节点更新low值*)
        if Array.sub(lown,vp) > clown andalso v <> Array.sub(parent,vp) then 
            let val b = Array.update(lown,vp,clown)
            in updatelown(vp,Array.sub(parent,vp),clown) end
        else if Array.sub(lown,vp) > clown andalso v = Array.sub(parent,vp) then
            let 
                val conpair = getconnect(vp,nil,nil,nov-1)
                val con1 = List.rev(#1 conpair)
                val con2 = #2 conpair
                val y = searchdfsfather(vp,con1,con2)
                val t = Array.update(parent,vp,y)
                val v = Array.update(lown,vp,clown)
            in updatelown(vp,Array.sub(parent,vp),clown) end
        else ();

fun dfs ([],_) = ()
    |dfs ((v::vs:int list),(dfn:int ref)) = 
        if Array.sub(visited,v) = 1 andalso v <> Array.sub(parent,Array.sub(parent,v)) then
            let 
                val clown = Array.sub(dfsn,v)
                val conpair = getconnect(v,nil,nil,nov-1)
                val con1 = List.rev(#1 conpair)
                val con2 = #2 conpair
                val y = searchdfsfather(v,con1,con2)
                val f = updatelown(v,Array.sub(parent,v),clown)         (*将其父亲节点以及祖先节点的low值一次性更新*)
                val t = Array.update(parent,v,y)
            in 
                dfs(vs,dfn)
            end
        else if Array.sub(visited,v) = 1 andalso v = Array.sub(parent,Array.sub(parent,v)) then 
            let 
                val conpair = getconnect(v,nil,nil,nov-1)
                val con1 = List.rev(#1 conpair)
                val con2 = #2 conpair
                val h = searchdfsfather(v,con1,con2)
                val r = Array.update(parent,v,h)
            in 
                dfs(vs,dfn) 
            end
        else 
            let 
                val m = if 1 > 0 then dfn := !dfn + 1 else dfn := !dfn
                val kid = if Array.sub(parent,v) = source then children := !children + 1 else children := !children
                val k = Array.update(visited,v,1)
                val l = Array.update(dfsn,v,!dfn)
                val c = Array.update(lown,v,!dfn)
                val sons = findsons(v,nov-1,nil)
                val pc = updateparents(v,sons)
                val dfsons = dfs(sons,dfn)
            in 
                dfs(vs,dfn)
            end;
if source = ~1 then  
    let 
        val iov = printInt(0)
        val ioe = printInt(0)
    in 
        ()
    end
else 
let val dfsresult = dfs([source],dfnarr);
    fun checkcutv(v,[],nocv) = nocv
        |checkcutv((v:int),(c::cov:int list),(nocv:int)) = 
        if  v <> source andalso v = Array.sub(parent,c)  andalso   (*此处要求v是u在dfs树中的子节点，在parent数组中筛选*)
        (Array.sub(lown,c) > Array.sub(dfsn,v) orelse Array.sub(lown,c) = Array.sub(dfsn,v)) then 1
        else if v = source andalso !children > 1 then 1
        else checkcutv(v,cov,nocv);
    fun checkcute(v,[],noce) = noce
        |checkcute((v:int),(c::cov:int list),(noce:int)) = 
        if Array.sub(lown,c) > Array.sub(dfsn,v) then checkcute(v,cov,noce+1)
        else checkcute(v,cov,noce)

    fun getcutvane(~1,vn,en) = (vn,en)
        |getcutvane((row:int),(vn:int),(en:int)) = 
        let val conpair = getconnect(row,nil,nil,nov-1)
            val conr = #2 conpair
            val vnor = vn+checkcutv(row,conr,0)
            val enor = en+checkcute(row,conr,0)
        in
            getcutvane(row-1,vnor,enor) end;
        
    val cutresult = getcutvane(nov-1,0,0);
    val cutvn = #1 cutresult;
    val cuten = #2 cutresult;
    val iov = printInt(cutvn)
    val ioe = printInt(cuten)
in
    ()
end;