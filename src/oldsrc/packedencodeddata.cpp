#include "packedencodeddata.h"PackedEncodedData::PackedEncodedData(int EncodType,int PackedType,int packedpats,int *packedReps,short *packedData,DataSettings *DSettings,int inputArraySpacing){	nchars=packedpats;	int sizeofRaw=nchars*NumShortsPerCharacter(PackedType);	typeOfEncoding=EncodType;	//int ncpc=NumColumnsPerCharacter(typeOfEncoding);	int nspc=NumShortsPerCharacter(typeOfEncoding);	int nsils=NumStatesInLastShort(typeOfEncoding);	SharedConstruction(nspc,sizeofRaw,nsils,packedpats,packedReps,packedData,DSettings,inputArraySpacing);}PackedEncodedData::PackedEncodedData(int EncodType,Model *mod,int packedpats,int *packedReps,short *packedData,DataSettings *DSettings,int inputArraySpacing){	assert(EncodingType(SubModMitoCodons)==EncodType || EncodType==EncodingType(SubModNucCodons));	nchars=packedpats;	int nspc=mod->NumShortsPerCharacter();	int sizeofRaw=nchars*nspc;	typeOfEncoding=EncodType;	int nsils=mod->NumStatesInLastShort();	SharedConstruction(nspc,sizeofRaw,nsils,packedpats,packedReps,packedData,DSettings,inputArraySpacing);}	void PackedEncodedData::SharedConstruction(int nspc,int sizeofRaw,int nsils,int packedpats,int *packedReps,short *packedData,DataSettings *DSettings,int inputArraySpacing){	short *tempd;	for(int i=0;i<DSettings->GetNIncludedTaxa();i++)		{tempd=new short[sizeofRaw];		CopyArray(tempd,(packedData+i*inputArraySpacing),sizeofRaw);		data[DSettings->GetNameOfNthIncludedTaxon(i)]=tempd;		}	nReps=new short[nchars];	short *temp;	temp=nReps;	for(int i=0;i<nchars;i++)		*temp++=*packedReps++;		vector<short> constpats;		isConstant=new int[nchars];	short *testchar;	short temps; int k;	testchar=new short[nspc];	for(int i=0;i<nchars;i++)		{bool couldBeConst=true;		for(k=0;k<nspc-1;k++)			*(testchar+k)=~0;		temps=1;		*(testchar+k)=0;		for(int j=0;j<nsils;j++)			{*(testchar+k)|=temps;			temps<<=1;			}		for(map<nxsstring,short *>::iterator datit=data.begin();(couldBeConst && datit!=data.end());datit++)			{temp=i*nspc+(*datit).second;			for(int j=0;j<nspc;j++)				*(testchar+j)&=*(temp+j);			couldBeConst=false;			for(int j=0;(!couldBeConst && j<nspc);j++)				if(*(testchar+j))					couldBeConst=true;			}		if(couldBeConst)			{isConstant[i]=(1+constpats.size()/nspc);			for(int j=0;j<nspc;j++)				constpats.push_back(*(testchar+j));			}		else			isConstant[i]=0;		}	/* if you switch back to the non-pointer math version of getting the constant chars in TreeLikeAttr, go back to the old	version of these lines:	ConstChar=new short[constpats.size()+nspc];	for(int i=0;i<constpats.size();i++)		*(ConstChar+i+nspc)=constpats[i];	*/		ConstChar=new short[constpats.size()];	for(int i=0;i<constpats.size();i++)		*(ConstChar+i)=constpats[i];	delete [] testchar;}	PackedEncodedData::~PackedEncodedData(){		for(map<nxsstring,short *>::iterator datit=data.begin(); datit!=data.end();datit++)		delete [] ((*datit).second);	delete []nReps;	delete []isConstant;	delete []ConstChar;}	int PackedEncodedData::GetNumTaxWithState(short *st,int charNum,bool equalsOnly/*=true*/){	assert(charNum>=0 && charNum<nchars);	int n=0;	int nspc=NumShortsPerCharacter(typeOfEncoding);	int offset=nspc*charNum;	if(equalsOnly)		for(map<nxsstring,short *>::iterator dIt=data.begin();dIt!=data.end();dIt++)			{short *temp=offset+dIt->second;			bool match =true;			for(int i=0;(match && i<nspc);i++)				if(temp[i]!=st[i])						match=false;			if(match)				n++;			}	else			{short *inter;//count intersections too		inter=new short[nspc];		for(map<nxsstring,short *>::iterator dIt=data.begin();dIt!=data.end();dIt++)			{short *temp=offset+dIt->second;			for(int i=0; i<nspc;i++)				inter[i]=temp[i]&st[i];			bool emptyInt =true;			for(int i=0;(emptyInt && i<nspc);i++)				if(inter[i])					emptyInt=false;			if(!emptyInt)				n++;			}		delete []inter;		}	return n;}	