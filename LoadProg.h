extern unsigned int ProgStartAdr;
extern unsigned int ProgLoadAdr;
extern unsigned int PortionOffset;
extern unsigned int PortionSize;
extern unsigned char ProgRes;

extern unsigned char ProgTmpBuf[256];


extern int BytesWritten;
void UpdateProgramAction();
void InitLoadProg();
extern bool ProgNextPortion(unsigned int Address);
extern void StoreLoadTableAction();
extern void ChangeLoadTable();
typedef struct
{
	unsigned int ProgramDestinationAddress;
	unsigned int SourceTableAddress;
	unsigned int ProgramSectionSize;
}LoadTableElement;
