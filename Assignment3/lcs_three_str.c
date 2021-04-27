// suffixCompare, computeRank, computeLCP these three functions were taken from http://ranger.uta.edu/~weems/NOTES5311/LAB/LAB3SPR21/SAcommonSubStr2.trace.c
// Written by Bob P Wheems

// Default gcc compiler(@omega.uta.edu) will be able to compile this code

// Use suffix array and LCP to compute
// longest common substring of three input strings.
// This version outputs multiple candidate solutions.


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char s[1000000],s1[500000],s2[500000],s3[500000];
int n,           // length of s[] including \0
sa[1000000],   // suffix array for s[]
rank[1000000], // rank[i] gives the rank (subscript) of s[i] in sa[]
lcp[1000000];  // lcp[i] indicates the number of identical prefix symbols
                 // for s[sa[i-1]] and s[sa[i]]

int suffixCompare(const void *xVoidPt,const void *yVoidPt)
{
	// Used in qsort call to generate suffix array.
	int *xPt=(int*) xVoidPt,*yPt=(int*) yVoidPt;
	
	return strcmp(&s[*xPt],&s[*yPt]);
}
	
void computeRank()
{
	// Computes rank as the inverse permutation of the suffix array
	int i;
	
	for (i=0;i<n;i++)
	  rank[sa[i]]=i;
}

void computeLCP()
{
//Kasai et al linear-time construction
	int h,i,j,k;

	h=0;  // Index to support result that lcp[rank[i]]>=lcp[rank[i-1]]-1
	for (i=0;i<n;i++)
	{
	  k=rank[i];
	  if (k==0)
	    lcp[k]=(-1);
	  else
	  {
	    j=sa[k-1];
	    // Attempt to extend lcp
	    while (i+h<n && j+h<n && s[i+h]==s[j+h])
	      h++;
	    lcp[k]=h;
	  }
	  if (h>0)
	    h--;  // Decrease according to result
	}
}


int minimum(int x, int y){
	if(x==-1 || x>y){
		return y;
	}else{
		return x;
	}
}


int get_t(int dollarPos, int i, int hashPos){
	return sa[i]<=dollarPos ? 0
		: sa[i]>hashPos ? 2 
		: 1
	;
}

int main(){
	
	int i, j, k, p, t, x,y,z,m, dollarPos, hashPos, LCSpos=-1, LCSlength=1;
	int length, x_t, last_t;
	printf("Input strings:\n");
	scanf("%s",s1);
	scanf("%s",s2);
	scanf("%s",s3);
	
	
	// Concatenate the Three input strings, but with $ and # separators.
	for (i=0;s1[i]!='\0';i++)
		s[i]=s1[i];
		
	dollarPos=i;
	s[i++]='$';
	
	for (j=0;s2[j]!='\0';j++)
		s[i+j]=s2[j];
		
	hashPos = i+j;
	s[i+j]='#';
	j++;
	
	for (k=0;s3[k]!='\0';k++)
		s[i+j+k]=s3[k];
		
	s[i+j+k]='\0';
	n=i+j+k+1;


	printf("Output:\n");
	// Print the total length of the three srings along with the separators
	printf("n is %d\n",n);
	
	// Quick-and-dirty suffix array construction
	for (i=0;i<n;i++)
	  sa[i]=i;
	qsort(sa,n,sizeof(int),suffixCompare);
	computeRank();
	computeLCP();
	
	/* Commenting the table data print statement as per Lab3 direction
	if (n<3000)
	{
		printf("i     sa    suffix                              lcp   t s  rank   lcp[rank]\n");
		for (i=0;i<n;i++){
			t = sa[i]<=dollarPos ? 0 
				: sa[i]>hashPos ? 2
				: 1
			;
			printf("%-5d %-5d %-35.35s %-5d %-1d %c  %-5d  %-5d\n",
			  i,sa[i],&s[sa[i]],lcp[i],t,
			  s[i],rank[i],lcp[rank[i]]);
		}
	
	}
	*/
	// Finding all the common substrings with length greater than the previous one
	for (i=1;i<n;i++){
		x_t = last_t = get_t(dollarPos, i, hashPos);
		y = z = length = -1;
		m = i+1;
		x = i;
		while(m<n){
			t = get_t(dollarPos, m, hashPos);
			
			if(t == x_t || LCSlength > lcp[m])
				break;
			else if(y == -1 || last_t == t)
				y = m;
			else
				z = m;
			
			length = minimum(length, lcp[m]);
			if (length == lcp[m])
				LCSpos = m;
			
			if(z>-1){
				LCSlength = length;
				printf("Length %d, x at %d, y ends at %d, z at %d\n", LCSlength, x, y, z);
				printf("%.*s\n",LCSlength,s+sa[LCSpos]);
				break;
			}
			last_t = t;
			m++;
		}
		
	}
	  
}
