#include <vertex_array.h>
#include <iostream>

VertexArray::VertexArray(unsigned int bufferSize) {
	mp_data = new __int16[bufferSize];
	m_bufferSize = bufferSize;
	m_bufferPosition = 0;
}

void VertexArray::resetBuffer() {
	m_bufferPosition = 0;
}

__int16* VertexArray::getData() const {
	return mp_data;
}

int VertexArray::getDataLength() const {
	return m_bufferPosition;
}

void VertexArray::addValues(__int16 valX, __int16 valY) {
	if (m_bufferPosition < m_bufferSize - 2) {
		mp_data[m_bufferPosition] = valX;
		mp_data[m_bufferPosition + 1] = valY;
		m_bufferPosition = m_bufferPosition + 2;
	}
	else {
		std::cout << "Cannot add values " << valX << ", " << valY << ". Reason: Buffer full";
	}

}

VertexArray::~VertexArray() {
	delete[] mp_data;
}