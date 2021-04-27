/*
Instruction to execute the code:
1. No need to specify any gcc version if you are compiling at Omega.uta.edu. It runs on any gcc versions from 83 and above.
2. If the # of coupons exceed 100, then the array size of pascal triangle need to change. Also long double datatype is defined. So it can handle nCr value upto exponent range of −6143 to +6144 and 34 decimals
*/

#include<stdio.h>
#include<stdlib.h>

double randomSimulation(int empirical, int seed, int m, double p1, double p2, int type1, int type2);
void theoretical(int m, double p1, double p2, int type1, int type2);

int main(){
        int m,type1,type2,empirical,seed;
        double p1,p2,box;
        // Enter the numOfCoupons & numOfCoupons of firstType separated by a space
        printf("Enter the num of coupons and num of coupons of first type separated by a space: ");
        scanf("%d %d", &m, &type1);
        // Enter the probability for each coupon of first type
        printf("Enter the probability for each coupon of first type: ");
        scanf("%lf", &p1);
        type2 = m - type1;
        p2 = (1 - (type1 * p1)) / type2;
        printf("There are %d coupons of second type with probability %f", type2, p2);
        // Enter # of iterations for empirical
        printf("%s", "\nEnter # of iterations for empirical: ");
        scanf("%d", &empirical);
        // Enter the seed val
        printf("%s", "Enter seed: ");
        scanf("%d", &seed);

        theoretical(m, p1, p2, type1, type2);
        box = randomSimulation(empirical, seed, m, p1, p2, type1, type2);
        printf("Empirical boxes opened: %lf\n",box);

        return 0;
}


void theoretical(int m, double p1, double p2, int type1, int type2){
        int i,j,set,pow,count=0;
        long double prod, sum, den, theoraticalSum = 0;
        long double pascal_triangle[100][100] = { 0 };

// Create the pascal triangle for the combination vals, the size is 100. If the value of # of coupons are more than 100 then change the val accordingly
        pascal_triangle[0][0] = 1;
        for(i=1;i<100;i++){
                pascal_triangle[i][0]=1;
                for(j=1;j<=i;j++){
                        pascal_triangle[i][j] = pascal_triangle[i-1][j-1] + pascal_triangle[i-1][j];
                }
        }

// Idea is to calculate the possible number of coupons drawn to get a new coupon using Flajole's theory.
        for(set=0;set<m;set++){
                sum = 0;
                for(i=0; i<=set; i++){
                        j = set-i;
                        prod = pascal_triangle[type1][i] * pascal_triangle[type2][j];
                        den = (i*p1)+(j*p2);
                        if(prod > 0){
                                sum += prod/(1- den);
                                count++;
                        }
                }
                //printf("val of i=%d, Sum=%Lf\n",i,Sum);
                pow = m-1-set;
                if(pow%2==0){
                        theoraticalSum += sum;
                }else{
                        theoraticalSum -= sum;
                }
        }
        printf("Theoretical %Lf Count %d \n", theoraticalSum, count);
}

/*
Step1: Create an array for the sample space named sample.
Step2: If (p1,p2,p3) = (.2,.4,.4) => (0.2*10, 0.4*10, 0.4*10), assign sample[] = [0,0, 1,1,1,1, 2,2,2,2].
                Where 0 the first coupon, 1 is the second and 2 is the 3rd one.
           If (p1,p2,p3,p4,p5) = (.1,.1,.2666,.2666,.2666) => (0.1*10000, 0.1*10000, 0.2666*10000, 0.2666*10000, 0.2666*10000),
           assign sample[] = [0 upto 999th index, 1 [for 1000 times],2 [for 2666 times], 3 [repeat ],4 [repeat ]].
Step3: Run a random simulator on the indices of sample[];
Step4: Take another array coupons[m] which will represent after some random collection, how many are collected and how many are yet to be.
Step5: Once m distint coupons are collected from sample, we will break the loop and count the number of random fetch performed.
Step6: We will perform the step5 for the # of iterations given as input and take the avg of the numbers and return back.
*/

double randomSimulation(int empirical, int seed, int m, double p1, double p2, int type1, int type2){
        srand(seed);
        int length, round_p1 = 0, round_p2 = 0, coupons[m];
        int new_coupons = 0,indx = 0, i, j, r;
        unsigned int loop_count = 0;
        // RAND_MAX is a constant whose default value may vary between implementations but it is granted to be at least 32767. bc p1/p2 is being multiplied by 2,
        // we should break once length is gonna cross the threshold val in the next loop
        // Step 2
        while((round_p1 != p1) || (round_p2 != p2)){
                p1 = p1*2;
                p2 = p2*2;
                round_p1 = p1;
                round_p2 = p2;
                length = (type1*round_p1)+(type2*round_p2);
                if(length >= 16380){break;}
        }
        int sample[length];
        // Creating the sample space
        for(i=0; i<m; i++){
                if(i<type1){
                        for(j=0; j<round_p1; j++){
                                sample[indx] = i;
                                indx += 1;
                        }
                        //printf("sample [%d]= %d\n", i,round_p1);
                }
                else{
                        for(j=0; j<round_p2; j++){
                                sample[indx] = i;
                                indx += 1;
                        }
                        //printf("sample [%d]= %d\n", i,round_p2);
                }
        }
        // Step3-6
        for(j=0; j<empirical; j++){
                for (i = 0; i < m; i++){
                        coupons[i] = 0;
                }
                new_coupons = 0;
                while(new_coupons<m){
                        r = rand() % length;
                        indx = sample[r];
                        if (coupons[indx] == 0){
                                coupons[indx] = 1;
                                new_coupons++;
                        }
                        loop_count++;
                }
        //      printf(" %d ",loop_count);
        }

        return (double)loop_count/empirical;
}
