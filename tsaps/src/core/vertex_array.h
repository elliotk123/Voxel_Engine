class VertexArray {
public:
	VertexArray(unsigned int bufferSize);
	void resetBuffer();
	__int16* getData() const;
	int getDataLength() const;
	void addValues(__int16 valX, __int16 valY);
	~VertexArray();

private:
	int m_bufferSize;  // total memory alocated 
	int m_bufferPosition; // starting at 0 current offest to write to 
	__int16* mp_data;

};