#include <ntddk.h>
#define assert DbgBreakPoint
#define dprintf DbgPrint
#define BOOL ULONG
#define TRUE 1
#define FALSE 0
typedef struct 
{
	volatile PULONG v;
	volatile LONG size;
	volatile LONG p;
} MagicList;
typedef struct
{
	volatile LONG readcount;
	volatile LONG writecount;
	MagicList v[256];
} MagicTable;

extern ULONG ver_major,ver_minor,ver_build;
extern PDRIVER_OBJECT drvobj;

VOID misc_get_sysver();
VOID magic_list_create(MagicList *plist);
VOID magic_list_free(MagicList *plist);
BOOL magic_list_add(MagicList *plist,ULONG v);
ULONG magic_list_query(MagicList *plist,ULONG v);
VOID magic_table_init(MagicTable *pt);
BOOL magic_table_add(MagicTable *pt,ULONG v);
ULONG magic_table_query(MagicTable *pt,ULONG v);
VOID magic_table_free(MagicTable *pt);
PVOID misc_get_func_address(PCWSTR fnname);
VOID magic_table_clear(MagicTable *pt);