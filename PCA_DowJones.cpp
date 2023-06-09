// These functions are defined below.
void AllocateMemory ();
void GetComponentData(void);
void GetM(void);
void Preliminaries ();
void ReportForScatter(double*,double*);
void ReportForScatterBeta(double*,double*);

// Global variables
double *M;
char **ticker;
int n=3041, p=8;
int *mm, *dd, *yy;
double **T, **Xtilde, **X, **D, **Q, **Y, **V, *mu,**L, **Lambda;
double* Beta = new double[31];
double* EVect_1 = new double[31];
#include "Functions.h"
#include <iostream>
int main () {
   double **E;
   int p, n, i;

   // Allocate some array space.
   AllocateMemory ();
//Read in the DJI data. These are daily returns for the 30 components for 2020.
//Put it into a 30 x 253 array X. For your convenience, the data has already been
//shifted so each row has mean zero. The means are very small and this has minimal
//impact.
   GetComponentData();
// Now read in the daily returns of the total Dow average for 2020. This is a list
// of 253 returns and is put into the list M.  This is used in problem 3
// to compute the individual components' betas. Also used in problem 4. The data
// here has also been shifted to have mean zero.
   GetM();
   // Get the dimensions of DJI.
   n = Columns (X);      // Should be 253
   p = Rows (X);         // Should be 30
   V = Covariance (X);
// Compute eigenvalues and store them in E
   E = QRalgorithm (V); 

// Compute the eigenvectors.
   Q = Evector (V, E);
   L = Correlation(X);
   Show(L);
//By using Show L, we see that L is positive.
// No need to check for any negative values.
//Check which stock pairs have correlation greater than 0.8
//L is symmetric 30 x 30 , therefore we loop though the columns while k<j
   for(int j = 1; j <= 30; ++j){                             
      for (int k = 1; k < j; ++k) {
         if(L[j][k] > 0.8){         //test condition
            std::cout<<ticker[k]<<"\n";      
            std::cout<<ticker[j] <<"\n"<<"Has correlation = "<<L[j][k]<<"\n\n";
         }
      }
   }
   Pause();
   // Number 3 after getting M. 
   // beta = COV(R,M) / VAR(M)
   // M is the overall market return that same day.
   // R is stock return out of 253 days. 
   // compute variance of M that same day, for each stock
   double* var = new double[31];
   double* covar = new double[31];
   for(int s = 1;s<=30; ++s){ // for each stock
      for(int d = 1;d <= 253;++d){ //compute the var, covar for each day.
         var[s] = ((d-1)*var[s] + M[d]*M[d]) / d;
         covar[s] =((d-1)*covar[s] + M[d]* X[s][d]) / d;
      }
   }
// compute beta for each stock, and show the associated ticker.
   for(int b = 1;b<=30;++b){ 
      Beta[b] = covar[b] / var[b];  
      std::cout<<"Beta for stock "<<ticker[b]<<"\n is = "<<Beta[b]<<"\n";
   }
   Pause();
   for(int q = 1;q<=30;++q){ 
      std::cout<<Q[q][1]<<"\n";
      EVect_1[q] = Q[q][1];
   }
// generate a txt file for (Beta,Eigenvector1) points,
// which will then be run through overleaf for scatterplot 
   ReportForScatterBeta(Beta,EVect_1); 
   //END of question 3
   double ** QT;
   QT = Transpose(Q);
   // Compute the principal components of each column of X. Put them into the
   //    corresponding column of Y.
   Y = Multiply (QT, X);
   Pause();
//Question 4: Compute x = B_i*M_t ; and y = Y_1t*q_i; 
// where Y1t is the first principle component of the t^th column of X 
// and q_i is the i^th component of the first eigenvector.

// create memory to store points.
   double* x = new double[7591]; double* y = new double[7591];

   // Compute x and y. Note: 1 <= i <= 30 and 1 <= t <= 253 
   int count = 0;
   for(int t = 1;t <= 253; ++t){ // loop through days.
      for(int i = 1; i<= 30; ++i){  //loop through all betas
         x[count] = M[t]*Beta[i];
         ++count;
      }
   }

   Pause();
   // reset count for the next 7590 points of data for y
   count = 0;
   for(int t = 1;t <= 253; ++t){ // loop through days
      for(int i = 1; i<= 30; ++i ){  // loop through 30 coomponents of Eigenvector 1. 
         y[count] =  Y[1][t] * Q[i][1];
         ++count;
      }
   }
// call function to write vector x, y into a txt file. 
// This will be converted to TeX.
   ReportForScatter(x,y);
  
   // Start of Question 6 //
// Show eigenvector 2 
   for(int i=1;i<=30;++i){
      std::cout<< Q[i][2]<<" :: "<<ticker[i]<<"\n";
   }
   std::cout<<"\n\n";
// Show components of eigenvector 2 such that the absolute value is greater than 0.15
   for(int i=1;i<=30;++i){
      if(Q[i][2] > 0.15 || Q[i][2] * -1 > 0.15 ){
         std::cout<< Q[i][2]<<" :: "<<ticker[i]<<"\n";
      }
      // Tech companies are heavily represented in this eigenvector
   }
   // END of Question 6 //
   Pause ();
   delete[] x; delete[] y; delete[] var; 
   delete[] covar; delete[] Beta; delete[] EVect_1;
}
// This function reads in the 253 daily returns
// for each of the 30 components of the Dow for the year 2020.
void GetComponentData() {
   int i, k, t;
   double R, mu;
   char input[100];
   FILE *fp;
   fp = fopen ("DJI.txt", "r");
   for (i = 1; i <= 30; i++) {
      // Read in stock i's return data.
      // Get the tickers for each stock.
      fgets (input, 99, fp);
      for (k = 0; k < 10; k++) {
         ticker[i][k] = input[k];
         if (ticker[i][k] == '\n') {
            ticker[i][k] = '\0';
            break;
         }
      }
      // Read in the 253 daily returns in percent.
      mu = 0.0;
      for (t = 1; t <= 253; t++) {
         // Month t return for stock i, put it into X[i][t].
         fgets (input, 99, fp);
         sscanf (input, "%lf", &R);
         X[i][t] = R;
         mu += R / 253.0;
      }
      // Make the mean return 0 (this has minimal impact).
      for (t = 1; t <= 253; t++) {
         X[i][t] -= mu;
      }   
   }
   fclose (fp);
   return;
}
// This function writes the coordinates for plotting into a txt file before TeX.
// Note that all arrays are already organized and ordered properly.
// takes in arrays as parameters
void ReportForScatterBeta(double* Beta, double* EVect_1) { 
     FILE *plotBeta;                             // file variable plotBeta
     plotBeta = fopen("Scat_Plot_Beta.txt","w"); // open txt file in write mode
     for(int i = 1 ;i <= 30;++i){
         fprintf(plotBeta,"%lf",Beta[i]);     // write first component of array, 
         fprintf(plotBeta,"%s"," ");          // separate with space on that line
         fprintf(plotBeta,"%lf\n",EVect_1[i]);             
    // write y coordinate point, followed by new line
     }
     fclose(plotBeta);     // close plotBeta file. 
}
// This function writes the coordinates for plotting into a txt file before TeX.
// This is the same function as above, except we are doing it for more points. 
// Note that all arrays are already organized and ordered properly.
void ReportForScatter(double* x, double* y) {
     FILE *plot;
     plot = fopen("Scat_Plot_xy3","w");
     for(int i = 1 ;i <= 7590;++i){
         fprintf(plot,"%lf",x[i]); 
         fprintf(plot,"%s"," ");
         fprintf(plot,"%lf\n",y[i]);
     }
     fclose(plot);
}
// This function reads in the daily aggregate returns of the Dow Jones Industrial Average
// for the year 2020.
void GetM() {
   int t;
   double R, mu=0.0;
   char input[100];
   FILE *fp;
   fp = fopen ("M.txt", "r");
   for (t = 1; t <= 253; t++) {
      // Month t return for the DJIA (the total Dow).
      fgets (input, 99, fp);
      sscanf (input, "%lf", &R);
      M[t] = R;
      mu += R / 253.0;
   }
   // Make the market returns mean zero (this has minimal impact).
   for (t = 1; t <= 253; t++) {
      M[t] -= mu;
   }
   fclose (fp);
   return;
}
// Allocate some array space.
void AllocateMemory () {
   int i;
   X = Array (30, 253);
   M = List (253);
   ticker = (char **) calloc (31, sizeof (char *));
   for (i = 1; i <= 30; i++) {
      ticker[i] = (char *) calloc (10, sizeof (char));
   }
   return;
}
