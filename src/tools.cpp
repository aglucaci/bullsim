
// This file is part of BULL, a program for phylogenetic simulations
// most of the code was written by Mark T. Holder.

//	This program is for internal use by the lab of Dr. Tandy Warnow only.
//	Do not redistribute the code.  It is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//
//	Some of the code is from publically available source by Paul Lewis, Ziheng Yang, 
//	John Huelsenbeck, David Swofford , and others  (as noted in the code).
//	In fact the main structure of the program was created by modifying Paul Lewis' 
//	basiccmdline.cpp from his NCL
//
//	This code was used in Mark's dissertation, some changes were made in order to 
//	get it to compile on gcc.  It is possible that this porting introduced bugs (very
//	little debugging has been done on UNIX platforms).	I would suggest checking 
//	the simulator by generating data on trees with short branches, etc.
 

/* code from Ziheng Yang's tools.c
*/
#include "tools.hpp"
namespace bull {
extern long gRngSeed;
}
double LnGamma (double alpha)
{
/* returns ln(gamma(alpha)) for alpha>0, accurate to 10 decimal places.
   Stirling's formula is used for the central polynomial part of the procedure.
   Pike MC & Hill ID (1966) Algorithm 291: Logarithm of the gamma function.
   Communications of the Association for Computing Machinery, 9:684
*/
   double x=alpha, f=0, z;

   if (x < 7) {
	  f=1;	z=x-1;
	  while (++z < 7)  f*=z;
	  x=z;	 f=-log(f);
   }
   z = 1/(x*x);
   return  f + (x-0.5)*log(x) - x + .918938533204673
		  + (((-.000595238095238*z+.000793650793651)*z-.002777777777778)*z
			   +.083333333333333)/x;
}

double IncompleteGamma (double x, double alpha, double ln_gamma_alpha)
{
/* returns the incomplete gamma ratio I(x,alpha) where x is the upper
		   limit of the integration and alpha is the shape parameter.
   returns (-1) if in error
   ln_gamma_alpha = ln(Gamma(alpha)), is almost redundant.
   (1) series expansion		if (alpha>x || x <= 1)
   (2) continued fraction	otherwise
   RATNEST FORTRAN by
   Bhattacharjee GP (1970) The incomplete gamma integral.  Applied Statistics,
   19: 285-287 (AS32)
*/
   int i;
   double p=alpha, g=ln_gamma_alpha;
   double accurate=1e-8, overflow=1e30;
   double factor, gin=0, rn=0, a=0,b=0,an=0,dif=0, term=0, pn[6];

   if (x == 0) return (0);
   if (x < 0 || p <= 0) return (-1);

   factor=exp(p*log(x)-x-g);
   if (x>1 && x >= p) goto l30;
   /* (1) series expansion */
   gin=1;  term=1;	rn=p;
 l20:
   rn++;
   term*=x/rn;	 gin+=term;

   if (term > accurate) goto l20;
   gin*=factor/p;
   goto l50;
 l30:
   /* (2) continued fraction */
   a=1-p;	b=a+x+1;  term=0;
   pn[0]=1;	 pn[1]=x;  pn[2]=x+1;  pn[3]=x*b;
   gin=pn[2]/pn[3];
 l32:
   a++;	 b+=2;	term++;	  an=a*term;
   for (i=0; i < 2; i++) pn[i+4]=b*pn[i+2]-an*pn[i];
   if (pn[5] == 0) goto l35;
   rn=pn[4]/pn[5];	 dif=fabs(gin-rn);
   if (dif>accurate) goto l34;
   if (dif <= accurate*rn) goto l42;
 l34:
   gin=rn;
 l35:
   for (i=0; i < 4; i++) pn[i]=pn[i+2];
   if (fabs(pn[4]) < overflow) goto l32;
   for (i=0; i < 4; i++) pn[i]/=overflow;
   goto l32;
 l42:
   gin=1-factor*gin;

 l50:
   return (gin);
}


