program laplsolv

	use omp_lib

	!-----------------------------------------------------------------------
	! Serial program for solving the heat conduction problem 
	! on a square using the Jacobi method. 
	! Written by Fredrik Berntsson (frber@math.liu.se) March 2003
	! Modified by Berkant Savas (besav@math.liu.se) April 2006
	!-----------------------------------------------------------------------
	integer, parameter                  :: n=1000, maxiter=1000, nt=8
	double precision,parameter          :: tol=1.0E-3
	double precision,dimension(0:n+1,0:n+1) :: T
	double precision,dimension(0:n-1,0:nt-1)   :: pBefore,pAfter
	double precision,dimension(n)       :: tmp1
	double precision                    :: error,x
	real                                :: t1,t0
	integer                             :: i,j,k,tid,partitionssize,rest,offset
	integer,dimension(nt)				:: partitions,startIndex,stopIndex
	character(len=20)                   :: str



	! Set boundary conditions and initial values for the unknowns
	T=0.0D0
	T(0:n+1 , 0)     = 1.0D0
	T(0:n+1 , n+1)   = 1.0D0
	T(n+1   , 0:n+1) = 2.0D0


	!Set startindex and stopindex
	rest = mod(n,nt)
	partitionssize = n/nt

	offset=0

	do i=0,nt-1

		if(i<rest) then
			partitions(i) = partitionssize+1
		else 
			partitions(i) = partitionssize
		end if

		startIndex(i) = offset + 1
		offset = offset + partitions(i)
		stopIndex(i) = offset

	end do


	! Solve the linear system of equations using the Jacobi method
	t0 = omp_get_wtime()


	do k=1,maxiter

		error=0.0D0

		!Set padding rows (after,before)
		do i=0,nt-1
			pAfter(0:n-1, i) = T(1:n, stopIndex(i)+1)
			pBefore(0:n-1, i) = T(1:n, startIndex(i)-1)
		end do

		call omp_set_num_threads(nt)

		!$omp parallel private(j,tid,tmp1) shared(T,pAfter,pBefore) reduction(max: error)
		tid = OMP_GET_THREAD_NUM()

		do j=startIndex(tid),stopIndex(tid)-1
			tmp1=T(1:n,j)
			T(1:n,j)=(T(0:n-1,j)+T(2:n+1,j)+T(1:n,j+1)+pBefore(0:n-1,tid))/4.0D0
			error=max(error,maxval(abs(tmp1-T(1:n,j))))
			pBefore(0:n-1,tid)=tmp1
		end do

		tmp1=T(1:n,stopIndex(tid))
		T(1:n,stopIndex(tid)) = (T(0:n-1,stopIndex(tid))+T(2:n+1,stopIndex(tid))+pAfter(0:n-1,tid)+pBefore(0:n-1,tid))/4.0D0
		error=max(error,maxval(abs(tmp1-T(1:n,stopIndex(tid)))))

		!$omp end parallel

		if (error<tol) then
			exit
		end if

	end do

	t1 = omp_get_wtime()



	write(unit=*,fmt=*) 'Time:',t1-t0,'Number of Iterations:',k
	write(unit=*,fmt=*) 'Temperature of element T(1,1)  =',T(1,1)

	! Uncomment the next part if you want to write the whole solution
	! to a file. Useful for plotting. 

	!open(unit=7,action='write',file='result.dat',status='unknown')
	!write(unit=str,fmt='(a,i6,a)') '(',N,'F10.6)'
	!do i=0,n+1
	!   write (unit=7,fmt=str) T(i,0:n+1)  
	!end do
	!close(unit=7)

end program laplsolv
