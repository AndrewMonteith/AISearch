#pragma once

template <typename T>
class SquareMatrix {
	// Matrix maintains the symmetric property.
public:
	SquareMatrix(int dimension, T defaultValue = 0)
	{
		this->dimension = dimension;
		buffer = std::vector<T>(dimension*dimension, defaultValue);
	}

	T get(int x, int y) {
		return buffer[flatten(x, y)];
	}

	void set(int x, int y, T value) {
		buffer[flatten(x, y)] = value;
		buffer[flatten(y, x)] = value;
	}

	inline int flatten(int x, int y) {
		return y * dimension + x;
	}

	std::vector<T>& rawBuffer() {
		return buffer;
	}

	void update(int x, int y, T update) {
		buffer[flatten(x, y)] += update;
		buffer[flatten(y, x)] += update;
	}

private:
	int dimension;
	std::vector<T> buffer;
};

