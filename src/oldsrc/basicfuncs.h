#ifndef _BASICFUNCS#define _BASICFUNCS#include <strstream>#include <fstream.h>#include <iomanip.h>/*Combo of my funcs and stuff taken from PAML and Mr Bayes*/#include <list>#include <vector>#include <string>#include <iterator>#include <stdio.h>#include "nxsstring.h"#include <iostream.h>#include <fstream.h>#include "tools.h"#include "mthexception.h"#include "basicbulldefs.h"class  VariableNotDetermined : public MTHException {};int ClassifyChange(short int brstart,short int brend,short int *plink);//Rogers and Swofford's Fixed and Potentially Sameint ClassifyChangeTerm(short int brstart,short int brend,short int *plink);//Rogers and Swofford's Fixed and Potentially Sameint ConvertCodeToIndex(short int c);int ConvertPermLinkToChange(short *plink);short int DecodeAChar(short int n);int DecodeAmbiguityCode(char *dest,short int c);void gcf( double& gammcf, double a, double x, double& gln );double GetNumberFromStr(char *source);double GetNumberFromStr(ifstream &input,char c);void GetStrToPunc(class ifstream &input,char *c,char *dest);double GetNumberFromNStr(nxsstring::iterator &c);void GetStrToPunc(char *source,char *dest);void GetNStrToPunc(nxsstring::iterator &c,char *dest);nxsstring GetNStrToPunc(nxsstring::iterator &inc);double gammln( double xx );double gammq( double a, double x );void gser( double& gamser, double a, double x, double& gln );long NumberOfLongsLeftInMemory();long NumberOfCharsLeftInMemory();long NumberOfBoolsLeftInMemory();int NextNexusWord(class ifstream &StreamRef,char *temp);char nexusget(class ifstream &inp);double point_normal( double pr );double point_chi2 ( double pr, double v );void PrintPMat(double *pmat);int ProductOfSquareMatrices(double *prod, int dimen, double *first, double *second);double RandomNumber(long int *seed);int ScreenStrForStr(class ifstream &StreamRef,char *source,char *fragment);int ScreenStrForStr(char *source,char *fragment);int StateCount(short int c);void itos(char *dest,int x);void ltos(char *dest,long x);void dtos(char *dest,double x);int NextWord(class ifstream &StreamRef,char *temp);int UtoCstrcpy(char *ctemp,unsigned char *temp);int CtoUstrcpy(unsigned char *ctemp, char *temp);int CtoUPascalstrcpy(unsigned char *ctemp, char *temp);bool StrForDoubleInRangeNoSci(char *str,double minim,double maxim);bool StrForDoubleInRange(char *str,double minim,double maxim);double GetPartNumberFromStr(char *source);bool CouldTurnIntoADouble(char *str);bool CouldTurnIntoADoubleNoSci(char *str);bool HasADigit(char *str);void SortDoubleArray(double *x,int len); char *AllocateCharAndCopyNStr(nxsstring instr,int len);nxsstring GetNextGraphStr(nxsstring::iterator &nextlet,nxsstring::iterator endp);short EncodeTripletToCodon(short *str);nxsstring MitoTranslateThreeNucleotides(short *bases);nxsstring MitoTranslateThreeMinusSecondArgNucleotides(short *bases,int basesIncluded);nxsstring NucTranslateThreeNucleotides(short *bases);nxsstring NucTranslateThreeMinusSecondArgNucleotides(short *bases,int basesIncluded);short TranslateNumCodonMito(short str);short ConverNumAAToLetAA(short str);bool FillArrayOfAllCodonsMt(short *codonarray, short currcodon);void RevTransNCodToNNuc(short aa,short *dnaseq);bool IsGap(short c);short GetMissing();short GetNStatesIncludingMissing();short GetNStatesIncludingMissingAndGaps();void GaussJordanElimination(double **mat,double *vars,int dim);int StateCount( int c);int EncodeTripletToCodon(int *str);bool PointsAreFarEnoughApart(double a,double b);#endif