#include "datapartition.h"#include "basicfuncs.h"DataPartition::DataPartition(){	packedData=NULL;	nmodels=0;	mods=NULL;	destination=NULL;	dirty=false;	modelMixingParam=NULL;	packingTypes=-1;	packingIndex=0;}DataPartition::DataPartition(LikeSettings *LSettings, map<nxsstring,EncodedChars *>& rawData,int num){	partCrit=0;	nmodels=LSettings->GetNModelsInPart(num);	packedData=new PackedEncodedData *[nmodels];	mods=new Model*[nmodels];	for(int n=0;n<nmodels;n++)	{		packedData[n]=NULL;		mods[n]=NULL;		}			assert(nmodels>0);	double *startingMixP;	int ModelMixingSetting;		if(nmodels>1)		{startingMixP=new double[nmodels];		ModelMixingSetting=LSettings->GetModelMixingParamSettings(num);		}	for(int j=0;j<nmodels;j++)		{AddModel(LSettings->GetModel(num,j),j);		if(nmodels>1 && ModelMixingSetting&par(CUR))			startingMixP[j]=LSettings->GetStartingFreqnOfThisModel(num,j);		}	if(nmodels>1)		{modelMixingParam=new FreqParamGroup(nmodels,startingMixP,ModelMixingSetting); 		delete [] startingMixP;		}	ntax=LSettings->sharedDataSettings->GetNIncludedTaxa();	packingTypes=(*mods)->GetEncodingType();	packingIndex=0;	PackDataPartitions(LSettings->sharedDataSettings,rawData,num);	dirty=false;}DataPartition::DataPartition(ParsSettings *PSettings, map<nxsstring,EncodedChars *>& rawData,int num){	partCrit=1;	nmodels=1;	packedData=new PackedEncodedData *[1];	packedData[0]=NULL;	mods=NULL;	destination=NULL;	modelMixingParam=NULL;	packingTypes=-1;	packingIndex=0;	packingTypes=PSettings->GetEncodingType(num);	packingIndex=0;	bool ordered=PSettings->Ordered(num);		assert(!ordered);	ntax=PSettings->sharedDataSettings->GetNIncludedTaxa();	PackDataPartitions(PSettings->sharedDataSettings,rawData,num);	dirty=false;}void DataPartition::AddModel(Model *m,int place){	assert(place<nmodels);	mods[place]=m;}void DataPartition::PackDataPartitions(DataSettings *DSettings,map<nxsstring,EncodedChars *>&rawData,int num){	//Add all encodings types for any model in this partition into a list	assert(nmodels=1);//reinstating multiple models (if they differ in character encodings) will involve	//postponing the encoding from CopyAndEncodeAPartitionsRawData() until the creation of PackedEncodedData (replacing	//the copy array with a copy and encode.  THis would also entail keeping better track of the extent to which data is 	//encoded in the packing portion (numshortsperfinal character versus numofcolumnspershort in raw data).  Ugh		bool allsame=true,found;	vector<int> encodings;	bool GapsArentNs=false;	if(partCrit==0)//ML multiple models in a partition are possible		{encodings.push_back((*mods)->GetEncodingType());		for(int i=1;i<nmodels;i++)			{int temp=(*mods+i)->GetEncodingType();			if((temp==EncodingType(SubModMitoCodons) || temp==EncodingType(SubModNucCodons))  && nmodels>0)				throw MTHException("Can't implement SubModel Mitochondrial Codon encoding with more than one model per partition");//if relaxing, much of this function needs to be modified			found=false;			for(int j=0;(!found && j<encodings.size());j++)				if(temp==encodings[j])					found=true;			if(!found)	{allsame=false;						encodings.push_back(temp);						}			}		//find the most restrictive (least packable coding of the characters)		packingTypes=encodings[0];		packingIndex=0;		if(encodings.size()>1)	//more than one encoding type in this partition			for(int j=0;(!found && j<encodings.size());j++)				{if(CanTreatGapsAsFifthBase(encodings[j]))					GapsArentNs=true;				if(LessPackable(encodings[j],packingTypes))					{packingTypes=encodings[j];					packingIndex=j;					}				}		if(!DSettings->TreatGapsAsFifth())			GapsArentNs=false;		}	else		{assert(partCrit==1);//parsimony		if(DSettings->TreatGapsAsFifth())			GapsArentNs=true;		else	GapsArentNs=false;		}				//Create a temporary matrix of the unpacked data in this partition and int array of destination	short *tempunpacked,*ttpack,*ttunpack;	int nspc;	int nRawColsPerChar;	int nRawCharsInPart;	if(packingTypes!=EncodingType(SubModMitoCodons) && packingTypes!=EncodingType(SubModNucCodons))			nspc=NumShortsPerCharacter(packingTypes);	else	nspc=(*mods)->NumShortsPerCharacter();		nRawColsPerChar=NumColumnsPerCharacter(packingTypes);	nRawCharsInPart=DSettings->GetRawDataSize(num);	if(nRawCharsInPart%nRawColsPerChar)			throw MTHException("The number of characters in the data partition isn't a multiple of the number of character per transformed character.");			int unpackedSize=nRawCharsInPart*nspc/nRawColsPerChar;	tempunpacked=new short[ntax*unpackedSize];	if(packingTypes!=EncodingType(SubModMitoCodons) && packingTypes!=EncodingType(SubModNucCodons))		for(int i=0;i<ntax;i++)			{DSettings->CopyAndEncodeAPartitionsRawData((tempunpacked+i*unpackedSize),rawData,num,i,nspc,nRawColsPerChar,packingTypes,GapsArentNs);						/*if(!GapsArentNs && nRawColsPerChar==1 && nspc==1)				{short *tempsptr;				tempsptr=(tempunpacked+i*unpackedSize);				for(int j=0;j<nRawCharsInPart;j++)					{if(IsGap(tempsptr,packingTypes))							EncodeACharacter(tempsptr,'-',packingTypes,false);//Should write as a function to make more general. 					tempsptr+=nspc;					}				}*/			}	else					for(int i=0;i<ntax;i++)			DSettings->CopyAndEncodeAPartitionsRawData(*mods,(tempunpacked+i*unpackedSize),rawData,num,i,nspc,nRawColsPerChar,packingTypes,GapsArentNs);			int *nReps,ndataPatterns=0;	destination=new int[DSettings->GetRawDataSize(num)];	nReps=new int[DSettings->GetRawDataSize(num)];		short *TEMPpacked;	TEMPpacked=new short[ntax*unpackedSize];		//If not aligning, pack the data matrix	bool pack=!(DSettings->MaintainCharacterInputOrder());	assert(unpackedSize%nspc==0);	int filledpackedSize=0;	int unpackbegpos=0,packedbegpos=0;	//		if(ncolsPerChar>1)	{	while(unpackbegpos<unpackedSize)		{found=false;		packedbegpos=0;		if(pack)			while(packedbegpos<filledpackedSize && !found)				{found=true;				ttpack=TEMPpacked+packedbegpos;				ttunpack=tempunpacked+unpackbegpos;				for(int tn=0;(tn<ntax && found);tn++)					{for(int whichShort=0;(whichShort<nspc && found);whichShort++)						if(*(ttpack+whichShort)!=*(ttunpack+whichShort))							found=false;					ttpack+=unpackedSize;					ttunpack+=unpackedSize;					}				if(!found)	packedbegpos+=nspc;				}		else			packedbegpos=filledpackedSize;		if(found)			{destination[unpackbegpos/nspc]=packedbegpos/nspc;			nReps[destination[unpackbegpos/nspc]]++;			}		else			{ndataPatterns++;			destination[unpackbegpos/nspc]=packedbegpos/nspc;			nReps[destination[unpackbegpos/nspc]]=1;			filledpackedSize+=nspc;			for(int dit=0;dit<nspc;dit++)				{for(int tn=0;tn<ntax;tn++)					TEMPpacked[tn*unpackedSize+packedbegpos+dit]=tempunpacked[tn*unpackedSize+unpackbegpos+dit];				}			}		unpackbegpos+=nspc;		}/*			}else{while(unpackbegpos<unpackedSize){found=false;packedbegpos=0;while(packedbegpos<filledpackedSize && !found){found=true;ttpack=temppacked+packedbegpos;ttunpack=tempunpacked+unpackbegpos;for(int tn=0;(tn<ntax && found);tn++){if(*ttpack!=*ttunpack)found=false;ttpack+=packedSize;ttunpack+=unpackedSize;}if(!found)	packedbegpos++;}if(found){destination[unpackbegpos]=packedbegpos;nReps[packedbegpos]++;}else{ndataPatterns++;destination[unpackbegpos]=packedbegpos;nReps[packedbegpos]=1;filledpackedSize++;temppacked[packedbegpos]=tempunpacked[unpackbegpos];}}}*/		delete [] tempunpacked;		map<int,PackedEncodedData *> PEDmap;	map<int,PackedEncodedData *>::iterator PEDmapIt;	if(partCrit==0)		{for(int i=0;i<nmodels;i++)			{int thistype=(*mods+i)->GetEncodingType();			PEDmapIt=PEDmap.find(thistype);			if(PEDmapIt!=PEDmap.end())				packedData[i]=(*PEDmapIt).second;			else				{if(packingTypes!=EncodingType(SubModMitoCodons) && packingTypes!=EncodingType(SubModNucCodons))					packedData[i]=new PackedEncodedData(thistype,packingTypes,ndataPatterns,nReps,TEMPpacked,DSettings,unpackedSize);				else packedData[i]=new PackedEncodedData(thistype,*(mods+i),ndataPatterns,nReps,TEMPpacked,DSettings,unpackedSize);				PEDmap[thistype]=packedData[i];				}			}		}	else		{int thistype=packingTypes;		PEDmapIt=PEDmap.find(thistype);		if(PEDmapIt!=PEDmap.end())			packedData[0]=(*PEDmapIt).second;		else			{assert(packingTypes!=EncodingType(SubModMitoCodons) && packingTypes!=EncodingType(SubModNucCodons));			packedData[0]=new PackedEncodedData(thistype,packingTypes,ndataPatterns,nReps,TEMPpacked,DSettings,unpackedSize);			PEDmap[thistype]=packedData[0];			}		}	delete [] TEMPpacked;	delete [] nReps;	}int DataPartition::GetPlaceInPackedPartition(int prePackedNum){	return destination[prePackedNum];}	DataPartition::~DataPartition(){	//avoid double deleting packeddataEncodings	if(packedData)		{list<PackedEncodedData *> packlist;		PackedEncodedData *tempPED;		for(int i=0;i<nmodels;i++)			if(find(packlist.begin(),packlist.end(),packedData[i])==packlist.end())				packlist.push_back(packedData[i]);		for(list<PackedEncodedData *>::iterator packIt=packlist.begin();packIt!=packlist.end();packIt++)			delete *packIt;		delete [] packedData;		}		//models are deleted at a higher level	delete [] mods;	delete [] destination;	delete modelMixingParam;}