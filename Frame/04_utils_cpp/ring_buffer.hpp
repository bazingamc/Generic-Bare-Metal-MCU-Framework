template<typename T>
class RingBuffer
{
public:
    explicit RingBuffer(size_t size)
        : size_(size), head_(0), tail_(0)
    {
        buffer_ = (T*)malloc(sizeof(T) * size_);
    }

    ~RingBuffer()
    {
        if (buffer_) free(buffer_);
    }

    bool push(const T& item)
    {
        size_t next = (head_ + 1) % size_;
        if (next == tail_) return false;   // Full
        buffer_[head_] = item;
        head_ = next;
        return true;
    }

    bool pop(T& item)
    {
        if (head_ == tail_) return false;  // Empty
        item = buffer_[tail_];
        tail_ = (tail_ + 1) % size_;
        return true;
    }

    bool empty() const { return head_ == tail_; }

private:
    T* buffer_;
    size_t size_;
    volatile size_t head_;
    volatile size_t tail_;
};