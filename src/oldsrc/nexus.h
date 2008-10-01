#ifndef __NEXUS_H#define __NEXUS_Hclass Nexus;//// NexusBlock class//class NexusBlock{	friend class Nexus;protected:	bool isEmpty;	Nexus* nexus;	NexusBlock* next;	nxsstring id;	virtual void Read( NexusToken& token ) = 0;	virtual void Reset() = 0;public:	nxsstring errormsg;	NexusBlock();	~NexusBlock();		void SetNexus( Nexus* nxsptr );	nxsstring GetID();	bool IsEmpty();   virtual int CharLabelToNumber( nxsstring s );   virtual int TaxonLabelToNumber( nxsstring s );	virtual void SkippingCommand( nxsstring commandName );	virtual void Report( ostream& out ) = 0;};//// Nexus class//class Nexus{protected:	NexusBlock* blockList;public:	Nexus();	~Nexus();	bool BlockListEmpty();	void Add( NexusBlock* newBlock );	void Detach( NexusBlock* newBlock );	void Execute( NexusToken& token );	virtual void DebugReportBlock( NexusBlock& nexusBlock );		char* NCLNameAndVersion();	char* NCLCopyrightNotice();	char* NCLHomePageURL();	// hooks implemented as pure virtual functions	virtual void ExecuteStarting() = 0;	virtual void ExecuteStopping() = 0;	virtual void EnteringBlock( nxsstring blockName ) = 0;	virtual void OutputComment( nxsstring comment ) = 0;	virtual void NexusError( nxsstring& msg, streampos pos, long line, long col ) = 0;	virtual void SkippingBlock( nxsstring blockName ) = 0;};#endif