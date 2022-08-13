#include "global/Utilities/MyUtilities.h"

#include "Algo/Find.h"

void UMyUtilities::FindByValue(const TMap<FString, FString>& map, const FString& value, FString& ret, bool& succeeded)
{
	auto ptr = Algo::FindByPredicate(map, [&value](const auto& tuple){ return tuple.Value == value; });
	succeeded = ptr != nullptr;
	if (succeeded)
	{
		ret = ptr->Key;
	}
}