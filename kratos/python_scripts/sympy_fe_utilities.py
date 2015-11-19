from sympy import *

def DefineMatrix( name, m,n ):
    return Matrix( m,n, lambda i,j: var(name+'_%d_%d' % (i,j)) )

def DefineSymmetricMatrix( name, m,n ):
    tmp = DefineMatrix(name,m,n)

    #impose symm
    for i in range(0,tmp.shape[0]):
        for j in range(i+1,tmp.shape[1]):
            tmp[j,i] = tmp[i,j]
   
    return tmp

def DefineVector( name, m):
    return Matrix( m,1, lambda i,j: var(name+'_%d' % (i)) )

###note that partition of unity is imposed
###the name HAS TO BE --> N and DN
def DefineShapeFunctions(nnodes, dim, impose_partion_of_unity=False):
    DN = DefineMatrix('DN',nnodes,dim)
    N = DefineVector('N',nnodes)
    
    #impose partition of unity
    if(impose_partion_of_unity == True):
        N[nnodes-1] = 1
        for i in range(0,nnodes-1):
            N[nnodes-1] -= N[i]
            
        DN[nnodes-1,:] = -DN[0,:]
        for i in range(1,nnodes-1):
            DN[nnodes-1,:] -= DN[i,:]
    
    return N,DN
    
    
def StrainToVoigt(M):
    #print(M.shape)
    if(M.shape[0] == 2):
        vm = Matrix( 3,1, zeros(3,1))
        vm[0,0] = M[0,0]
        vm[1,0] = M[1,1]
        vm[2,0] = 2.0*M[0,1]
    elif(M.shape[0] == 3):
        raise Exception("not implemented yet")
    return vm

def MatrixB(DN):
    dim = DN.shape[1] 
    if(dim == 2):
        strain_size = 3
        nnodes = DN.shape[0]
        B = Matrix( zeros(strain_size, nnodes*dim) )
        for i in range(0,nnodes):
            for k in range(0,dim):
                B[0,i*dim] = DN[i,0]; B[0, i*dim+1] = 0;
                B[1,i*dim] = 0;       B[1, i*dim+1] = DN[i,1];
                B[2,i*dim] = DN[i,1]; B[2, i*dim+1] = DN[i,0];
        return B
    elif(dim == 3):
        strain_size = 6
        nnodes = DN.shape[0]
        B = Matrix( zeros(strain_size, nnodes*dim) )
        for i in range(0,nnodes):
            for k in range(0,dim):
                B[0,i*dim] = DN[i,0]; 
                B[1, i*dim+1] = DN[i,1];
                B[2, i*dim+2] = DN[i,2];
                
                #TODO: revise the next three lines
                B[3,i*dim] = DN[i,1]; B[3, i*dim+1] = DN[i,0]; #01
                B[4,i*dim] = DN[i,2]; B[4, i*dim+2] = DN[i,0]; #02
                B[5,i*dim+1] = DN[i,2]; B[5, i*dim+2] = DN[i,1]; #12
        #raise Exception("B not implemented yet for 3D")
    
def grad_sym_voigtform(DN, x):
    dim = DN.shape[1]
    nnodes = DN.shape[0]
    print(nnodes, dim, x.shape)

    B = MatrixB(DN)
    
    #put the x components one after the other in a vector
    xvec = Matrix( zeros(B.shape[1], 1 ) );    
    for i in range(0,nnodes):
        for k in range(0,dim):
            xvec[i*dim+k] = x[i,k]
    
    return simplify( B*xvec )

def grad(DN,x):
    return simplify(DN.transpose()*x)

def div(DN,x):
    if(DN.shape != x.shape):
        raise Exception("shapes are not compatible")
    
    div_x = 0
    for i in range(0,DN.shape[0]):
        for k in range(0,DN.shape[1]):
            div_x += DN[i,k]*x[i,k]
    
    return Matrix( [ simplify(div_x) ])
    

def OutputRHS(r, mode="python", initial_tabs = 3,max_index=10):
    initial_spaces = str("")
    for i in range(0,initial_tabs):
        initial_spaces += str("    ")
    
    outstring = str("")
    for i in range(0,r.shape[0]):
        
        if(mode == "python"):
            outstring += initial_spaces+str("rhs[")+str(i)+str("]=")+str(r[i,0])+str("\n")
        elif(mode=="c"):
            outstring += initial_spaces+str("rhs[")+str(i)+str("]=")+ccode(r[i,0])+str(";\n")
            
    #matrix entries (two indices)
    for i in range(0,max_index):
        for j in range(0,max_index):
            if(mode == "python"):
                replacement_string = str("[")+str(i)+str(",")+str(j)+str("]")
            elif(mode=="c"): 
                replacement_string = str("(")+str(i)+str(",")+str(j)+str(")")
            to_be_replaced  = str("_")+str(i)+str("_")+str(j)
            newstring = outstring.replace(to_be_replaced, replacement_string)
            outstring = newstring
            
    #vector entries(one index(
    for i in range(0,max_index):
        replacement_string = str("[")+str(i)+str("]")
        to_be_replaced  = str("_")+str(i)
        newstring = outstring.replace(to_be_replaced, replacement_string)
        outstring = newstring
            
    return outstring
    

def OutputLHS(lhs,mode, initial_tabs = 3, max_index=10):
    initial_spaces = str("")
    for i in range(0,initial_tabs):
        initial_spaces += str("    ")
    outstring = str("")
    for i in range(0,lhs.shape[0]):
        for j in range(0,lhs.shape[1]):
            if(mode == "python"):
                outstring += initial_spaces+str("lhs[")+str(i)+str(",")+str(j)+str("]=")+str(lhs[i,j])+str("\n")
            elif(mode=="c"):
                outstring += initial_spaces+str("lhs(")+str(i)+str(",")+str(j)+str(")=")+ccode(lhs[i,j])+str(";\n")
    
    #matrix entries (two indices)
    for i in range(0,max_index):
        for j in range(0,max_index):
            if(mode == "python"):
                replacement_string = str("[")+str(i)+str(",")+str(j)+str("]")
            elif(mode=="c"): 
                replacement_string = str("(")+str(i)+str(",")+str(j)+str(")")
            to_be_replaced  = str("_")+str(i)+str("_")+str(j)
            newstring = outstring.replace(to_be_replaced, replacement_string)
            outstring = newstring
            
    #vector entries(one index(
    for i in range(0,max_index):
        replacement_string = str("[")+str(i)+str("]")
        to_be_replaced  = str("_")+str(i)
        newstring = outstring.replace(to_be_replaced, replacement_string)
        outstring = newstring
            
    return outstring
    