  
namespace Event
{
    class Event
    {
    public:

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        Event(int fd);
        ~Event();
        int fd() const { return fd_; }
        int events() const { return events_; }
        void enableReading() { events_ |= kReadEvent; update(); }
        void disableReading() { events_ &= ~kReadEvent; update(); }
        void enableWriting() { events_ |= kWriteEvent; update(); }
        void disableWriting() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
    private:
        void update();
        const int  fd_;
        int  events_;
    };
} // namespace Event
  
 