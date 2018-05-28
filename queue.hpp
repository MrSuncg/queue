
#include <cstddef>
#include <string>
#include <iostream>
#include <cstring>

template<class EleT>
class PipeStream {
public:
	explicit PipeStream(size_t capacity);

	~PipeStream();

	// 删除下面几个函数
	PipeStream(const PipeStream &) = delete;

	PipeStream(const PipeStream &&) = delete;

	PipeStream &operator=(const PipeStream &&) = delete;

	PipeStream &operator=(const PipeStream &) = delete;

	/**
	 *
	 * @return 缓冲中还有多少元素可读取
	 */
	int Available() const;

	/**
	 * 从流中读取若干个元素.
	 * @return 实际读取到的字节数
	 */
	int Read(EleT *buffer, size_t size);

	/**
	 * 向流中写入若干数据
	 * @param data
	 * @param size
	 * @return 实际写入的数据量
	 */
	int Write(EleT *data, size_t size);

	void showQueue()
	{
		std::cout << "showQueue: " << std::endl;
		std::cout << "length: " << length << std::endl;
		std::cout << "front: " << front << "  tail: " << tail << std::endl;
		if (tail == front && length == 0) {
			std::cout << "empty";
		} else {
			if (tail > front) {
				for (auto i = front; i < tail; i++) {
					std::cout << q[i] << " ";
				}
			} else {
				for (auto i = front; i < capacity; i++) {
					std::cout << q[i] << " ";
				}
				for (auto i = 0; i < tail; i++) {
					std::cout << q[i] << " ";
				}
			}
		}
		std::cout << std::endl;
	}

	void actuallQueueShow()
	{
		std::cout << "actuallyQueueShow: " << std::endl;
		for (int i = 0; i < capacity; i++) {
			std::cout << q[i] << " ";
		}
		std::cout << std::endl;
	}

private:
	EleT *q;
	size_t capacity;
	size_t front, tail;
	size_t length;
};

template<class EleT>
PipeStream<EleT>::PipeStream(size_t capacity)
{
	q = new EleT[capacity];
	this->capacity = capacity;
	this->front = this->tail = 0;
	this->length = 0;
}

template<class EleT>
PipeStream<EleT>::~PipeStream()
{
	this->capacity = 0;
	this->front = 0;
	this->tail = 0;
	this->length = 0;
	delete[] q;
	q = nullptr;
}

template<class EleT>
int PipeStream<EleT>::Available() const
{
	return int(this->length);
}

template<class EleT>
int PipeStream<EleT>::Write(EleT *data, size_t size)
{
	if (data == nullptr) return 0;
	//full
	if (front == tail && length == capacity) return 0;
	if (tail < front) {
		size_t copyNum = 0, tmpTail = tail;
		//空间足够
		if (front - tail > size) {
			copyNum = size;
			tmpTail += size;
		} else {
			copyNum = front - tail;
			tmpTail = front;
		}
		memcpy(q + tail, data, copyNum * sizeof(EleT));
		tail = tmpTail;
		length += copyNum;
		return (int) copyNum;
	} else {
		//先填充tail到capacity的位置，
		size_t copyNum = 0, tmpTail = tail, copyNum2 = 0;
		//空间足
		if (capacity - tail > size) {
			copyNum = size;
			tmpTail += size;
		} else {
			copyNum = capacity - tail;
			tmpTail = 0;
		}
		memcpy(q + tail, data, copyNum * sizeof(EleT));
		tail = tmpTail;

		size -= copyNum;
		length += copyNum;
		if (size == 0) return (int) copyNum;
		//再填充0到front的位置
		if (front - tail > size) {
			copyNum2 = size;
			tmpTail = tail + size;
		} else {
			copyNum2 = front - tail;
			tmpTail = front;
		}
		//data前面已经被copy了copyNum个元素
		memcpy(q + tail, data + copyNum, copyNum2 * sizeof(EleT));
		tail = tmpTail;
		length += copyNum2;
		return (int) (copyNum + copyNum2);
	}
}

template<class EleT>
int PipeStream<EleT>::Read(EleT *buffer, size_t size)
{
	if (buffer == nullptr) return 0;
	//empty
	if (front == tail && length == 0) return 0;
	if (tail > front) {
		size_t copyNum = 0;
		if (tail - front >= size) {
			copyNum = size;
			memcpy(buffer, q + front, copyNum * sizeof(EleT));
			front += size;
		} else {
			copyNum = tail - front;
			memcpy(buffer, q + front, copyNum * sizeof(EleT));
			front = tail;
		}
		length -= copyNum;
		return (int) copyNum;
	} else {
		size_t copyNum = 0, copyNum2 = 0;
		if (capacity - front > size) {
			copyNum = size;
			memcpy(buffer, q + front, copyNum * sizeof(EleT));
			front += size;
			length -= copyNum;
			return (int) copyNum;
		} else {
			//先copy front到capacity的元素，再copy 0-tail的元素
			copyNum = capacity - front;
			memcpy(buffer, q + front, copyNum * sizeof(EleT));
			front = 0;
			size -= copyNum;
			length -= copyNum;
			if (size == 0) return (int) copyNum;
			if (tail - front >= size) {
				copyNum2 = size;
			} else {
				copyNum2 = tail - front;
			}
			length -= copyNum2;
			memcpy(buffer + copyNum, q + front, copyNum2 * sizeof(EleT));
			front += copyNum2;
			return (int) (copyNum + copyNum2);
		}
	}
}
