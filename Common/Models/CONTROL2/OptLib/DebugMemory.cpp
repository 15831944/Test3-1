#include <stdafx.h>
//	This is meant to be a database of calls to allocate and destroy memory
//	to see if leaks are occurring.
//	
#include <string.h>

#include "DebugMemory.h"
//-----------------------------------------------------------------------------
CMemTransaction	Transactions[MAX_TRANSACTIONS];
char					Users[MAX_USERS][MAX_STR_SIZE+1];	//	Only 1st 32 char's recorded
CMemTransaction	DeleteWithoutNew[1000];
CMemTransaction	NewWithoutDelete[1000];

int					TransactionNo		= 0;	// to indicate the order the operations occurred
int					TransactionCount	= 0;	//	The number of CMemTransactions recorded
int					UserCount			= 0;	//	The number of valid user strings.
int					DeleteWithoutNewCount = 0;
int					NewWithoutDeleteCount = 0;


//-----------------------------------------------------------------------------
bool FindUser(pchar UserStr, int & index);
int GetUser(const pchar UserStr);
//-----------------------------------------------------------------------------
int DebugMemAllocate(pvoid Address, const pchar Description)
{
	int	index;
   int   debugTransactionCount = 164;

   if (TransactionCount == debugTransactionCount)
      index = 54;

	if (TransactionCount >= MAX_TRANSACTIONS)
		return -1;

	Transactions[TransactionCount].Address = Address;
	Transactions[TransactionCount].CreateUserIndex = index = GetUser(Description);
	Transactions[TransactionCount].CreateTransaction = TransactionNo;

	TransactionNo++;
	TransactionCount++;

	return index;
}

//-----------------------------------------------------------------------------
int DebugMemDestroy(pvoid address, const pchar Description)
{
	bool	addrFound = false;
	int	index;
   int   i;

	index = GetUser(Description);

	//	Now we search for the create transaction
	for (i = 0; i < TransactionCount; i++)
	{
		if (address == Transactions[i].Address)
		{
			//	We have to make sure this transaction is not an old
			// one using the same memory that completed successfully.
			if (Transactions[i].DestroyUserIndex < 0)
			{
				addrFound = true;
				break;
			}
		}
	}

	if (addrFound)
	{
		Transactions[i].DestroyUserIndex = index;
		Transactions[i].DestroyTransaction = TransactionNo;
	}	//	IF
	else
	{
		if (DeleteWithoutNewCount < 1000)
		{
			DeleteWithoutNew[DeleteWithoutNewCount].DestroyUserIndex = index;
			DeleteWithoutNew[DeleteWithoutNewCount].DestroyTransaction = TransactionNo;
			DeleteWithoutNew[DeleteWithoutNewCount].Address = address;
			DeleteWithoutNewCount++;
		}	//	IF
	}	//	ELSE

	TransactionNo++;

	return index;
}
//-----------------------------------------------------------------------------
bool FindUser(pchar UserStr, int & index)
{
	for (int i = 0; i < UserCount; i++)
	{
		if (strcmp(Users[i], UserStr) == 0)
		{
			index = 1;
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
int GetUser(const pchar UserStr)
{
	bool	userFound = false;
   int   i;

	for (i = 0; i < UserCount; i++)
	{
		if (strcmp(Users[i], UserStr) == 0)
		{
			userFound = true;
			break;
		}
	}

	if (userFound)
	{
		return i;
	}
	else
	{
		if (UserCount >= MAX_USERS)
			return -1;
		strncpy(Users[UserCount], UserStr, MAX_STR_SIZE);
		UserCount++;
		return UserCount-1;
	}

}
//-----------------------------------------------------------------------------
int NumberOfDeleteWithoutAllocates(void)
{
	return DeleteWithoutNewCount;
}
//-----------------------------------------------------------------------------
int GetUnfinishedTransactionsCount(void)
{
	int	i;

	NewWithoutDeleteCount = 0;

	for (i = 0; i < TransactionCount; i++)
	{
		if (Transactions[i].DestroyUserIndex == -1)
		{
			if (NewWithoutDeleteCount < 1000)
			{
				NewWithoutDelete[NewWithoutDeleteCount] = Transactions[i];
				NewWithoutDeleteCount++;
			}
		}
	}
	return NewWithoutDeleteCount;
}