double rndgamma1 (double s);
double rndgamma2 (double s);
double rndgamma (double s)
{
/* random standard gamma (Mean=Var=s,  with shape par=s, scale par=1)
	  r^(s-1)*exp(-r)
   J. Dagpunar (1988) Principles of random variate generation,
   Clarendon Press, Oxford
   calling rndgamma1() if s < 1 or
		   rndgamma2() if s>1 or
		   exponential if s=1
*/
   double r=0;

   if (s <= 0)	  puts ("jgl gamma..");
   else if (s < 1)	r=rndgamma1 (s);
   else if (s>1)  r=rndgamma2 (s);
   else			  r=-log(rndu());
   return (r);
}

double rndu (void)		//MTH redirect to RandomNumber
{
/* U(0,1): AS 183: Appl. Stat. 31:188-190
   Wichmann BA & Hill ID.  1982.  An efficient and portable
   pseudo-random number generator.	Appl. Stat. 31:188-190

   x, y, z are any numbers in the range 1-30000.  Integer operation up
   to 30323 required.

   static int x_rndu=11, y_rndu=23;
   double r;

   x_rndu = 171*(x_rndu%177) -	2*(x_rndu/177);
   y_rndu = 172*(y_rndu%176) - 35*(y_rndu/176);
   z_rndu = 170*(z_rndu%178) - 63*(z_rndu/178);
   if (x_rndu < 0) x_rndu+=30269;
   if (y_rndu < 0) y_rndu+=30307;
   if (z_rndu < 0) z_rndu+=30323;
   r = x_rndu/30269.0 + y_rndu/30307.0 + z_rndu/30323.0;
   return (r-(int)r); */

	return RandomNumber();
}


double rndgamma1 (double s)
{
/* random standard gamma for s < 1
   switching method
*/
   double r, x=0,small=1e-37,w;
   static double a,p,uf,ss=10,d;

   if (s!=ss) {
	  a=1-s;
	  p=a/(a+s*exp(-a));
	  uf=p*pow(small/a,s);
	  d=a*log(a);
	  ss=s;
   }
   for (; ; ) {
	  r=rndu();
	  if (r>p)		  x=a-log((1-r)/(1-p)), w=a*log(x)-d;
	  else if (r>uf)  x=a*pow(r/p,1/s), w=x;
	  else			  return (0);
	  r=rndu ();
	  if (1-r <= w && r>0)
		 if (r*(w+1) >= 1 || -log(r) <= w)	continue;
	  break;
   }
   return (x);
}

double rndgamma2 (double s)
{
/* random standard gamma for s>1
   Best's (1978) t distribution method
*/
   double r,d,f,g,x;
   static double b,h,ss=0;
   if (s!=ss) {
	  b=s-1;
	  h=sqrt(3*s-0.75);
	  ss=s;
   }
   for (; ; ) {
	  r=rndu ();
	  g=r-r*r;
	  f=(r-0.5)*h/sqrt(g);
	  x=b+f;
	  if (x <= 0) continue;
	  r=rndu();
	  d=64*r*r*g*g*g;
	  if (d*x < x-2*f*f || log(d) < 2*(b*log(x/b)-f))  break;
   }
   return (x);
}

