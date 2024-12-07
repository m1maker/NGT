// A very primitive implementation of AI similarity

class linear_regression {
	private float m_slope;
	private float m_intercept;
	private int m_count;

	linear_regression() {
		m_slope = 0.0;
		m_intercept = 0.0;
		m_count = 0;
	}

	void fit(array<float> &in x, array<float> &in y) {
		if (x.length() != y.length()) {
			throw("Input arrays must have the same length");
		}

		int n = x.length();
		float sum_x = 0.0;
		float sum_y = 0.0;
		float sum_xy = 0.0;
		float sum_xx = 0.0;

		for (int i = 0; i < n; ++i) {
			sum_x += x[i];
			sum_y += y[i];
			sum_xy += x[i] * y[i];
			sum_xx += x[i] * x[i];
		}

		m_slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
		m_intercept = (sum_y - m_slope * sum_x) / n;
		m_count = n;
	}

	float predict(float x) {
		return m_slope * x + m_intercept;
	}

	float get_slope() const property {
		return m_slope;
	}

	float get_intercept() const property {
		return m_intercept;
	}
};


