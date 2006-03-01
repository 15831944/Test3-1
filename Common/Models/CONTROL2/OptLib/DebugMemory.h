#ifndef DEBUG_MEMORY_H
#define DEBUG_MEMORY_H
//-----------------------------------------------------------------------------
#ifdef DEBUG_MEMORY
//-----------------------------------------------------------------------------

typedef void * pvoid;
typedef char * pchar;
//-----------------------------------------------------------------------------
class CMemTransaction
{
	public:
		int	CreateUserIndex;
		int	DestroyUserIndex;
		int	CreateTransaction;
		int	DestroyTransaction;
		void	*Address;

		CMemTransaction()
		{
			CreateUserIndex = -1;
			DestroyUserIndex = -1;
			CreateTransaction = -1;
			DestroyTransaction = -1;
			Address = NULL;
		};
};

//-----------------------------------------------------------------------------
const int MAX_TRANSACTIONS	= 10000;
const int MAX_USERS = 1000;
const int MAX_STR_SIZE = 32;

//-----------------------------------------------------------------------------
extern CMemTransaction	Transactions[MAX_TRANSACTIONS];
extern char					Users[MAX_USERS][MAX_STR_SIZE+1];	//	Only 1st 32 char's recorded
extern CMemTransaction	DeleteWithoutNew[1000];
extern CMemTransaction	NewWithoutDelete[1000];

extern int	TransactionNo;				// to indicate the order the operations occurred
extern int	TransactionCount;			//	The number of CMemTransactions recorded
extern int	UserCount;					//	The number of valid user strings.
extern int	DeleteWithoutNewCount;
extern int	NewWithoutDeleteCount;


//-----------------------------------------------------------------------------
int DebugMemAllocate(pvoid address, const pchar Description);
int DebugMemDestroy(pvoid address, const pchar Description);
int NumberOfDeleteWithoutAllocates(void);
int GetUnfinishedTransactionsCount(void);
//-----------------------------------------------------------------------------
#endif DEBUG_MEMORY
//-----------------------------------------------------------------------------
#endif DEBUG_MEMORY_H