//MTH modification	this random number generator requires a global seed gRngSeed
/*-------| rnum |------------------------------------------------
|	This pseudorandom number generator is described in:
|	Park, S. K. and K. W. Miller.  1988.  Random number generators: good
|	   ones are hard to find.  Communications of the ACM, 31(10):1192-1201.
*/
double RandomNumber()
{

	long int	lo, hi, test;

	hi = (bull::gRngSeed) / Q_Q;
	lo = (bull::gRngSeed) % Q_Q;

	test = A_A * lo - R_R * hi;
	if (test > 0.0)
		bull::gRngSeed = test;
	else
		bull::gRngSeed = test + M_M;

	return (double)(bull::gRngSeed) / (double)M_M;

}
/* functions concerning the CDF and percentage points of the gamma and
   Chi2 distribution
*/
double PointNormal (double prob)
{
/* returns z so that Prob{x < z}=prob where x ~ N(0,1) and (1e-12)<prob < 1-(1e-12)
   returns (-9999) if in error
   Odeh RE & Evans JO (1974) The percentage points of the normal distribution.
   Applied Statistics 22: 96-97 (AS70)

   Newer methods:
	 Wichura MJ (1988) Algorithm AS 241: the percentage points of the
	   normal distribution.	 37: 477-484.
	 Beasley JD & Springer SG  (1977).	Algorithm AS 111: the percentage
	   points of the normal distribution.  26: 118-121.

*/
   double a0=-.322232431088, a1=-1, a2=-.342242088547, a3=-.0204231210245;
   double a4=-.453642210148e-4, b0=.0993484626060, b1=.588581570495;
   double b2=.531103462366, b3=.103537752850, b4=.0038560700634;
   double y, z=0, p=prob, p1;

   p1 = (p < 0.5 ? p : 1-p);
   if (p1 < 1e-20) z=999;
   else {
	  y = sqrt (log(1/(p1*p1)));
	  z = y + ((((y*a4+a3)*y+a2)*y+a1)*y+a0) / ((((y*b4+b3)*y+b2)*y+b1)*y+b0);
   }
   return (p < 0.5 ? -z : z);
}


double PointChi2 (double prob, double v)
{
/* returns z so that Prob{x < z}=prob where x is Chi2 distributed with df=v
   returns -1 if in error.	 0.000002 < prob < 0.999998
   RATNEST FORTRAN by
	   Best DJ & Roberts DE (1975) The percentage points of the
	   Chi2 distribution.  Applied Statistics 24: 385-388.	(AS91)
   Converted into C by Ziheng Yang, Oct. 1993.
*/
   double e=.5e-6, aa=.6931471805, p=prob, g, small=1e-6;
   double xx, c, ch, a=0,q=0,p1=0,p2=0,t=0,x=0,b=0,s1,s2,s3,s4,s5,s6;

   if (p < small)	return(0);
   if (p>1-small) return(9999);
   if (v <= 0)	  return (-1);

   g = LnGamma (v/2);
   xx=v/2;	 c=xx-1;
   if (v >= -1.24*log(p)) goto l1;

   ch=pow((p*xx*exp(g+xx*aa)), 1/xx);
   if (ch-e < 0) return (ch);
   goto l4;
l1:
   if (v>.32) goto l3;
   ch=0.4;	 a=log(1-p);
l2:
   q=ch;  p1=1+ch*(4.67+ch);  p2=ch*(6.73+ch*(6.66+ch));
   t=-0.5+(4.67+2*ch)/p1 - (6.73+ch*(13.32+3*ch))/p2;
   ch-=(1-exp(a+g+.5*ch+c*aa)*p2/p1)/t;
   if (fabs(q/ch-1)-.01 <= 0) goto l4;
   else						  goto l2;

l3:
   x=PointNormal (p);
   p1=0.222222/v;	ch=v*pow((x*sqrt(p1)+1-p1), 3.0);
   if (ch>2.2*v+6)	ch=-2*(log(1-p)-c*log(.5*ch)+g);
l4:
   q=ch;   p1=.5*ch;
   if ((t=IncompleteGamma (p1, xx, g))<0) {
	  printf ("\nerr IncompleteGamma");
	  return (-1);
   }
   p2=p-t;
   t=p2*exp(xx*aa+g+p1-c*log(ch));
   b=t/ch;	a=0.5*t-b*c;

   s1=(210+a*(140+a*(105+a*(84+a*(70+60*a))))) / 420;
   s2=(420+a*(735+a*(966+a*(1141+1278*a))))/2520;
   s3=(210+a*(462+a*(707+932*a)))/2520;
   s4=(252+a*(672+1182*a)+c*(294+a*(889+1740*a)))/5040;
   s5=(84+264*a+c*(175+606*a))/2520;
   s6=(120+c*(346+127*c))/5040;
   ch+=t*(1+0.5*t*s1-b*c*(s1-b*(s2-b*(s3-b*(s4-b*(s5-b*s6))))));
   if (fabs(q/ch-1) > e) goto l4;

   return (ch);
}



