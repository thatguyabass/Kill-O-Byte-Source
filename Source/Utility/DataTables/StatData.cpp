
#include "Snake_Project.h"
#include "StatData.h"

void FDataTableName::SelectName()
{
	if (TableNames.IsValidIndex(NameIndex))
	{
		RowName = TableNames[NameIndex];
	}
}