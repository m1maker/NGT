class instance {
	protected bool already_locked = false;
	protected named_mutex@ mtx = null;
	protected string mtx_name;

	// Constructor: Initializes the named mutex and attempts to lock it
	instance(const string& in app_id = "") {
		mtx_name = app_id;
		@mtx = named_mutex(mtx_name);
		try {
			already_locked = mtx.try_lock(); // Lock if this is the first instance
		} catch {
			// Handle exception silently; mutex may already be locked
		}
	}

	// Destructor: Attempts to unlock the mutex if it was locked by this instance
	~instance() {
		try {
			mtx.unlock();
		} catch {
			// Ignore exception if the mutex is locked by another process
		}
		@mtx = null; // Clean up the mutex reference
	}

	// Function to check if another instance is already running
	bool is_running() {
		if (already_locked) return false; // This instance successfully locked the mutex

		try {
			return !mtx.try_lock(); // Check if another instance has locked the mutex
		} catch {
			// Handle abandoned mutex scenario
			recreate();
			return false;
		}
	}

	// Waits until this instance can lock the mutex (become standalone)
	void wait_until_standalone() {
		try {
			mtx.lock(); // Block until the mutex can be locked
		} catch {
			// Handle abandoned mutex scenario
			recreate();
			try {
				mtx.try_lock(); // Attempt to lock after recreation
			} catch {
				// Ignore any exceptions during this attempt
			}
		}
	}

	// Helper function to recreate the named mutex
	protected void recreate() {
		@mtx = named_mutex(mtx_name);
		already_locked = false; // Reset the locked state since we recreated the mutex
	}
};

