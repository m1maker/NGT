class combination
{
	protected int items;
	protected int combinationSize;
	protected array<int> currentCombination;
	bool repetitionMode;

	// Constructor
	combination()
	{
		items = 0;
		combinationSize = 0;
		repetitionMode = false;
	}

	// Generate all combinations with repetitions
	bool generate_all_combinations(int items, int combinationSize) {
		if (items < 2 || combinationSize < 2)
		{
			return false;
		}
		this.items = items;
		this.combinationSize = combinationSize;
		currentCombination.resize(combinationSize);
		for (int i = 0; i < combinationSize; ++i)
		{
			currentCombination[i] = 0;
		}
		repetitionMode = true;
		return true;
	}

	// Generate unique combinations
	bool generate_unique_combinations(int items, int combinationSize) {
		if (items < 3 || !(1 < combinationSize && combinationSize < items))
		{
			return false;
		}
		this.items = items;
		this.combinationSize = combinationSize;
		currentCombination.resize(combinationSize);
		for (int i = 0; i < combinationSize; ++i)
		{
			currentCombination[i] = i;
		}
		repetitionMode = false;
		return true;
	}

	// Get the next combination
	array<int> next()
	{
		if (repetitionMode)
		{
			return nextAllCombinations();
		}
		else
		{
			return nextUniqueCombinations();
		}
	}

	// Reset the state
	void reset() {
		items = 0;
		combinationSize = 0;
		currentCombination.resize(0);
	}

	// Generate the next combination for all combinations with repetitions
	protected array<int> nextAllCombinations()
	{
		for (int i = combinationSize - 1; i >= 0; --i)
		{
			if (currentCombination[i] < items - 1)
			{
				currentCombination[i]++;
				return currentCombination;
			}
			else
			{
				currentCombination[i] = 0;
			}
		}
		return array<int>(); // Return empty array if no more combinations
	}

	// Generate the next unique combination
	protected array<int> nextUniqueCombinations()
	{
		for (int i = combinationSize - 1; i >= 0; --i)
		{
			if (currentCombination[i] < items - 1)
			{
				currentCombination[i]++;
				for (int j = i + 1; j < combinationSize; ++j)
				{
					currentCombination[j] = currentCombination[j - 1] + 1;
				}
				return currentCombination;
			}
			else
			{
				if (i == 0)
				{
					return array<int>(); // Return empty array if no more combinations
				}
				else
				{
					currentCombination[i] = i;
				}
			}
		}
		return array<int>(); // Return empty array if no more combinations
	}
}
