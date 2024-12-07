// A numeric variable type that allows you to protect its modification by cheaters using, for example, Cheat Engine.


class pvar {
	protected string encryptedValue;
	protected string key;

	protected void generate_key() {
		key = string_to_hex(time_stamp_millis + string_base64_encode(random(343234, 3456985883)));
	}

	pvar()
	{
		generate_key();
	}

	// Encrypt or decrypt the value based on the operation type
	protected string processValue(const string&in operation, const string&in input) {
		if (operation == "e") {
			string encrypted = string_encrypt(input, hex_to_string(key));
			string base64Encoded = string_base64_encode(encrypted);
			return string_to_hex(base64Encoded);
		} else if (operation == "d") {
			string hexToString = hex_to_string(input);
			string base64Decoded = string_base64_decode(hexToString);
			return string_decrypt(base64Decoded, hex_to_string(key));
		}
		return "";
	}

	// Convert the encrypted value to a double
	double opImplConv() {
		return string_to_number(processValue("d", encryptedValue));
	}

	double getDecryptedValue() {
		return opImplConv();
	}

	void opAssign(double value) {
		encryptedValue = processValue("e", value);
	}

	void opAddAssign(double value) {
		double currentValue = getDecryptedValue();
		encryptedValue = processValue("e", currentValue + value);
	}

	void opSubAssign(double value) {
		double currentValue = getDecryptedValue();
		encryptedValue = processValue("e", currentValue - value);
	}

	void opMulAssign(double value) {
		double currentValue = getDecryptedValue();
		encryptedValue = processValue("e", currentValue * value);
	}

	void opDivAssign(double value) {
		double currentValue = getDecryptedValue();
		encryptedValue = processValue("e", currentValue / value);
	}

	void opPreInc() {
		double currentValue = getDecryptedValue();
		encryptedValue = processValue("e", currentValue + 1);
	}

	void opPreDec() {
		double currentValue = getDecryptedValue();
		encryptedValue = processValue("e", currentValue - 1);
	}

	void opPostInc() {
		opPreInc(); // Reuse pre-increment logic
	}

	void opPostDec() {
		opPreDec(); // Reuse pre-decrement logic
	}

	double opAdd(double value) {
		return value + getDecryptedValue();
	}

	double opSub(double value) {
		return getDecryptedValue() - value;
	}

	double opMul(double value) {
		return value * getDecryptedValue();
	}

	double opDiv(double value) {
		return getDecryptedValue() / value;
	}

	bool opEquals(double value) {
		return getDecryptedValue() == value;
	}

	int opCmp(double value) {
		double currentValue = getDecryptedValue();
		if (currentValue < value) return -1;
		if (currentValue == value) return 0;
		return 1;
	}

};
