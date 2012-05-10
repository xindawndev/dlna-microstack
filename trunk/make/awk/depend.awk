#!/bin/awk -f 
BEGIN { FS = ":"; RS = "!";}
{
   sub(/[[:blank:]]*$/,"",$1)
   sub(/[[:blank:]]*$/,"",$2)
   sub(/[[:blank:]]*$/,"",$3)
   sub(/[[:blank:]]*$/,"",$4)
   sub(/^[[:blank:]]*/,"",$1)
   sub(/^[[:blank:]]$/,"",$2)
   sub(/^[[:blank:]]$/,"",$3)
   sub(/^[[:blank:]]$/,"",$4) 
   if(1 == NR)
   {
   	root=$1
   }
   param1 = sprintf("%s/%s",$1,$2);
   gsub(/[[:blank:]]*/,"",param1)	
   lib[$1] = param1
   
   
   if(0 < index($2,".so") || 0 < index($2,".dll"))
   {
       type[$1]=1
   }
   else if(0 < index($2,".a"))
   {
       type[$1]=0
   }
   else
   {
       type[$1]=2
   }
   
  # print $1,type[$1]  
 
   if(1 < length($3))
   {
      depend[$1] =$3
   }
	
	
  # print $1,type[$1]

   if(1 < length($4))
   {
      syslib[$1] = $4
   }
}
function GetArray(x,a)
{
return split(x,a," ")
}
END{	
    i = 0
    name[root] = i
    num[i] = root
    temp[root]
    temp1[root]
    a[root]
    dog = 0	
    while(1)
    {
	delete temp1
        if(1 > length(temp))
        {
            break
        }
 
	for(jj in temp)
        {
	 if(jj == root || (0 == type[jj]) ){
	   delete a
	  
	   if(1 > length(depend[jj])) continue
           ii = GetArray(depend[jj],a)
	   if(ii > 0)
           {
		for(kk in a)
                {
		    temp1[a[kk]]
		}
	   }
	 }
	 else
	 {
	     if(jj != root)
	     {
		delete syslib[jj]		
 	     }
	 }
        }

	delete temp
	for(jj in temp1)
	{
	    temp[jj]
	}
	
	for(kk in temp)
	{	
	    i++
	    if(kk in name)
	    {
		delete num[name[kk]]
	    }	
	    name[kk] = i
            num[i] = kk
	}
       
    }	    
    for(mm in syslib)
    {
	if(! (mm in name)) delete syslib[mm]
    }
  

 
    for(mm in syslib)
    {
        delete a
        ii = GetArray(syslib[mm],a)
	if(ii > 0)
	{
	    for(ll in a) tema[a[ll]]
	}
    }	

    delete num[name[root]]
    
    for(ll in num)
    {
        if(0 == type[num[ll]])
	{
	    stalib[ll] = lib[num[ll]]
	}
	else if(1 == type[num[ll]])
	{
	    dynlib[ll] = lib[num[ll]]
	}
	else
	{
	#	print num[ll]
	}	
    }    
    
   #for(i in stalib) print i,stalib[i]
   for(i=1; i < 200; ++i)
   {
       if(0 < length(stalib[i])) print stalib[i]
   }
   for(j in dynlib) print dynlib[j]
   for(k in tema) print k

}
